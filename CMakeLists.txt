cmake_minimum_required(VERSION 2.8)
project( Main )
set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
find_package( OpenCV REQUIRED )
add_executable( out Source.cpp )
include_directories( /home/pi/Boost/boost_1_63_0 )
link_directories( /home/pi/Boost/boost_1_63_0/stage/lib )
target_link_libraries( out ${OpenCV_LIBS} wiringPi pthread boost_system)



#add_compile_options( -std=c++11 )
#set (CMAKE_CXX_STANDARD 11)
