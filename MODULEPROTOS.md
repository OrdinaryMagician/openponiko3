# OpenPONIKO 3.0 module communication protocols and languages
Modules communicate through a bunch of named UNIX sockets

Every module has its designated socket file

All IDs are at most 8 characters long

Ponikomain is PNKOMAIN

Databases are PNKODBxx, where the xx is an hex number from 00 to FF

IRC server connections are similar, PNKOIRCx

URL handler is PNKOURL

Ponikosay uses PNKOSAY

Ponikocmd and Ponikoxcmd are PNKOCMD and PNKOXCMD

PonikoAI should be PNKOAI

PNKOALL is a special socket that sends the data to all modules

PNKOADB is the same for all databases, and PNKOAIRC is for all irc connecions

All the transfers are in plain text, terminated by a <crlf> and following a specific form layout for each task.

Malformed or unsupported forms should be replied to with an error form

## ping form for all modules
 * **HEADER:** PING
   * 1 blank space
 * **TIMESTAMP:** all data until end of packet

## error form for all modules
 * **HEADER:** ERR
   * 1 blank space
 * **ERRORCODE:** code for error, usually "UNSUPPORTED" or "MALFORMED".

## data request form for ponikodb
 * **HEADER:** REQ
   * 1 blank space
 * **REQUEST TYPE:** LINE, COUNT, DELETE
   * 1 blank space
 * **REQUEST SUBTYPE:**
   * **for LINE:** LINENUM, NAME, KEYWORD
   * **for COUNT:** NAME, KEYWORD, ALL
   * **for DELETE:** LINENUM, NAME, KEYWORD, ALL
   (the following parts aren't needed if subtype is ALL)
   * 1 blank space
 * **REQUEST VALUE:** all the data until end of packet

## data transfer form for ponikodb:
 * **HEADER:** PDB
   * 1 blank space
 * **TRANSFER TYPE:** NUMVAL, LINEDUMP, STATUS
   * 1 blank space
 * **EXTRA DATA:**
   * **for NUMVAL:** the number
   * **for LINEDUMP:** total number of lines being transferred
   * **for STATUS:** the status code (OK, ERROR, NOAUTH, READONLY, BUSY)
   (following parts ignored unless type is LINEDUMP)
   * 1 blank space
 * **LINE DUMP:** all the data until end of packet data insert form for ponikodb:
   * **HEADER:** INS
   * 1 blank space
   * **INSERT LINE:** all the data until end of packet (multiple lines not allowed)

## data status form for ponikodb:
 * **HEADER:** STAT
   * 1 blank space
 * **TYPE:** QUERY, REPLY
   (following parts only if REPLY)
   * 1 blank space
 * **REPLY DATA:** all the data until end of packet. should contain full database path with filename, size and last access time

## irc connection form for ponikoirc:
 * **HEADER:** IRCC
   * 1 blank space
 * **OPERATION TYPE:** CONNECT, DISCONNECT, STATUS
   * 1 blank space
   (in case of CONNECT)
 * **SERVER ADDRESS:** hostname:port, must not contain any spaces
   * 1 blank space
 * **SERVER CONFIG:** filename for configuration on this server (nick, username, passwords, etc.), can be set to AUTO to use a file with the name of the host
   (in case of DISCONNECT)
 * **DISCONNECT TYPE:**
   * **FORCED**: module will close socket and shut down
   * **SOFT**: send a quit message to server, shut down when server has closed the connection
   (in case of STATUS)
 * **SIDE:** REQUEST or SEND
   (if side is SEND)
   * 1 blank space
 * **STATUS MESSAGE:** all data until end of packet. should contain IP of server and port, last message time, total amount of bytes sent and received and total count of reconnections due to ping timeout / socket error. if the connection has problems, it should just contain ERROR and a numeric code

## irc packet receive form for ponikoirc:
 * **HEADER:** IRCP
   * 1 blank space
 * **SOURCE:** hostname:port
   * 1 blank space
 * **TIMESTAMP:** as a string
   * 1 blank space
 * **IRCDATA:** all the data until end of packet

## irc packet send form for ponikoirc:
 * **HEADER:** IRCC
   * 1 blank space
 * **IRCDATA:** all the data until end of packet

## url request form for ponikohttp:
 * **HEADER:** URLR
   * 1 blank space
 * **REQUEST URL:** the link we want information of

## url reply form for ponikohttp:
 * **HEADER:** URLZ
   * 1 blank space
 * **CONTENT TYPE:** content-type of the obtained file
   * 1 blank space
 * **FILESIZE:** size in bytes of said file
   * 1 blank space
 * **TITLE:** All the data until end of packet. usually "[URL] Title" or similar. in case of it being a file, it will display "[FILE] filename" instead. There are other special functions in the http request handler that may obtain extra data from sites such as YouTube, for example

## ponikosay notification form:
 * **HEADER:** PSAY
   * 1 blank space
 * **ANIMATION:** animation name to use
   * 1 blank space
 * **MOOD:** mood variation to use
   * 1 blank space
 * **LINE:** the line (or lines) the avatar will say

## ponikocmd/xcmd request form:
 * **HEADER:** PCMD
   * 1 blank space
 * **HOSTNAME:** hostname of irc server we are connected to
   * 1 blank space
 * **CHANNEL:** irc channel we're in
   * 1 blank space
 * **USER:** full user identifier (nick!user@host) who issued the command
   * 1 blank space
 * **LINE:** all data until end of packet is the line and/or command

### footer notes
ponikocmd/xcmd has no reply form other than raw irc packets

ponikoai is supposed to use the same forms as ponikocmd/xcmd
