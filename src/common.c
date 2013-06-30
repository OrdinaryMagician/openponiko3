/*
	common.c : Common code.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#include "common.h"
#include <stdarg.h>
int bail( const char *m, ... )
{
	va_list arg;
	va_start(arg,m);
	vfprintf(stderr,m,arg);
	va_end(arg);
	return 1;
}
