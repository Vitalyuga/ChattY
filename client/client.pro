#
# FILNAMN: 		server.pro
# PROGRAMMERARE:	hanel742, eriek984, jened502, tobgr602, niker917, davha227
# SKAPAD DATUM:		2013-11-17
# BESKRIVNING:		Project file for Qmake. Creates makefile compatible with make and c++ compilers

# Use GNU compilers - not clang
QMAKE_CXX	= g++
QMAKE_CC	= gcc

# Qt components required 
QT		+= core 
QT		+= network gui testlib 

TARGET		= client

# Use C++11 standard
CONFIG		+= console c++11
CONFIG		-= app_bundle

TEMPLATE	= app

# Files to include while building
SOURCES		+= main.cc netClientConsole.cc ReadThread.cc
HEADERS		+= netClientConsole.h ReadThread.h

