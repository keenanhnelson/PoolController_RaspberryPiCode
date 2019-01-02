#List of .cpp files: Controller.cpp  Server.cpp  ServoContinuous.cpp  ServoPosition.cpp  Source.cpp Stepper.cpp Webcam.cpp
#List of .hpp files: Config.hpp  Controller.hpp  Server.hpp  ServoContinuous.hpp  ServoPosition.hpp  Stepper.hpp Webcam.hpp

TARGET = out 
LIBS = -pthread -lwiringPi -lboost_system -lopencv_core -lopencv_imgcodecs -lopencv_videoio
INCLUDES = -I/usr/local/include/opencv4
BUILD_DIR = ./Build

CC = g++
CFLAGS = -std=c++11 -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.hpp)
OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(wildcard *.cpp))

$(BUILD_DIR)/ServoContinuous.o: ServoContinuous.cpp ServoContinuous.hpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@	
	
$(BUILD_DIR)/ServoPosition.o: ServoPosition.cpp ServoPosition.hpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@	
	
$(BUILD_DIR)/Stepper.o: Stepper.cpp Stepper.hpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
$(BUILD_DIR)/Webcam.o: Webcam.cpp Webcam.hpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
$(BUILD_DIR)/Controller.o: Controller.cpp Controller.hpp $(BUILD_DIR)/Stepper.o $(BUILD_DIR)/ServoPosition.o
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@	

$(BUILD_DIR)/Server.o: Server.cpp Server.hpp $(BUILD_DIR)/Controller.o $(BUILD_DIR)/Webcam.o
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@	
	
$(BUILD_DIR)/Source.o: Source.cpp Config.hpp $(BUILD_DIR)/Server.o
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@	

#general run to build all cpp files but doesn't account for dependencies
#$(BUILD_DIR)/%.o: %.cpp 
#	@mkdir -p $(BUILD_DIR)
#	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
	-rm -r $(BUILD_DIR)
