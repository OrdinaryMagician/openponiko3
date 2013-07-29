/*
	ponikofn.c : Poniko functions.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#include <time.h>
#include <string.h>
#include "ponikocfg.h"
#include "ircbase.h"
#include "shoutbot.h"
#include "ponikofn.h"
#include <strings.h>
#include <stdio.h>
/* internal vars */
static time_t laststamp = 0;
static int spamthreshold = 5;
/* builtin commands */
static int rawcmd( int sock, char **user, char *msg, char *replyto )
{
	if ( strcmp(user[2],cfg.owner) )
		return ircsend(sock,"PRIVMSG %s "
			":Warning: %s is not in the sudoers file, "
			"this incident will be reported",replyto,user[0]);
	char *parm = strchr(msg,' ');
	return parm?ircsend(sock,parm+1):0;
}
static int who( int sock, char **user, char *msg, char *replyto )
{
	char *parm = strchr(msg,' ');
	if ( !parm || !(++parm) )
		return ircsend(sock,"NOTICE %s "
			":no parameter specified",user[0]);
	if ( !strcmp(parm,"last") )
	{
		if ( !shout_q.id )
			return ircsend(sock,"PRIVMSG %s "
				":%s, I haven't shouted yet :/",replyto,user[0]);
		char timestr[256];
		strftime(timestr,256,"%a %d/%m/%Y %H:%M:%S",
			localtime((time_t*)&shout_q.epoch));
		if ( !strcmp(shout_q.channel,"UNKNOWN") )
			return ircsend(sock,"PRIVMSG %s "
				":%s, %s said that on %s",replyto,user[0],
				shout_q.name,timestr);
		return ircsend(sock,"PRIVMSG %s "
			":%s, %s said that at %s on %s",replyto,user[0],
			shout_q.name,shout_q.channel,timestr);
	}
	quote_t got = {0,0,{0},{0},{0}};
	shout_get_key(parm,&got);
	if ( !got.id )
		return ircsend(sock,"PRIVMSG %s "
			":%s, no one said that",replyto,user[0]);
	char timestr[256];
	strftime(timestr,256,"%a %d/%m/%Y %H:%M:%S",
		localtime((time_t*)&got.epoch));
	if ( !strcmp(got.channel,"UNKNOWN") )
		return ircsend(sock,"PRIVMSG %s "
			":%s, %s said that on %s",replyto,user[0],
			got.name,timestr);
	return ircsend(sock,"PRIVMSG %s "
		":%s, %s said that at %s on %s",replyto,user[0],
		got.name,got.channel,timestr);
}
static int tidy( int sock, char **user, char *msg, char *replyto )
{
	/* silence warnings */
	msg = 0;
	if ( strcmp(user[2],cfg.owner) )
		return ircsend(sock,"PRIVMSG %s "
			":Warning: %s is not in the sudoers file, "
			"this incident will be reported",replyto,user[0]);
	shout_tidy();
	/* TODO
	lseen_tidy();
	blist_tidy();
	memos_tidy();
	*/
	return ircsend(sock,"PRIVMSG %s "
			":Full database vacuum finished.");
}
static int count( int sock, char **user, char *msg, char *replyto )
{
	char *parm = strchr(msg,' ');
	/* assume caller if no parameter passed */
	if ( !parm || !(++parm) )
		parm = user[0];
	long long numbered = 0;
	if ( shout_countname(parm,&numbered) )
		return ircsend(sock,"PRIVMSG %s "
			":An error occured in the sqlite3 interface, "
			"please contact owner",replyto);
	return ircsend(sock,"PRIVMSG %s "
			":There is a total of %lld quotes recorded from %s",
			replyto,numbered,parm);
}
static int countall( int sock, char **user, char *msg, char *replyto )
{
	/* silence warnings */
	user = 0;
	msg = 0;
	long long numbered = 0;
	if ( shout_countall(&numbered) )
		return ircsend(sock,"PRIVMSG %s "
			":An error occured in the sqlite3 interface, "
			"please contact owner",replyto);
	return ircsend(sock,"PRIVMSG %s "
			":I have a total of %lld quotes stored",
			replyto,numbered);
}
static int countkey( int sock, char **user, char *msg, char *replyto )
{
	char *parm = strchr(msg,' ');
	if ( !parm || !(++parm) )
		return ircsend(sock,"NOTICE %s "
			":no parameter specified",user[0]);
	long long numbered = 0;
	if ( shout_countkey(parm,&numbered) )
		return ircsend(sock,"PRIVMSG %s "
			":An error occured in the sqlite3 interface, "
			"please contact owner",replyto);
	return ircsend(sock,"PRIVMSG %s "
			":%lld quotes found for specified key",
			replyto,numbered,parm);
}
static int countchan( int sock, char **user, char *msg, char *replyto )
{
	char *parm = strchr(msg,' ');
	if ( !parm || !(++parm) )
		return ircsend(sock,"NOTICE %s "
			":no parameter specified",user[0]);
	long long numbered = 0;
	if ( shout_countchan(parm,&numbered) )
		return ircsend(sock,"PRIVMSG %s "
			":An error occured in the sqlite3 interface, "
			"please contact owner",replyto);
	return ircsend(sock,"PRIVMSG %s "
			":%lld quotes found for specified channel",
			replyto,numbered,parm);
}
static int poke( int sock, char **user, char *msg, char *replyto )
{
	/* silence warnings */
	msg = 0;
	user = 0;
	shout_get();
	if ( shout_q.id )
		return ircsend(sock,"PRIVMSG %s :%s",replyto,shout_q.line);
	return 0;
}
static int recall( int sock, char **user, char *msg, char *replyto )
{
	/* silence warnings */
	msg = 0;
	if ( shout_q.id )
		return ircsend(sock,"PRIVMSG %s "
			":%s, I haven't shouted yet :/",replyto,user[0]);
	return ircsend(sock,"PRIVMSG %s "
		":%s, my last shout was: %s",replyto,user[0],shout_q.line);
}
/* builtin lists */
/*
    * rawcommand: execute raw IRC command
    * who: print who said which quote ("last" for last shouted)
    - forget: forget a quote
    - forgetbyname: forget all quotes from a user
    - brainbleach: confirm forget
    * tidy: clean up and vacuum databases
    * count: count quotes by user
    * countall: count all quotes
    * countchan: count all quotes on a channel
    * countkey: count quotes by case insensitive key
    - threshold: set spam detection threshold
    - (un)ban: add or remove user from blacklist
    - (no)save: toggle quote saving
    - (no)shout: toggle shouting
    - (no)autoban: toggle automatic blacklisting after 3 spam triggers
    - seen: when a user was last seen and what did they last say
    - whereis: check which channels a user is on
    * poke: force shout
    * recall: repeat last shout
*/
char *builtin_names[] =
{
	"rawcommand",
	"who",
	"tidy",
	"countall",
	"countkey",
	"countchan",
	"count",
	"poke",
	"recall",
	NULL,
};
builtinfn_t builtin_funcs[] =
{
	rawcmd,
	who,
	tidy,
	countall,
	countkey,
	countchan,
	count,
	poke,
	recall,
	NULL,
};
/* spam protector */
static int flood_detected( void )
{
	time_t current = time(NULL);
	int ret = (botflags&BOT_DELAY) && ((current-laststamp) < spamthreshold);
	laststamp = current;
	return ret;
}
/* parse message from irc user */
int parsemesg( int sock, char **user, char *msg, char *replyto )
{
	/*
	   TODO list:
	    - Add more builtins
	    - External command support
	    - Blacklist support
	    - URL handling
	*/
	/* some important internals */
	if ( !strncasecmp(msg,"\001PING ",6) )
		return (ircsend(sock,"NOTICE %s :%s",replyto,msg)
			||putchar('\a'))&0;
	if ( !strcasecmp(msg,"\001TIME\001") )
	{
		char timestr[256];
		time_t tt = time(NULL);
		strftime(timestr,256,"%a %d/%m/%Y %H:%M:%S",localtime(&tt));
		return ircsend(sock,"NOTICE %s :%s",replyto,timestr)&0;
	}
	if ( !strcasecmp(msg,"\001VERSION\001") )
		return ircsend(sock,"NOTICE %s :%s",replyto,
			"OpenPONIKO 3.0 - Compiled on " __DATE__ " " __TIME__)&0;
	if ( *msg != '!' )
		goto skipcmd;
	int i, len;
	for ( i=0; builtin_funcs[i]; i++ )
	{
		len = strlen(builtin_names[i]);
		if ( strncasecmp(msg+1,builtin_names[i],len) )
			continue;
		if ( flood_detected() )
			goto skipall;
		return builtin_funcs[i](sock,user,msg+1,replyto);
	}
skipcmd:
	if ( strpbrk(msg,"abcdefghijklmnopqrstuvwxyz") || (strlen(msg) < 10)
	     || !(botflags&BOT_SHOUT) )
		goto skipshout;
	if ( flood_detected() )
		goto skipall;
	shout_get();
	if ( shout_q.id )
		ircsend(sock,"PRIVMSG %s :%s",replyto,shout_q.line);
	if ( botflags&BOT_RECORD )
		shout_save(user[0],replyto,msg);
skipshout:
skipall:
	return 0;
}
