/*
	common.h : Common code.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
/* external code */
/* error out */
int bail( const char *, ... );
/* timestamp */
void ptime( const char *fmt, time_t *t );
