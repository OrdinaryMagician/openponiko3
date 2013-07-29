/*
	ponikocfg.c : Poniko config handling.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "common.h"
#include <errno.h>
#include "ponikocfg.h"
/* helper */
static void readval( const char *from, const char *val, char *to )
{
	int flen = strlen(val), llen = strlen(from);
	if ( !strncmp(from,val,flen) )
		strncpy(to,from+flen,(llen-flen)-1);
}
/* loads user config */
int loadcfg( void )
{
	char cpath[256] = {0};
	strcat(strcpy(cpath,getenv("HOME")),"/poniko3.conf");
	FILE *conf = fopen(cpath,"r");
	if ( !conf )
		return bail("loadcfg error: %s\n",strerror(errno));
	/* extremely naïve method */
	char line[256] = {0};
	for ( ; ; )
	{
		if ( !fgets(line,255,conf) || feof(conf) || (line[0] == '\n') )
			break;
		readval(line,"SERVER=",cfg.server);
		readval(line,"PORT=",cfg.port);
		readval(line,"CHANNEL=",cfg.chan);
		readval(line,"USER=",cfg.user);
		readval(line,"NICKNAME=",cfg.nick);
		readval(line,"REALNAME=",cfg.name);
		readval(line,"PASSWORD=",cfg.pass);
		readval(line,"OWNER=",cfg.owner);
	}
	return fclose(conf)&0;
}
