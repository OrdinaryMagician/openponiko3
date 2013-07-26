#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <sqlite3.h>
/* SQL stuff */
#define QDB_CREATE	"CREATE TABLE IF NOT EXISTS QUOTES ("\
			"ID INTEGER PRIMARY KEY, "\
			"EPOCH INTEGER NOT NULL, "\
			"NAME TEXT NOT NULL, "\
			"CHANNEL TEXT NOT NULL, "\
			"LINE TEXT NOT NULL)"
#define QDB_SAVE	"INSERT INTO QUOTES (EPOCH,NAME,CHANNEL,LINE) VALUES("\
			"%Q,%Q,%Q,%Q)"
#define LDB_CREATE	"CREATE TABLE IF NOT EXISTS LASTSEEN ("\
			"ID INTEGER PRIMARY KEY, "\
			"EPOCH INTEGER NOT NULL, "\
			"NAME TEXT NOT NULL UNIQUE, "\
			"CHANNEL TEXT NOT NULL, "\
			"LINE TEXT NOT NULL)"
#define LDB_SAVE	"INSERT INTO LASTSEEN (EPOCH,NAME,CHANNEL,LINE)"\
			"VALUES(%Q,%Q,%Q,%Q)"
/* helper macro */
#define min(x,y)	((x)<(y))?(x):(y)
/* the quote database */
sqlite3 *quotedb;
/* the lastseen database */
sqlite3 *lseendb;
/* error out */
int bail( const char *m, ... )
{
	va_list arg;
	va_start(arg,m);
	vfprintf(stderr,m,arg);
	va_end(arg);
	return 1;
}
/* quotedb init */
int qdb_init( const char *path )
{
	if ( sqlite3_open(path,&quotedb) )
		return (sqlite3_close(quotedb)&0)
			||bail("qconv: %s\n",sqlite3_errmsg(quotedb));
	if ( sqlite3_exec(quotedb,QDB_CREATE,NULL,NULL,NULL) )
		return (sqlite3_close(quotedb)&0)
			||bail("qconv: %s\n",sqlite3_errmsg(quotedb));
	return 0;
}
/* lseendb init */
int mdb_init( const char *path )
{
	if ( sqlite3_open(path,&lseendb) )
		return (sqlite3_close(lseendb)&0)
			||bail("mconv: %s\n",sqlite3_errmsg(lseendb));
	if ( sqlite3_exec(lseendb,LDB_CREATE,NULL,NULL,NULL) )
		return (sqlite3_close(lseendb)&0)
			||bail("mconv: %s\n",sqlite3_errmsg(lseendb));
	return 0;
}
/* shutdown */
int qdb_quit( void )
{
	return sqlite3_close(quotedb)&0;
}
int mdb_quit( void )
{
	return sqlite3_close(lseendb)&0;
}
/* insert quote */
int shout_save( char *e, char *u, char *c, char *l )
{
	char *fmtsav = sqlite3_mprintf(QDB_SAVE,e,u,c,l);
	if ( sqlite3_exec(quotedb,fmtsav,NULL,NULL,NULL) )
	{
		sqlite3_free(fmtsav);
		return bail("\nqconv: %s\n",sqlite3_errmsg(quotedb));
	}
	sqlite3_free(fmtsav);
	return 0;
}
/* insert lastseen */
int seen_save( char *e, char *u, char *c, char *l )
{
	char *fmtsav = sqlite3_mprintf(LDB_SAVE,e,u,c,l);
	if ( sqlite3_exec(lseendb,fmtsav,NULL,NULL,NULL) )
	{
		sqlite3_free(fmtsav);
		return bail("\nmconv: %s\n",sqlite3_errmsg(lseendb));
	}
	sqlite3_free(fmtsav);
	return 0;
}
/* parse and save legacy quote */
int qparse( char *l, int ln )
{
	/* check for EOF or error */
	if ( !l )
		return 1;
	/* read epoch */
	if ( *l != '[' )
		return bail("\nqparse: parse error at line %d: "
			"expected \'[\' got \'%c\'\n",ln,*l);
	char *cl = strchr(l,']');
	if ( !cl )
		return bail("\nqparse: parse error at line %d: "
			"no closing \']\' found\n",ln);
	char epochstr[256] = {0};
	strncpy(epochstr,(l+1),min(255,(cl-(l+1))));
	/* read nickname */
	l = cl+2;
	if ( *l != '<' )
		return bail("\nqparse: parse error at line %d: "
			"expected \'<\' got \'%c\'\n",ln,*l);
	cl = strchr(l,'>');
	if ( !cl )
		return bail("\nqparse: parse error at line %d: "
			"no closing \'>\' found\n",ln);
	char namestr[256] = {0};
	strncpy(namestr,(l+1),min(255,(cl-(l+1))));
	l = cl+2;
	/* read quote */
	char linestr[512] = {0};
	char *el = strchr(l,'\r');
	if ( el )
		*el = '\0';
	if ( (el-l) > 511 )
		bail("\nqparse: warning at line %d: "
			"quote is longer than 511 characters\n",ln);
	strncpy(linestr,l,511);
	shout_save(epochstr,namestr,"UNKNOWN",linestr);
	printf("\rParsed %d lines.",ln);
	fflush(stdout);
	return 0;
}
/* parse and save legacy lastseen */
int rparse( char *l, int ln )
{
	/* check for EOF or error */
	if ( !l )
		return 1;
	/* read epoch */
	if ( *l != '[' )
		return bail("\nrparse: parse error at line %d: "
			"expected \'[\' got \'%c\'\n",ln,*l);
	char *cl = strchr(l,']');
	if ( !cl )
		return bail("\nrparse: parse error at line %d: "
			"no closing \']\' found\n",ln);
	char epochstr[256] = {0};
	strncpy(epochstr,(l+1),min(255,(cl-(l+1))));
	/* read channel */
	l = cl+2;
	if ( *l != '[' )
		return bail("\nrparse: parse error at line %d: "
			"expected \'[\' got \'%c\'\n",ln,*l);
	cl = strchr(l,']');
	if ( !cl )
		return bail("\nrparse: parse error at line %d: "
			"no closing \']\' found\n",ln);
	char chanstr[256] = {0};
	strncpy(chanstr,(l+1),min(255,(cl-(l+1))));
	/* read nickname */
	l = cl+2;
	if ( *l != '<' )
		return bail("\nrparse: parse error at line %d: "
			"expected \'<\' got \'%c\'\n",ln,*l);
	cl = strchr(l,'>');
	if ( !cl )
		return bail("\nrparse: parse error at line %d: "
			"no closing \'>\' found\n",ln);
	char namestr[256] = {0};
	strncpy(namestr,(l+1),min(255,(cl-(l+1))));
	l = cl+2;
	/* read quote */
	char linestr[512] = {0};
	char *el = strchr(l,'\r');
	if ( el )
		*el = '\0';
	if ( (el-l) > 511 )
		bail("\nrparse: warning at line %d: "
			"message is longer than 511 characters\n",ln);
	strncpy(linestr,l,511);
	seen_save(epochstr,namestr,chanstr,linestr);
	printf("\rParsed %d lines.",ln);
	fflush(stdout);
	return 0;
}
/* convert quotes */
int qconv( const char *old, const char *new )
{
	FILE *o = fopen(old,"r");
	if ( !o )
		return bail("qconv: %s\n",strerror(errno));
	if ( qdb_init(new) )
		return (fclose(o)&0)+1;
	puts("Migrating OpenPONIKO 2.0 quotes database...");
	char l[4096] = {0};
	int i = 1;
	sqlite3_exec(quotedb,"BEGIN TRANSACTION",NULL,NULL,NULL);
	while ( !qparse(fgets(l,4095,o),i++) );
	sqlite3_exec(quotedb,"END TRANSACTION",NULL,NULL,NULL);
	putchar('\n');
	return qdb_quit()||(fclose(o)&0);
}
/* convert messages */
int rconv( const char *old, const char *new )
{
	FILE *o = fopen(old,"r");
	if ( !o )
		return bail("rconv: %s\n",strerror(errno));
	if ( mdb_init(new) )
		return (fclose(o)&0)+1;
	puts("Migrating OpenPONIKO 2.0 lastseen database...");
	char l[4096] = {0};
	int i = 1;
	sqlite3_exec(lseendb,"BEGIN TRANSACTION",NULL,NULL,NULL);
	while ( !rparse(fgets(l,4095,o),i++) );
	sqlite3_exec(lseendb,"END TRANSACTION",NULL,NULL,NULL);
	putchar('\n');
	return mdb_quit()||(fclose(o)&0);
}
/* main */
int main( int argc, char **argv )
{ 
	return (argc<4)?bail("usage: %s <qdb|rdb> <old db> <new db>\n",argv[0])
		:!strcmp(argv[1],"qdb")?qconv(argv[2],argv[3])
		:!strcmp(argv[1],"rdb")?rconv(argv[2],argv[3])
		:1;
}
