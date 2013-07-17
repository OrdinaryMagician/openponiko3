/*
	shoutbot.c : Shoutbot routines.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#include "common.h"
#include "shoutbot.h"
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
/* SQL stuff */
#define QDB_CREATE	"CREATE TABLE IF NOT EXISTS QUOTES ("\
			"ID INTEGER PRIMARY KEY, "\
			"EPOCH INTEGER NOT NULL, "\
			"NAME TEXT NOT NULL, "\
			"CHANNEL TEXT NOT NULL, "\
			"LINE TEXT NOT NULL);"
#define QDB_VACUUM	"VACUUM;"
#define QDB_SELECTRAND	"SELECT * FROM QUOTES LIMIT 1 OFFSET ("\
			"SELECT ABS(RANDOM())%(SELECT COUNT(*) FROM QUOTES)"\
			");"
#define QDB_COUNTQUOTES	"SELECT COUNT(*) FROM QUOTES;"
#define QDB_COUNTLINE	"SELECT COUNT(*) FROM QUOTES WHERE LINE LIKE %Q;"
#define QDB_COUNTCHAN	"SELECT COUNT(*) FROM QUOTES WHERE CHANNEL LIKE %Q;"
#define QDB_COUNTNAME	"SELECT COUNT(*) FROM QUOTES WHERE NAME LIKE %Q;"
#define QDB_SAVE	"INSERT INTO QUOTES (EPOCH,NAME,CHANNEL,LINE) VALUES("\
			"date('now'),%Q,%Q,%Q);"
/* the quote database */
sqlite3 *quotedb;
/* currently loaded quote */
quote_t shout_q =
{
	0,
	0,
	{0},
	{0},
	{0},
};
/* start up */
int shout_init( void )
{
	/* generate quotedb path */
	char qpath[256];
	strcpy(qpath,getenv("HOME"));
	strcat(qpath,"/quotes.db");
	if ( sqlite3_open(qpath,&quotedb) )
	{
		bail("shoutbot error: %s\n",sqlite3_errmsg(quotedb));
		sqlite3_close(quotedb);
		return 1;
	}
	if ( sqlite3_exec(quotedb,QDB_CREATE,NULL,NULL,NULL) )
	{
		bail("shoutbot error: %s\n",sqlite3_errmsg(quotedb));
		sqlite3_close(quotedb);
		return 1;
	}
	return 0;
}
/* shut down */
int shout_quit( void )
{
	sqlite3_close(quotedb);
	return 0;
}
/* clean up database (vacuum) */
void shout_tidy( void )
{
	sqlite3_exec(quotedb,QDB_VACUUM,NULL,NULL,NULL);
}
/* quote get internal */
static int shout_get_cl( void *unused, int argc, char **argv, char **coln )
{
	/* silence compiler errors */
	unused = NULL;
	/* first sanity check */
	if ( argc != 5 )
		return bail("shout_get_cl: failed sanity check #1\n");
	/* second sanity check */
	if ( strcmp(coln[0],"ID") || strcmp(coln[1],"EPOCH")
		|| strcmp(coln[2],"NAME") || strcmp(coln[3],"CHANNEL")
		|| strcmp(coln[4],"LINE") )
		return bail("shout_get_cl: failed sanity check #2\n");
	memset(&shout_q,0,sizeof(shout_q));
	shout_q.id = atoll(argv[0]);
	shout_q.epoch = atoll(argv[1]);
	strncpy(shout_q.name,argv[2],256);
	strncpy(shout_q.channel,argv[3],256);
	strncpy(shout_q.line,argv[4],256);
	return 0;
}
/* get a random quote */
int shout_get( void )
{
	if ( sqlite3_exec(quotedb,QDB_SELECTRAND,shout_get_cl,NULL,NULL) )
		return bail("shoutbot error: %s\n",sqlite3_errmsg(quotedb));
	return 0;
}
/* save a new quote */
int shout_save( const char *who, const char *where, const char *what )
{
	char *fmtsav = sqlite3_mprintf(QDB_SAVE,who,where,what);
	if ( sqlite3_exec(quotedb,fmtsav,NULL,NULL,NULL) )
	{
		sqlite3_free(fmtsav);
		return bail("shoutbot error: %s\n",sqlite3_errmsg(quotedb));
	}
	sqlite3_free(fmtsav);
	return 0;
}