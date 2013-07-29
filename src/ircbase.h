/*
	ircbase.h : Base IRC handling.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
/* shoutbot flags */
#define BOT_SHOUT  0x01 /* can we shout? */
#define BOT_RECORD 0x02 /* can we save shouts? */
#define BOT_TITLE  0x04 /* resole URL titles? */
#define BOT_BLIST  0x08 /* use a blacklist? */
#define BOT_REJOIN 0x10 /* auto rejoin when kicked? */
#define BOT_EXTERN 0x20 /* can we execute external scripts? */
#define BOT_DELAY  0x40 /* is delay between commands active? */
/* shoutbot variables (all on by default) */
unsigned char botflags;
/* other flags */
char active;
char autojoined;
/* send data */
int ircsend( int ssock, const char *fmt, ... );
/* receive data */
int ircget( int ssock, char *to, int max );
/* are we still running? */
int ircactive( void );
/* open irc connection */
int ircopen( void );
/* close irc connection */
int ircclose( void );
/* process irc connection */
void process( void );
