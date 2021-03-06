/*
	ponikomain.c : Main C entry code for the bot.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#include <stdio.h>
#include "shoutbot.h"
#include "lastseen.h"
#include <time.h>
#include "common.h"
#include "ponikocfg.h"
#include "ircbase.h"
int main( void )
{
	/* greeting */
	puts("OpenPONIKO 3.0 - Compiled on " __DATE__ " " __TIME__);
	/* shoutbot */
	if ( shout_init() )
		return bail("Couldn't init shoutbot core\n");
	/* activity */
	if ( lseen_init() )
		return bail("Couldn't init lastseen core\n");
	/* load config */
	if ( loadcfg() )
		return bail("Couldn't load configuration\n");
	if ( ircopen() )
		return bail("Couldn't connect to default irc server\n");
	while ( active )
		process();
	return (ircclose()||shout_quit()||lseen_quit())!=0;
}
