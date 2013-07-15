/*
	ponikomain.c : Main C entry code for the bot.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#include "common.h"
#include "ponikocfg.h"
#include "ircbase.h"
int main( void )
{
	/* greeting */
	puts("OpenPONIKO 3.0 - Compiled on " __DATE__ " " __TIME__);
	/* load config */
	if ( loadcfg() )
		return bail("Couldn't load configuration\n");
	if ( ircopen() )
		return bail("Couldn't connect to default irc server\n");
	while ( active )
		process();
	ircclose();
	return 0;
}
