/*
	ponikocfg.c : Poniko config handling.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#include <string.h>
#include <errno.h>
#include "common.h"
#include "ponikocfg.h"
/* helper */
#define readval(n,o,to) if(!strncmp(line,n,o))strncpy(to,line+o,256)
/* loads user config */
int loadcfg( void )
{
	FILE *conf = NULL;
	if ( !(conf = fopen(CFGPATH,"r")) )
		return bail("loadcfg error: %s\n",strerror(errno));
	/* extremely naïve method */
	char line[256] = {0};
	while( !feof(conf) )
	{
		fgets(line,255,conf);
		readval("SERVER=",7,cfg.server);
		readval("CHANNEL=",8,cfg.server);
		readval("USER=",5,cfg.server);
		readval("NICKNAME=",9,cfg.server);
		readval("PASSWORD=",9,cfg.server);
		readval("OWNER=",6,cfg.server);
	}
	fclose(conf);
	return 0;
}
