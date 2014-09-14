#cmake compatibility issues: don't modify this, please!
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

#allow more human readable "if then else" constructs
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

#verbose makefile on
SET(CMAKE_VERBOSE_MAKEFILE ON)

####################
# project settings #
####################

PROJECT(radolan)

# add project include directories first
INCLUDE_DIRECTORIES(BEFORE ${PROJECT_SOURCE_DIR}/include)

#set c++
ENABLE_LANGUAGE(CXX)

SET(${PROJECT_NAME}_MAJOR_VERSION 1)
SET(${PROJECT_NAME}_MINOR_VERSION 0)
SET(${PROJECT_NAME}_PATCH_LEVEL 0)

#should not be defined, only if doxygen is found, but then people would forget
#to update it, because it would be down below
#also, actually it might be variables in variables are supported in cmake,
#then it is not needed at all
SET(FULL_VERSION_FOR_DOXYGEN 0.1.0)

#setup OS
IF(CMAKE_SYSTEM_NAME MATCHES Darwin)
    SET(${PROJECT_NAME}_OS_MACOSX TRUE)
ELSE()
  IF(CMAKE_SYSTEM_NAME MATCHES Linux)
    SET(${PROJECT_NAME}_OS_LINUX TRUE)
  ELSE()
    MESSAGE(FATAL_ERROR "unknown target OS")
  ENDIF()
ENDIF()

# this is a speciality on my mac where
# clang was compiled and it has a bug
# http://bisqwit.iki.fi/story/howto/openmp/

IF (CMAKE_CXX_COMPILER MATCHES ".*clang")
    MESSAGE(STATUS "Adding include dirs for LLVM/OpenMP on OSX 10.9")
#    INCLUDE_DIRECTORIES(/usr/include/c++/4.2.1)
#    INCLUDE_DIRECTORIES(/usr/include/c++/4.2.1/backward/)
#   MESSAGE(STATUS "Setting -stdlib=libc++" linkflags="-stdlib=libc++")
#   SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++0x -stdlib=libc++ -g")
#   SET(CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++")
#   SET(CMAKE_SHARED_LINKER_FLAGS "-stdlib=libc++")
    ADD_DEFINITIONS(-DBOOST_HAS_INT128=1)
ENDIF()

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules")

#################################
#dependencies on other libraries#
#################################

find_package( ZLIB REQUIRED )
IF (NOT ZLIB_FOUND)
   MESSAGE(FATAL_ERROR "HDF5 not found INCLUDE_DIR=${HDF5_INCLUDE_DIR} LIBRARIES=${HDF5_LIBRARIES}")
ENDIF()

# HDF5
FIND_PACKAGE(HDF5)
IF (NOT HDF5_FOUND)
   MESSAGE(FATAL_ERROR "HDF5 not found INCLUDE_DIR=${HDF5_INCLUDE_DIR} LIBRARIES=${HDF5_LIBRARIES}")
ENDIF()

# netcdf
FIND_PACKAGE(NETCDF)
IF (NOT NETCDF_FOUND)
   MESSAGE(FATAL_ERROR "netcdf(cxx) not found (http://www.unidata.ucar.edu/software/netcdf/docs/netcdf-cxx/)")
ENDIF()

# Shapefile
FIND_PACKAGE(SHP)
IF (NOT SHP_FOUND)
   MESSAGE(FATAL_ERROR "Shapefile not found INCLUDE_DIR=${SHP_INCLUDE_DIR} LIBRARIES=${SHP_LIBRARIES}")
ENDIF()

#add include directories
INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/include
	${ZLIB_INCLUDE_DIRS}
	${HDF5_INCLUDEDIR}
	${SHP_INCLUDE_DIR}
)

#######################
#create configure file#
#######################

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/config_h.in
               ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/config.h)

#set default cmake build type to Debug
#(None Debug Release RelWithDebInfo MinSizeRel)

IF(NOT CMAKE_BUILD_TYPE)
SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

#the library
ADD_LIBRARY(radolan SHARED 
    src/read.c
    src/coordinatesystem.cpp
    src/radolan_utils.cpp
    src/observationfile.cpp
    src/shapefile.cpp
    include/radolan/coordinatesystem.h
    include/radolan/endianess.h
    include/radolan/observationfile.h
    include/radolan/radolan.h
    include/radolan/radolan_utils.h
    include/radolan/read.h
    include/radolan/shapefile.h
    include/radolan/types.h
)

TARGET_LINK_LIBRARIES(radolan
   ${ZLIB_LIBRARIES}
   ${HDF5_LIBRARIES}
   ${SHP_LIBRARIES}
   ${NETCDF_LIBRARIES}
)

# Shapefile tool

ADD_EXECUTABLE(radolan2shapefile
    src/radolan2shapefile.cpp
)

# test executable

ADD_EXECUTABLE(radolan_test
    src/radolan_test.cpp
)

TARGET_LINK_LIBRARIES(radolan_test
    radolan
    ${ZLIB_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${SHP_LIBRARIES}
)

TARGET_LINK_LIBRARIES(radolan2shapefile
    radolan
    ${ZLIB_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${SHP_LIBRARIES}
)

######################
#documentation target#
######################

INCLUDE(FindDoxygen)
IF(DOXYGEN_EXECUTABLE)
    IF(DOXYGEN_DOT_EXECUTABLE)
        SET(UBIQ_DOXYGEN_HAVE_DOT "YES")
    ELSE(DOXYGEN_DOT_EXECUTABLE)
        SET(UBIQ_DOXYGEN_HAVE_DOT "NO")
    ENDIF(DOXYGEN_DOT_EXECUTABLE)
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/doxy.in
                   ${PROJECT_BINARY_DIR}/doxyfile)
    FILE(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/doc/html)
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/doc/footer_file
                   ${PROJECT_BINARY_DIR}/doc/html
                   COPYONLY)
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/doc/header_file
                   ${PROJECT_BINARY_DIR}/doc/html
                   COPYONLY)
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/doc/logo.png
                   ${PROJECT_BINARY_DIR}/doc/html
                   COPYONLY)
    ADD_CUSTOM_TARGET(doc 
        COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/doxyfile)
ENDIF(DOXYGEN_EXECUTABLE)

################
#install target#
################

INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/${PROJECT_NAME} DESTINATION "/usr/local/include")
INSTALL(DIRECTORY include/${PROJECT_NAME}
        DESTINATION "/usr/local/include"
        PATTERN ".svn" EXCLUDE
        PATTERN ".git" EXCLUDE
        PATTERN "*.swp" EXCLUDE)

INSTALL(TARGETS radolan LIBRARY DESTINATION "/usr/local/lib")
INSTALL(TARGETS radolan2shapefile RUNTIME DESTINATION "/usr/local/bin")
