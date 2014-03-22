CXX=g++
RM=rm -f
NDEBUG=_DEBUG
CPPFLAGS=-g -c -D $(NDEBUG)
LINKERJUNK=$(shell pkg-config --libs --cflags opencv) $(shell pkg-config --libs --cflags Qt5Widgets) $(shell pkg-config --libs --cflags Qt5OpenGL) $(shell pkg-config --libs --cflags Qt5Test) $(shell pkg-config --libs --cflags libavcodec)

all: bin/main.o
	$(CXX) -o bin/GraphicsProj bin/main.o $(LINKERJUNK) bin/Hands.o bin/Hand.o bin/Util.o

bin/main.o: main.cpp bin/Util.o bin/Hands.o
	$(CXX) $(CPPFLAGS) -o bin/main.o main.cpp

bin/Hands.o: Hands.h Hands.cpp Line.h bin/Hand.o bin/Util.o
	$(CXX) $(CPPFLAGS) -o bin/Hands.o Hands.cpp

bin/Hand.o: bin/Util.o Hand.cpp Hand.h
	$(CXX) $(CPPFLAGS) -o bin/Hand.o Hand.cpp

bin/Util.o: Util.h Util.cpp Colour.h
	$(CXX) $(CPPFLAGS) -o bin/Util.o Util.cpp

clean:
	$(RM) bin/*.o
	$(RM) bin/GraphicsProj
