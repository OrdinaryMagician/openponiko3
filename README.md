# OpenPONIKO 3.0

## About
Just a simple IRC bot.
When you shout at her, she shouts back.

## Host requirements
x86_64 Linux with an active internet connection (obviously)

## Build requirements
* sqlite3 (for databases)
* curl (for HTTP handling)
* yajl (for certain website APIs)

## About buildscripts
OpenPONIKO 3.0 uses an AOS buildscript for compiling and linking.
It requires Zsh and (by default, unless $CC is overriden) Clang.