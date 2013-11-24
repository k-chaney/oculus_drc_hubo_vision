#############################################################################
#
# Filename    : Makefile
# Content     : Makefile for building linux OculusWorldDemo
# Created     : 2013
# Authors     : Simon Hallam and Peter Giokaris
#			Modified for this project by: Kenneth Chaney
# Copyright   : Copyright 2013 OculusVR, Inc. All Rights Reserved
# Instruction : The g++ compiler and stdndard lib packages need to be 
#               installed on the system.  Navigate in a shell to the 
#               directory where this Makefile is located and enter:
#
#               make                builds the release version for the 
#                                   current architechture
#               make clean          delete intermediate release object files 
#                                   and the executabe file
#               make DEBUG=1        builds the debug version for the current
#                                   architechture
#               make clean DEBUG=1  deletes intermediate debug object files 
#                                   and the executable file
#
# Output      : Relative to the directory this Makefile lives in, executable
#               files get built at the following locations depending upon the
#               architechture of the system you are running:
#
#               ./Release/OculusWorldDemo_i386_Release
#               ./Release/OculusWorldDemo_x86_64_Release
#               ./Release/OculusWorldDemo_i386_Debug
#               ./Release/OculusWorldDemo_x86_64_Debug
#
#############################################################################

####### Detect system architecture

SYSARCH       = i386
ifeq ($(shell uname -m),x86_64)
SYSARCH       = x86_64
endif

####### Compiler, tools and options

CXX           = g++
LINK          = g++
MAKE          = make
DELETEFILE    = rm -f
DEFINES       = -DQT_WEBKIT -DGL_GLEXT_PROTOTYPES

####### Detect debug or release

DEBUG         = 0
ifeq ($(DEBUG), 1)
	CXXFLAGS      = -pipe -DDEBUG -g $(DEFINES)
	LFLAGS        = 
	RELEASETYPE   = Debug
else
	CXXFLAGS      = -pipe -O2 $(DEFINES)
	LFLAGS        = -O1
	RELEASETYPE   = Release
endif

####### Paths

LIBOVRPATH    = ../LibOVR
COMMONSRCPATH = ../Samples/CommonSrc
3RDPARTYPATH  = ../3rdParty
INCPATH       = -I. -I.. -I$(COMMONSRCPATH) -I$(LIBOVRPATH)/Include -I$(LIBOVRPATH)/Src -I/usr/include/opencv2
OBJPATH       = ./Objects
CXX_BUILD     = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJPATH)/

####### Files

LIBS          = -L$(LIBOVRPATH)/Lib/Linux/$(RELEASETYPE)/$(SYSARCH) \
		-lovr \
		-ludev \
		-lpthread \
		-lGL \
		-lX11 \
		-lXinerama \
		-lach \
		-lrt

LIBSCV        =  -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_objdetect

OBJECTS1       = $(OBJPATH)/drc_head_manipulation.o 
OBJECTS2       = $(OBJPATH)/drc_vision.o 

TARGET1        = ./drc_head_manipulation 
TARGET2        = ./drc_vision

####### Rules

all:    $(TARGET1) $(TARGET2)
$(TARGET1):  $(LIBOVRPATH)/Lib/Linux/$(RELEASETYPE)/$(SYSARCH)/libovr.a
	$(MAKE) -C $(LIBOVRPATH) DEBUG=$(DEBUG)

$(TARGET1):  $(OBJECTS1)
	$(LINK)  $(LFLAGS) -o $(TARGET1) $(OBJECTS1) $(LIBS)

$(TARGET2):  $(LIBOVRPATH)/Lib/Linux/$(RELEASETYPE)/$(SYSARCH)/libovr.a
	$(MAKE) -C $(LIBOVRPATH) DEBUG=$(DEBUG)

$(TARGET2):  $(OBJECTS2)
	$(LINK)  $(LFLAGS) -o $(TARGET2) $(OBJECTS2) $(LIBS) $(LIBSCV)

$(OBJPATH)/drc_head_manipulation.o: head_manip.cpp 
	$(CXX_BUILD)drc_head_manipulation.o head_manip.cpp

$(OBJPATH)/drc_vision.o: drc_vision.cpp
	$(CXX_BUILD)drc_vision.o drc_vision.cpp


clean:
	-$(DELETEFILE) $(OBJECTS1)
	-$(DELETEFILE) $(TARGET1)
	-$(DELETEFILE) $(OBJECTS2)
	-$(DELETEFILE) $(TARGET2)
