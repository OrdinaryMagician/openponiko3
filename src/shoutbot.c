/*
	shoutbot.c : Shoutbot routines.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#include <sqlite3.h>
#include "shoutbot.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"
/* SQL stuff */
#define QDB_CREATE	"CREATE TABLE IF NOT EXISTS QUOTES ("\
			"ID INTEGER PRIMARY KEY, "\
			"EPOCH INTEGER NOT NULL, "\
			"NAME TEXT NOT NULL, "\
			"CHANNEL TEXT NOT NULL, "\
			"LINE TEXT NOT NULL)"
#define QDB_VACUUM	"VACUUM"
#define QDB_SELECTRAND	"SELECT * FROM QUOTES LIMIT 1 OFFSET ("\
			"SELECT ABS(RANDOM())%(SELECT COUNT(*) FROM QUOTES))"
#define QDB_SELECTLINE	"SELECT * FROM QUOTES WHERE LINE=%Q LIMIT 1"
#define QDB_COUNTQUOTES	"SELECT COUNT(*) FROM QUOTES"
#define QDB_COUNTLINE	"SELECT COUNT(*) FROM QUOTES WHERE LINE LIKE %Q"
#define QDB_COUNTCHAN	"SELECT COUNT(*) FROM QUOTES WHERE CHANNEL=%Q"
#define QDB_COUNTNAME	"SELECT COUNT(*) FROM QUOTES WHERE NAME=%Q"
#define QDB_SAVE	"INSERT INTO QUOTES (EPOCH,NAME,CHANNEL,LINE) VALUES("\
			"date('now'),%Q,%Q,%Q)"
/* the quote database */
sqlite3 *quotedb;
/* currently loaded quote */
quote_t shout_q = {0,0,{0},{0},{0},};
/* start up */
int shout_init( void )
{
	/* generate quotedb path */
	char qpath[256];
	strcpy(qpath,getenv("HOME"));
	strcat(qpath,"/quotes.db");
	if ( sqlite3_open(qpath,&quotedb) )
		return (sqlite3_close(quotedb)&0)
			||bail("shoutbot error: %s\n",sqlite3_errmsg(quotedb));
	if ( sqlite3_exec(quotedb,QDB_CREATE,NULL,NULL,NULL) )
		return (sqlite3_close(quotedb)&0)
			||bail("shoutbot error: %s\n",sqlite3_errmsg(quotedb));
	return 0;
}
/* shut down */
int shout_quit( void )
{
	return sqlite3_close(quotedb)&0;
}
/* clean up database (vacuum) */
void shout_tidy( void )
{
	sqlite3_exec(quotedb,QDB_VACUUM,NULL,NULL,NULL);
}
/* quote get internal */
static int shout_get_cl( void *to, int argc, char **argv, char **coln )
{
	/* silence warnings */
	coln = 0;
	/* where to save? */
	quote_t *sq = (quote_t*)to;
	/* sanity check */
	if ( argc != 5 )
		return bail("shout_get_cl: failed sanity check\n");
	if ( sq && argv )
	{
		memset(sq,0,sizeof(quote_t));
		sq->id = atoll(argv[0]);
		sq->epoch = atoll(argv[1]);
		strncpy(sq->name,argv[2],256);
		strncpy(sq->channel,argv[3],256);
		strncpy(sq->line,argv[4],256);
	}
	return 0;
}
/* counter internal */
static int shout_get_cn( void *to, int argc, char **argv, char **coln )
{
	/* silence warnings */
	coln = 0;
	/* where to save? */
	long long *cn = (long long*)to;
	/* sanity check */
	if ( argc != 1 )
		return bail("shout_get_cn: failed sanity check\n");
	if ( cn )
		*cn = argv?atoll(argv[0]):-1;
	return 0;
}
/* get a random quote */
int shout_get( void )
{
	if ( sqlite3_exec(quotedb,QDB_SELECTRAND,shout_get_cl,&shout_q,NULL) )
		return bail("shoutbot error: %s\n",sqlite3_errmsg(quotedb));
	return 0;
}
/* count all */
int shout_countall( long long *to )
{
	if ( sqlite3_exec(quotedb,QDB_COUNTQUOTES,shout_get_cn,to,NULL) )
		return bail("shoutbot error: %s\n",sqlite3_errmsg(quotedb));
	return 0;
}
/* count key */
int shout_countkey( const char *k, long long *to )
{
	char *fmtsav = sqlite3_mprintf(QDB_COUNTLINE,k);
	if ( sqlite3_exec(quotedb,fmtsav,shout_get_cn,to,NULL) )
	{
		sqlite3_free(fmtsav);
		return bail("shoutbot error: %s\n",sqlite3_errmsg(quotedb));
	}
	sqlite3_free(fmtsav);
	return 0;
}
/* count channel */
int shout_countchan( const char *c, long long *to )
{
	char *fmtsav = sqlite3_mprintf(QDB_COUNTCHAN,c);
	if ( sqlite3_exec(quotedb,fmtsav,shout_get_cn,to,NULL) )
	{
		sqlite3_free(fmtsav);
		return bail("shoutbot error: %s\n",sqlite3_errmsg(quotedb));
	}
	sqlite3_free(fmtsav);
	return 0;
}
/* count name */
int shout_countname( const char *n, long long *to )
{
	char *fmtsav = sqlite3_mprintf(QDB_COUNTNAME,n);
	if ( sqlite3_exec(quotedb,fmtsav,shout_get_cn,to,NULL) )
	{
		sqlite3_free(fmtsav);
		return bail("shoutbot error: %s\n",sqlite3_errmsg(quotedb));
	}
	sqlite3_free(fmtsav);
	return 0;
}
/* get quote by key */
int shout_get_key( const char *k, quote_t *where )
{
	char *fmtsav = sqlite3_mprintf(QDB_SELECTLINE,k);
	if ( sqlite3_exec(quotedb,fmtsav,shout_get_cl,where,NULL) )
	{
		sqlite3_free(fmtsav);
		return bail("shoutbot error: %s\n",sqlite3_errmsg(quotedb));
	}
	sqlite3_free(fmtsav);
	return 0;
}
/* save a new quote */
int shout_save( char *u, char *c, char *l )
{
	char *fmtsav = sqlite3_mprintf(QDB_SAVE,u,c,l);
	if ( sqlite3_exec(quotedb,fmtsav,NULL,NULL,NULL) )
	{
		sqlite3_free(fmtsav);
		return bail("shoutbot error: %s\n",sqlite3_errmsg(quotedb));
	}
	sqlite3_free(fmtsav);
	return 0;
}
