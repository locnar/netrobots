all:
clean:

## jag; 14nov2018 -- This Makefile modified to build on Ubuntu systems
##                   (the original version worked fine on FreeBSD).
##                   I had to copy the /usr/local/include/cairo and
##                   /usr/local/include/SDL directories from a FreeBSD
##                   machine, since those include files didn't exist here,
##                   but the libraries did already exist.
## jag; 16nov2018 -- Converted to C++;
##                   increased warning level and fixed several warnings.

.PHONY: all clean

## server

COMMON_CFLAGS = -g -Wall -O2
CFLAGS  = $(COMMON_CFLAGS) -I/usr/local/include/cairo
CFLAGS += -I/usr/local/include/SDL -Icommon -fPIC 
CXXFLAGS  = $(COMMON_CFLAGS) -Wno-write-strings
CXXFLAGS += -I/usr/local/include/cairo -I/usr/local/include/SDL
CXXFLAGS += -Icommon -fPIC 
##LDFLAGS  = -g -L/usr/lib/x86_64-linux-gnu/ -l:libcairo.so.2
##LDFLAGS += -Wl,-rpath,/usr/lib/x86_64-linux-gnu/ -l:libSDL-1.2.so.0
##LDFLAGS += -pthread -lm 
LDFLAGS = -g `pkg-config cairo --libs` `pkg-config sdl --libs` -pthread -lm
MAIN_OBJ  = server/main.o server/drawing.o server/field.o
MAIN_OBJ += common/net_utils.o server/net_commands.o
MAIN_OBJ += server/net_core.o server/robotserver.o

robotserver: $(MAIN_OBJ)
	$(CXX) -o robotserver $(MAIN_OBJ) $(LDFLAGS)
server/field.o: server/field.cpp server/drawing.h server/field.h
server/testlogic.o: server/testlogic.cpp server/robotserver.h
server/robotserver.o: server/robotserver.cpp server/robotserver.h
server/drawing.o: server/drawing.cpp server/drawing.h
server/net_core.o: server/net_core.cpp server/robotserver.h common/net_utils.h server/net_defines.h server/field.h
server/net_commands.o: server/net_commands.cpp server/net_defines.h server/robotserver.h

common/net_utils.o: common/net_utils.cpp common/net_utils.h

## robots

ROBOTS = counter rabbit rook sniper spot
LIBROBOTS_OBJS = clients/robots.o common/net_utils.o

all-robots: $(ROBOTS)

clients/robots.o: clients/robots.cpp
robots.a: $(LIBROBOTS_OBJS)
	ar crD $@ $(LIBROBOTS_OBJS)

counter: clients/counter.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

rabbit: clients/rabbit.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

rook: clients/rook.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

sniper: clients/sniper.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

spot: clients/spot.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

## overall rules

all: robotserver $(ROBOTS)

edit:	clients/rook.cpp
	gvim clients/rook.cpp

run:	$(ROBOTS)
	./fight rook rabbit

clean:
	rm -f $(ROBOTS) $(LIBROBOTS_OBJS) robots.a robotserver $(MAIN_OBJ)

