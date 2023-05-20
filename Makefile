#System specific options. Uncomment either Cross compiled or Raspberry Pi native compiled.
#Cross compiled
INCLUDES = -I ./Inc -I ${PoolControllerProjectPath}/cross-pi-gcc-10.3.0-64/aarch64-linux-gnu/libc/usr/include/opencv4 -I ${PoolControllerProjectPath}/boost_1_82_0
EXTRA_LINK_OPTIONS = -Wl,-rpath-link,${PoolControllerProjectPath}/cross-pi-gcc-10.3.0-64/aarch64-linux-gnu/libc/usr/lib/aarch64-linux-gnu
CC = ${PoolControllerProjectPath}/cross-pi-gcc-10.3.0-64/bin/aarch64-linux-gnu-g++
#Raspberry Pi native compiled
#INCLUDES = -I ./Inc -I /usr/include/opencv4 -I ${PoolControllerProjectPiPath}/boost_1_82_0
#EXTRA_LINK_OPTIONS = 
#CC = g++

TARGET = out 
LIBS_DIR = -L 
LIBS = -lrt -lopencv_core -lopencv_imgcodecs -lopencv_videoio -lpigpio -pthread
BUILD_DIR = ./Build
SRC_DIR = ./Src
INC_DIR = ./Inc
CFLAGS = -std=c++11 -g

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
	$(CC) $(OBJECTS) -Wall $(EXTRA_LINK_OPTIONS) $(LIBS_DIR) $(LIBS) -o $(BUILD_DIR)/$@

clean:
	-rm -r $(BUILD_DIR)
