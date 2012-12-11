include os.mk

ifdef WIN32
VLOG_CFLAGS = -Id:/iverilog/include/iverilog
VLOG_LIBS = -Ld:/iverilog/lib -lvpi
JAVA_HOME = "c:/Program Files/Java/jdk1.7.0_07"
JVM_CFLAGS = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/win32
JVM_LIBS = -L$(JAVA_HOME)/lib -ljvm
CC = i686-pc-mingw32-gcc
else
VLOG_CFLAGS = -I/usr/include/iverilog -fPIC
JVM_CFLAGS = -I$(JAVA_HOME)/include
JVM_LIBS = -L$(JAVA_HOME)/jre/lib/amd64/server -ljvm
CC = gcc
endif

