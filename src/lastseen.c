/*
	lastseen.c : Activity tracking routines.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#include <sqlite3.h>
#include "lastseen.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"
/* SQL stuff */
#define LDB_CREATE	"CREATE TABLE IF NOT EXISTS LASTSEEN ("\
			"ID INTEGER PRIMARY KEY, "\
			"EPOCH INTEGER NOT NULL, "\
			"NAME TEXT NOT NULL, "\
			"CHANNEL TEXT NOT NULL, "\
			"LINE TEXT NOT NULL)"
#define LDB_VACUUM	"VACUUM"
#define LDB_SELECT	"SELECT * FROM LASTSEEN WHERE NAME=%Q AND CHANNEL=%Q"\
			"LIMIT 1"
#define LDB_UPDATE	"UPDATE LASTSEEN SET EPOCH=date('now'),LINE=%Q"\
			"WHERE NAME=%Q AND CHANNEL=%Q"
#define LDB_INSERT	"INSERT INTO LASTSEEN (EPOCH,NAME,CHANNEL,LINE)"\
			"VALUES(date('now'),%Q,%Q,%Q)"
/* the lastseen database */
sqlite3 *lseendb;
/* start up */
int lseen_init( void )
{
	/* generate lseendb path */
	char lpath[256];
	strcpy(lpath,getenv("HOME"));
	strcat(lpath,"/lastseen.db");
	if ( sqlite3_open(lpath,&lseendb) )
		return (sqlite3_close(lseendb)&0)
			||bail("lastseen error: %s\n",sqlite3_errmsg(lseendb));
	if ( sqlite3_exec(lseendb,LDB_CREATE,NULL,NULL,NULL) )
		return (sqlite3_close(lseendb)&0)
			||bail("lastseen error: %s\n",sqlite3_errmsg(lseendb));
	return 0;
}
/* shut down */
int lseen_quit( void )
{
	return sqlite3_close(lseendb)&0;
}
/* clean up database (vacuum) */
void lseen_tidy( void )
{
	sqlite3_exec(lseendb,LDB_VACUUM,NULL,NULL,NULL);
}
/* lastseen get internal */
static int lseen_get_cl( void *to, int argc, char **argv, char **coln )
{
	/* silence warnings */
	coln = 0;
	/* where to save? */
	lseen_t *sl = (lseen_t*)to;
	/* sanity check */
	if ( argc != 5 )
		return bail("lseen_get_cl: failed sanity check\n");
	if ( sl && argv )
	{
		memset(sl,0,sizeof(lseen_t));
		sl->id = atoll(argv[0]);
		sl->epoch = atoll(argv[1]);
		strncpy(sl->name,argv[2],256);
		strncpy(sl->channel,argv[3],256);
		strncpy(sl->line,argv[4],256);
	}
	return 0;
}
/* get a lastseen */
int lseen_get( const char *u, const char *c, lseen_t *to )
{
	char *fmtsav = sqlite3_mprintf(LDB_SELECT,u,c);
	if ( sqlite3_exec(lseendb,fmtsav,lseen_get_cl,to,NULL) )
	{
		sqlite3_free(fmtsav);
		return bail("lastseen error: %s\n",sqlite3_errmsg(lseendb));
	}
	sqlite3_free(fmtsav);
	return 0;
}
/* save a lastseen */
int lseen_save( char *u, char *c, char *l )
{
	lseen_t exists = {0,0,{0},{0},{0}};
	lseen_get(u,c,&exists);
	char *fmtsav = exists.id
		?sqlite3_mprintf(LDB_UPDATE,l,u,c)
		:sqlite3_mprintf(LDB_INSERT,u,c,l);
	if ( sqlite3_exec(lseendb,fmtsav,NULL,NULL,NULL) )
	{
		sqlite3_free(fmtsav);
		return bail("lastseen error: %s\n",sqlite3_errmsg(lseendb));
	}
	sqlite3_free(fmtsav);
	return 0;
}
