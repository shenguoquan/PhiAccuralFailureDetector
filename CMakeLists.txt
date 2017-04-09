cmake_minimum_required(VERSION 3.6)
project(PhiAccuralFailureDetector)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp headers/phiAccuralFailureDetector.h src/phiAccuralFailureDetector.cpp)
add_executable(PhiAccuralFailureDetector ${SOURCE_FILES})