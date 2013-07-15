/*
	ponikofn.h : Poniko functions.
	(C)2013 Marisa Kirisame, UnSX Team.
	Part of OpenPONIKO 3.0, the ultimate CAPS LOCK bot.
	Released under the MIT License.
*/
#ifndef PONIKOFN_H
#define PONIKOFN_H
/* builtin command */
typedef int (*builtinfn_t)( int, char**, char*, char* );
/* parse message from irc user */
int parsemesg( int sock, char **user, char *msg, char *replyto );
#endif
