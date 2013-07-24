# OpenPONIKO 3.0

![Poniko](https://github.com/OrdinaryMagician/openponiko3/raw/master/poniko.png)

## About
Just a simple IRC bot.
When you shout at her, she shouts back.

## Host requirements
Any *nix with an active internet connection (obviously).
Might run on Windows with some patching.

## Build requirements
* sqlite3 (for databases)
* curl (for HTTP handling)
* yajl (for certain website APIs)

## About buildscripts
OpenPONIKO 3.0 uses an AOS buildscript for compiling and linking.
It requires Zsh and (by default, unless $CC is overriden) Clang.

There's also a makefile (ew) for those who hate AOS buildscripts.
(And it seems this is a lot of people)
