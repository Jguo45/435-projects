cmake_minimum_required(VERSION 3.6...3.19)
project(trace)
set(TARGET trace)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET})


# set C++ version to use
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# I'm lazy here, and building with all cpp files
file(GLOB SOURCES  "*.cpp" "*.cxx" "*.cc" "*.c")
file(GLOB INCLUDES "*.hpp" "*.hxx" "*.hh" "*.h")
file(GLOB INLINES  "*.inl" "*.ixx" "*.ii" "*.i")
add_executable(${TARGET} ${SOURCES} ${INCLUDES} ${INLINES})

