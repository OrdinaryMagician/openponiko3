/*
	lastseen.h : Activity tracking routines.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
/* a lastseen */
typedef struct
{
	long long id;
	long long epoch;
	char name[256];
	char channel[256];
	char line[512];
} lseen_t;
/* start up */
int lseen_init( void );
/* shut down */
int lseen_quit( void );
/* clean up database (vacuum) */
void lseen_tidy( void );
/* get a lastseen */
int lseen_get( const char *u, const char *c, lseen_t *to );
/* save a lastseen */
int lseen_save( char *u, char *c, char *l );
