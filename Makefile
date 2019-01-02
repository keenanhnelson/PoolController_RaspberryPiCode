#List of .cpp files: Controller.cpp  Server.cpp  ServoContinuous.cpp  ServoPosition.cpp  Source.cpp Stepper.cpp Webcam.cpp
#List of .hpp files: Config.hpp  Controller.hpp  Server.hpp  ServoContinuous.hpp  ServoPosition.hpp  Stepper.hpp Webcam.hpp

TARGET = out 
LIBS = -pthread -lwiringPi -lboost_system -lopencv_core -lopencv_imgcodecs -lopencv_videoio
INCLUDES = -I/usr/local/include/opencv4 -I./Inc
BUILD_DIR = ./Build
SRC_DIR = ./Src
INC_DIR = ./Inc

CC = g++
CFLAGS = -std=c++11 -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
HEADERS = $(wildcard $(INC_DIR)/*.hpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))

$(BUILD_DIR)/ServoContinuous.o: $(SRC_DIR)/ServoContinuous.cpp $(INC_DIR)/ServoContinuous.hpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@	
	
$(BUILD_DIR)/ServoPosition.o: $(SRC_DIR)/ServoPosition.cpp $(INC_DIR)/ServoPosition.hpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@	
	
$(BUILD_DIR)/Stepper.o: $(SRC_DIR)/Stepper.cpp $(INC_DIR)/Stepper.hpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
$(BUILD_DIR)/Webcam.o: $(SRC_DIR)/Webcam.cpp $(INC_DIR)/Webcam.hpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
$(BUILD_DIR)/Controller.o: $(SRC_DIR)/Controller.cpp $(INC_DIR)/Controller.hpp $(BUILD_DIR)/Stepper.o $(BUILD_DIR)/ServoPosition.o
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@	

$(BUILD_DIR)/Server.o: $(SRC_DIR)/Server.cpp $(INC_DIR)/Server.hpp $(BUILD_DIR)/Controller.o $(BUILD_DIR)/Webcam.o
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@	
	
$(BUILD_DIR)/Source.o: $(SRC_DIR)/Source.cpp $(INC_DIR)/Config.hpp $(BUILD_DIR)/Server.o
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
