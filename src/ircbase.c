/*
	ircbase.c : Base IRC handling.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#include "common.h"
#include "ircbase.h"
#include "ponikofn.h"
#include "ponikocfg.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
/* shoutbot variables (all on by default) */
unsigned char botflags = 0xFF;
/* other flags */
char active = 0;
char autojoined = 0;
/* connection variables */
static int sock;
static struct sockaddr_in s_in;
static struct hostent *he;
/* send data */
int ircsend( int ssock, const char *fmt, ... )
{
	if ( !fmt )
		return -1;
	/* format to sendbuf */
	char sendbuf[512];
	va_list arg;
	va_start(arg,fmt);
	vsnprintf(sendbuf,512,fmt,arg);
	va_end(arg);
	/* trim if line is too long */
	if ( strlen(sendbuf) > 350 )
	{
		sendbuf[350] = '\0';
		strcat(sendbuf,"...");
	}
	ptime("[%Y/%m/%d %H:%M:%S]",NULL);
	printf(" >>> %s\n",sendbuf);
	/* special check, autoquit */
	if ( !strncasecmp(strcat(sendbuf,"\r\n"),"QUIT",4) )
		active = 0;
	/* time to send */
	return send(ssock,sendbuf,strlen(sendbuf),0);
}
/* receive data */
int ircget( int ssock, char *to, int max )
{
	int res = 0, restotal = 0, i = 0;
	char tmp = 0;
	do
	{
		res = recv(ssock,&tmp,1,0);
		/* connection error, possibly */
		/* will implement reconnecting later, just quit for now */
		if ( res < 0 )	
			return (active=0)-1;
		if ( res > 0 )
		{
			restotal += res;
			if ( i >= max )
				return -2;	/* buffer too small */
			to[i] = tmp;
			to[i+1] = 0;
			if ( (i > 0) && (to[i-1] == '\r') && (to[i] == '\n') )
				return restotal;
			i++;
		}
	}
	while ( res > 0 );
	return restotal;
}
/* forced quit with SIGTERM */
static void ircterm()
{
	ircsend(sock,"QUIT");
	active = 0;
}
/* open irc connection */
int ircopen( void )
{
	/* make socket and other data */
	if ( (sock = socket(PF_INET,SOCK_STREAM,0)) < 0 )
		bail("ircopen error: %s\n",strerror(errno));
        memset(&s_in,0,sizeof(struct sockaddr_in));
	s_in.sin_family = PF_INET;
	s_in.sin_port = htons(atoi(cfg.port));
	if ( (he = gethostbyname(cfg.server)) == NULL )
		bail("ircopen error: %s\n",strerror(h_errno));
	memcpy(&s_in.sin_addr.s_addr,he->h_addr_list[0],he->h_length);
	if ( connect(sock,(struct sockaddr*)&s_in,sizeof(s_in)) < 0 )
		bail("ircopen error: %s\n",strerror(h_errno));
	ircsend(sock,"USER %s %s %s :%s",cfg.user,cfg.user,cfg.server,cfg.name);
	ircsend(sock,"NICK %s",cfg.nick);
	signal(SIGTERM,ircterm);
	return (active=1)&0;
}
/* close irc connection */
int ircclose( void )
{
	if ( sock )
		ircsend(sock,"QUIT");
	if ( (shutdown(sock,SHUT_RDWR) < 0) && (errno != ENOTCONN) )
		return bail("ircclose error: %s\n",strerror(errno));
	if ( close(sock) )
		return bail("ircclose error: %s\n",strerror(errno));
	return (active=0);
}
/* process irc connection */
void process( void )
{
	char getbuf[512];
	int len = ircget(sock,getbuf,512);
	if ( len < 0 )
		return;
	getbuf[len-2] = 0;	/* trim crlf */
	ptime("[%Y/%m/%d %H:%M:%S]",NULL);
	printf(" <<< %s\n",getbuf);
	/* generate argument list */
	char *s, *t, **av, *source, *c, *temp;
	int ac;
	if ( !(temp = strdup(getbuf)) )
		return;
	ac = 0;
	s = ((*temp == ':')?temp+1:temp);
	av = malloc(sizeof(char*)*(++ac));
	for ( ;; )
	{
		av[ac-1] = (*s != ':')?s:s+1;
		if ( (t=strchr(s,' ')) && (*s!=':') )
			*t = 0;
		else
			break;
		while( *(++t) == ' ' );
		s = t;
		av = realloc(av,sizeof(char*)*(++ac));
	}
	source = (*temp == ':')?(av++)[0]:NULL;
	c = (av++)[0];
	ac -= source?2:1;
	/* reply to pings */
	if ( !strcmp(c,"PING") )
		ircsend(sock,"PONG :%s",av[0]);
	/* handle quits */
	else if ( !strcmp(c,"ERROR")
		  && strstr(av[0],"Closing Link")
		  && strstr(av[0],"Quit:") )
		active = 0;
	/* check for notices about our vhost */
	else if ( !strcmp(c,"NOTICE") && !autojoined )
	{
		if ( (!strncmp(source,"HostServ!",9)
		     || !strncmp(source,"NickServ!",9))
		     && strstr(av[1],"Your vhost of")
		     && strstr(av[1],"is now activated.") )
		{
			ircsend(sock,"JOIN %s",cfg.chan);
			autojoined = 1;
		}
	}
	/* we have to identify for this nick */
	else if ( !strcmp(c,"376") || !strcmp(c,"422") )
		ircsend(sock,"PRIVMSG NickServ :IDENTIFY %s",cfg.pass);
	/* attempt to ghost if nick is in use */
	else if ( !strcmp(c,"433") )
	{
		if ( strstr(av[1],cfg.nick) )
		{
			ircsend(sock,"PRIVMSG NickServ :GHOST %s %s",cfg.nick,
				cfg.pass);
			ircsend(sock,"NICK %s",cfg.nick);
			ircsend(sock,"PRIVMSG NickServ :IDENTIFY %s",cfg.pass);
		}
	}
	/* actual messaging here */
	else if ( !strcmp(c,"PRIVMSG" ) )
	{
		char *utmp = strdup(source);
		char *user[3];
		char *div;
		user[0] = utmp;
		div = strchr(utmp,'!');
		if ( div )
			*(div++) = 0;
		user[1] = div;
		div = strchr(div,'@');
		if ( div )
			*(div++) = 0;
		user[2] = div;
		char *chan = av[0];
		char *replyto = strcasecmp(chan,cfg.nick)?chan:user[0];
		char *msg = av[1];
		if ( parsemesg(sock,user,msg,replyto) < 0 )
			active = 0;
		free(utmp);
	}
	free(source?av-2:av-1);
	free(temp);
}
