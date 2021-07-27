CC = g++
CFLAGS = -Wall -g
TARGET = Program
LDLIBS  = -lcurl -lrpcrt4

# $(wildcard *.cpp /xxx/xxx/*.cpp): get all .cpp files from the current directory and dir "/xxx/xxx/"
SRCS := $(wildcard *.cpp)
# $(patsubst %.cpp,%.o,$(SRCS)): substitute all ".cpp" file name strings to ".o" file name strings
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

# Compile sources with args
all: $(TARGET) | clean run
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDLIBS)
%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

# Clean object files
clean:
	rm -rf $(OBJS)

# Run program	
run:
	.\$(TARGET).exe

# Help command 
help:
	@echo "Please use 'make <target>' where <target> is one of"
	@echo ""
	@echo "  all         to run all commands altogether."
	@echo "  help        obtain this page."
	@echo "  run         run main code."
	@echo "  clean       remove *.o files."
	@echo ""
	@echo "Execute 'make' standalone to run everything necessary."
	@echo "Check the Makefile to know exactly what each target is doing."

.PHONY: all clean run help
