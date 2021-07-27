CC = g++
CFLAGS = -Wall -g
TARGET = Program
LDLIBS  = -lcurl -lrpcrt4

# $(wildcard *.cpp /xxx/xxx/*.cpp): get all .cpp files from the current directory and dir "/xxx/xxx/"
SRCS := $(wildcard *.cpp)
# $(patsubst %.cpp,%.o,$(SRCS)): substitute all ".cpp" file name strings to ".o" file name strings
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

all: $(TARGET) | clean run
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDLIBS)
%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(OBJS)
	
run:
	.\Program.exe

.PHONY: all clean run
