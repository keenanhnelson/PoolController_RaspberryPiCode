#This makefile doesn't work if the header file is updated

TARGET = out 
LIBS = -pthread -lwiringPi -lboost_system -lopencv_core -l opencv_imgcodecs -lopencv_videoio

CC = g++
CFLAGS = -std=c++11 -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.hpp)
OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

%.o: %.cpp 
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
