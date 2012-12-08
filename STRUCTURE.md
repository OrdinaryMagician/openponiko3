The modular restructure of the bot is planned to have everything separated in these independend binaries:
* **ponikomain:** main loop, GUI, timers, "proxy interface"
* **ponikodb:** database file handling
* **ponikoirc:** one instance per irc server connected to
* **ponikoai:** AI handling (still won't be implemented)
* **ponikourl:** curl methods for parsing linkss to sites and retrieve the titles and other data
* **ponikosay:** cute little avatar for the bot on the desktop
* **ponikocmd:** complex message parser
* **ponikoxcmd:** external PonikoScript command system

The binaries communicate through named UNIX sockets.
