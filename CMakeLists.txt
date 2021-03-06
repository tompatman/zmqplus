# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required(VERSION 2.8.11)
#Set the project name
set(PROJ_NAME zmqplus)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wextra -fmessage-length=0 -Wno-unused-function -Wno-format-security -Werror=return-type \
 -Wno-missing-field-initializers -Wno-psabi -Wformat-truncation=1")
# -Wno-missing-field-initializers -Wno-psabi -Wformat-truncation=1 -Wreorder -Werror -fmax-errors=5 ")

#Add a flag for stack trace with debug builds
message("CMAKE_C_FLAGS_DEBUG is ${CMAKE_C_FLAGS_DEBUG}")
message("CMAKE_C_FLAGS_RELEASE is ${CMAKE_C_FLAGS_RELEASE}")

set(PROJ_PATH ${CMAKE_CURRENT_LIST_DIR})
project(${PROJ_NAME})

file(GLOB PROJ_SRC
        "./util/*.cpp"
        "./include/*.h"
        "./src/*.cpp"
        )

file(GLOB_RECURSE ZMQPLUS_INCLUDE_FILES "include/*.h")

include_directories(${CMAKE_SOURCE_DIR}/output/${PROJ_NAME}
        ./include
        )

set(SOURCE_FILES ${PROJ_SRC})


if (NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX d)
endif ()

add_library(${PROJ_NAME} SHARED ${SOURCE_FILES})
add_library(${PROJ_NAME}${CMAKE_DEBUG_POSTFIX} ALIAS ${PROJ_NAME})
set_target_properties(${PROJ_NAME} PROPERTIES LINKER_LANGUAGE CXX PUBLIC_HEADER "${ZMQPLUS_INCLUDE_FILES}")

# Add google test to support unit testing
# add_subdirectory(googletest)
# add_subdirectory(testsuite)

#Installation rules
#Do not want google test to install, disable that here
SET(INSTALL_GTEST OFF CACHE BOOL "Disabling google test installation" FORCE)
install(TARGETS ${PROJ_NAME}
        LIBRARY
        DESTINATION lib
        CONFIGURATIONS Release
        RUNTIME DESTINATION /usr/local/lib
        PUBLIC_HEADER
        DESTINATION /usr/local/include/${PROJ_NAME})
