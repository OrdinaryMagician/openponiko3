/*
	common.c : Common code.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#include "common.h"
#include <stdarg.h>
/* error out */
int bail( const char *m, ... )
{
	va_list arg;
	va_start(arg,m);
	vfprintf(stderr,m,arg);
	va_end(arg);
	return 1;
}
/* timestamp */
void ptime( const char *fmt, time_t *t )
{
	char stamp[256] = {0};
	time_t tt = time(NULL);
	strftime(stamp,256,fmt,localtime(t?t:&tt));
	printf("%s",stamp);
}
