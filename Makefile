all:
clean:

.PHONY: all clean

## server

COMMON_CFLAGS = -g -Wall -O2

CFLAGS = $(COMMON_CFLAGS) `pkg-config cairo --cflags` `pkg-config sdl --cflags`
CFLAGS += -I/usr/local/include/SDL -Icommon -fPIC

CXXFLAGS  = $(COMMON_CFLAGS) -Wno-write-strings
CXXFLAGS += `pkg-config cairo --cflags` `pkg-config sdl --cflags`
CXXFLAGS += -Icommon -fPIC

LDFLAGS  = -g -L/usr/lib/x86_64-linux-gnu/
LDFLAGS += -Wl,-rpath,/usr/lib/x86_64-linux-gnu/
LDFLAGS += `pkg-config cairo --libs` `pkg-config sdl --libs`
LDFLAGS += -pthread -lm

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
ROBOTS += team4918 jeff jack max ella gage sam emma olivia mary
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

team4918: clients/team4918.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

jeff: clients/jeff.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

jack: clients/jack.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

max: clients/max.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

ella: clients/ella.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

gage: clients/gage.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

sam: clients/sam.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

emma: clients/emma.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

olivia: clients/olivia.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

mary: clients/mary.cpp clients/robots.h robots.a
	$(CXX) $(COMMON_CFLAGS) -o $@ $< -lm robots.a

## overall rules

all: robotserver $(ROBOTS)

edit: clients/team4918.cpp
	gvim clients/team4918.cpp

run: $(ROBOTS)
	./fight team4918 rabbit

gitstatusshort:	
	git status -s

gitstatus:	
	git status

gitdiff:	
	git diff clients/team4918.cpp

gitdifftool:	
	git difftool -t tkdiff clients/team4918.cpp

gitup: clients/team4918.cpp
	git add clients/team4918.cpp
	git commit
	git push origin master

gitadd: clients/team4918.cpp
	git add clients/team4918.cpp

gitcommit:	
	git commit

gitpush:	
	git push origin master

gitpull:	
	git pull

clean:
	rm -f $(ROBOTS) $(LIBROBOTS_OBJS) robots.a robotserver $(MAIN_OBJ)

