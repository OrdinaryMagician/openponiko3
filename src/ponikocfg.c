/*
	ponikocfg.c : Poniko config handling.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#include "common.h"
#include "ponikocfg.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
/* helper */
static void readval( const char *from, const char *val, char *to )
{
	int flen = strlen(val);
	int llen = strlen(from);
	if ( !strncmp(from,val,flen) )
	{
		strncpy(to,from+flen,(llen-flen)-1);
		puts(to);
	}
}
/* loads user config */
int loadcfg( void )
{
	FILE *conf = NULL;
	char cpath[256];
	strcpy(cpath,getenv("HOME"));
	strcat(cpath,"/poniko3.conf");
	if ( !(conf = fopen(cpath,"r")) )
		return bail("loadcfg error: %s\n",strerror(errno));
	/* extremely naïve method */
	char line[256] = {0};
	for ( ; ; )
	{
		fgets(line,255,conf);
		if ( feof(conf) || line[0] == '\n' )
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
	fclose(conf);
	return 0;
}
