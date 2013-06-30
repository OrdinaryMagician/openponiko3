/*
	common.h : Common code.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#ifndef COMMON_H
#define COMMON_H
/* external code */
#include <stdio.h>
#include <unistd.h>
/* some constants */
#ifndef CFGPATH
#define CFGPATH "/etc/poniko3.conf"
#endif
/* error out */
int bail( const char *, ... );
#endif
