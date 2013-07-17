/*
	ponikofn.c : Poniko functions.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#include "common.h"
#include "ircbase.h"
#include "ponikofn.h"
#include "ponikocfg.h"
#include "shoutbot.h"
#include <string.h>
#include <time.h>
/* builtin commands */
static int rawcmd( int sock, char **user, char *msg, char *replyto )
{
	if ( strcmp(user[2],cfg.owner) )
	{
		ircsend(sock,"PRIVMSG %s "
			":Warning: %s is not in the sudoers file, "
			"this incident will be reported.",replyto,user[0]);
		return 0;
	}
	char *parm = strchr(msg,' ');
	if ( parm++ )
		ircsend(sock,parm);
	return 0;
}
/* builtin lists */
/*
    - rawcommand: execute raw IRC command
    - who: print who said which quote ("last" for last shouted)
    - forget: forget a quote
    - forgetbyname: forget all quotes from a user
    - brainbleach: confirm forget
    - tidy: clean up and vacuum databases
    - count: count quotes by user
    - countall: count all quotes
    - countkey: count quotes by case insensitive key
    - threshold: set spam detection threshold
    - (un)ban: add or remove user from blacklist
    - (no)save: toggle quote saving
    - (no)shout: toggle shouting
    - (no)autoban: toggle automatic blacklisting after 3 spam triggers
    - seen: when a user was last seen and what did they last say
    - whereis: check which channels a user is on
    - poke: force shout
    - recall: repeat last shout
*/
char *builtin_names[] =
{
	"rawcommand",
	NULL,
};
builtinfn_t builtin_funcs[] =
{
	rawcmd,
	NULL,
};
/* parse message from irc user */
int parsemesg( int sock, char **user, char *msg, char *replyto )
{
	/*
	   TODO list:
	    - Add more builtins
	    - External command support
	    - Shouting
	    - Blacklist support
	    - URL handling
	*/
	/* some important internals */
	if ( !strncasecmp(msg,"\001PING ",6) )
	{
		ircsend(sock,"NOTICE %s :%s",replyto,msg);
		putchar('\a');
		return 0;
	}
	if ( !strcasecmp(msg,"\001TIME\001") )
	{
		char timestr[256];
		time_t tt = time(NULL);
		strftime(timestr,256,"%a %d/%m/%Y %H:%M:%S",localtime(&tt));
		ircsend(sock,"NOTICE %s :%s",replyto,timestr);
		return 0;
	}
	if ( !strcasecmp(msg,"\001VERSION\001") )
	{
		ircsend(sock,"NOTICE %s :%s",replyto,
			"OpenPONIKO 3.0 - Compiled on " __DATE__ " " __TIME__);
		return 0;
	}
	if ( *msg != '!' )
		goto skipcmd;
	int i, len;
	for ( i=0; builtin_funcs[i]; i++ )
	{
		len = strlen(builtin_names[i]);
		if ( strncasecmp(msg+1,builtin_names[i],len) )
			continue;
		if ( builtin_funcs[i](sock,user,msg+1,replyto) < 0)
			return -1;
	}
	if ( !(botflags&BOT_EXTERN) )
		goto skipcmd;
skipcmd:
	if ( strpbrk(msg,"abcdefghijklmnopqrstuvwxyz") || (strlen(msg) < 10)
	     || !(botflags&BOT_SHOUT) )
		goto skipshout;
	shout_get();
	if ( shout_q.id )
		ircsend(sock,"PRIVMSG %s :%s",replyto,shout_q.line);
	if ( botflags&BOT_RECORD )
		shout_save(user[0],replyto,msg);
skipshout:
	return 0;
}
