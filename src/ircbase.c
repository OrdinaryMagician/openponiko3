/*
	ircbase.c : Base IRC handling.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#include "ircbase.h"
#include <sys/socket.h>
/* shoutbot variables */
static char botflags;
/* are we still running? */
int ircactive( void )
{
	return 0;
}
/* open irc connection */
int ircopen( void )
{
	return 0;
}
/* close irc connection */
int ircclose( void )
{
	return 0;
}
/* process irc connection */
void process( void )
{
}
