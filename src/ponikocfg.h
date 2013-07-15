/*
	ponikocfg.h : Poniko config handling.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#ifndef PONIKOCFG_H
#define PONIKOCFG_H
typedef struct
{
	char server[256];
	char port[256];
	char chan[256];
	char user[256];
	char nick[256];
	char name[256];
	char pass[256];
	char owner[256];
} config_t;
config_t cfg;
/* loads user config */
int loadcfg( void );
#endif
