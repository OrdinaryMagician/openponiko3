/*
	shoutbot.h : Shoutbot routines.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#ifndef SHOUTBOT_H
#define SHOUTBOT_H
/* a quote */
typedef struct
{
	long long id;
	long long epoch;
	char name[256];
	char channel[256];
	char line[256];
} quote_t;
/* currently loaded quote */
quote_t shout_q;
/* start up */
int shout_init( void );
/* shut down */
int shout_quit( void );
/* clean up database (vacuum) */
void shout_tidy( void );
/* get a random quote */
int shout_get( void );
/* save a new quote */
int shout_save( const char *who, const char *where, const char *what );
#endif
