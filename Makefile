CXXFLAGS =	-std=c++11 -O3 -g -Wall -msse -msse2 -fmessage-length=0 -fno-strict-aliasing

OBJS =		main.o

LIBS =

TARGET =	main

$(TARGET):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
