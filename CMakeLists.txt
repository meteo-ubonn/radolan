# -------------------------------------
# cmake configuration
# -------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_MACOSX_RPATH OFF)

# -------------------------------------
# project settings 
# -------------------------------------

PROJECT(radolan)
ENABLE_LANGUAGE(CXX)
SET(${PROJECT_NAME}_MAJOR_VERSION 1)
SET(${PROJECT_NAME}_MINOR_VERSION 1)
SET(${PROJECT_NAME}_PATCH_LEVEL 0)
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules")
INCLUDE_DIRECTORIES(BEFORE ${PROJECT_SOURCE_DIR}/include)

# -------------------------------------
# Operating system
# -------------------------------------

IF(CMAKE_SYSTEM_NAME MATCHES Darwin)
    SET(${PROJECT_NAME}_OS_MACOSX TRUE)
ELSE()
  IF(CMAKE_SYSTEM_NAME MATCHES Linux)
    SET(${PROJECT_NAME}_OS_LINUX TRUE)
  ELSE()
    MESSAGE(FATAL_ERROR "unknown target OS")
  ENDIF()
ENDIF()

# -------------------------------------
# Build tests?
# -------------------------------------

IF (NOT WITH_TESTS)
    SET(TESTS_ENABLED 0)
ELSE()
    SET(TESTS_ENABLED ${WITH_TESTS})
ENDIF()

# -------------------------------------
# Compiler flags
# -------------------------------------

IF (APPLE)
    IF (CMAKE_CXX_COMPILER MATCHES ".*clang")
        # Boost storage type problem
        ADD_DEFINITIONS(-DBOOST_HAS_INT128=1)
    ENDIF()
ENDIF()

# -------------------------------------
# dependencies on other libraries
# -------------------------------------

FIND_PACKAGE( ZLIB REQUIRED )
IF (NOT ZLIB_FOUND)
   MESSAGE(FATAL_ERROR "ZLIB not found (http://www.zlib.net)")
ELSE()
    MESSAGE(STATUS "zlib found")
    INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
ENDIF()

# HDF5
FIND_PACKAGE(HDF5)
IF (NOT HDF5_FOUND)
   MESSAGE(FATAL_ERROR "HDF5 not found (http://www.hdfgroup.org/HDF5/)")
ELSE()
    MESSAGE(STATUS "HDF5 found")
    INCLUDE_DIRECTORIES(${HDF5_INCLUDEDIR})
ENDIF()

# netcdf
FIND_PACKAGE(NETCDF)
IF (NOT NETCDF_FOUND)
   MESSAGE(FATAL_ERROR "netcdf(cxx) not found (http://www.unidata.ucar.edu/software/netcdf/docs/netcdf-cxx/)")
ELSE()
    MESSAGE(STATUS "NETCDF found")
    INCLUDE_DIRECTORIES(${NETCDF_INCLUDEDIR})
ENDIF()

# Shapefile
FIND_PACKAGE(SHP)
IF (NOT SHP_FOUND)
    ADD_DEFINITIONS(-DWITH_SHAPELIB=0)
    MESSAGE(WARNING "shapelib not found (http://shapelib.maptools.org/). Disabling shapelib code.")
ELSE()
    ADD_DEFINITIONS(-DWITH_SHAPELIB=1)
    MESSAGE(STATUS "Shapefile found")
    INCLUDE_DIRECTORIES(${SHP_INCLUDE_DIR})
ENDIF()

# -------------------------------------
# configure file
# -------------------------------------

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/config_h.in
               ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/config.h)

#set default cmake build type to Debug
#(None Debug Release RelWithDebInfo MinSizeRel)

# -------------------------------------
# Build Type
# -------------------------------------

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

# -------------------------------------
# Library and executables
# -------------------------------------

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

IF (SHP_FOUND)
    TARGET_LINK_LIBRARIES(radolan
       ${ZLIB_LIBRARIES}
       ${HDF5_LIBRARIES}
       ${SHP_LIBRARIES}
       ${NETCDF_LIBRARIES}
       ${APPLE_STDLIBCXX})
ELSE()
    TARGET_LINK_LIBRARIES(radolan
            ${ZLIB_LIBRARIES}
            ${HDF5_LIBRARIES}
            ${NETCDF_LIBRARIES}
            ${APPLE_STDLIBCXX})
ENDIF()

# Shapefile tool
IF (SHP_FOUND)
    ADD_EXECUTABLE(radolan2shapefile
        src/radolan2shapefile.cpp
    )
    TARGET_LINK_LIBRARIES(radolan2shapefile
       radolan
       ${ZLIB_LIBRARIES}
       ${HDF5_LIBRARIES}
       ${SHP_LIBRARIES}
       ${APPLE_STDLIBCXX}
    )
ENDIF()

# -------------------------------------
# Tests
# -------------------------------------

IF (TESTS_ENABLED)
    ADD_EXECUTABLE(radolan_test
        src/radolan_test.cpp)
    IF (SHP_FOUND)
        TARGET_LINK_LIBRARIES(radolan_test
            radolan
            ${ZLIB_LIBRARIES}
            ${HDF5_LIBRARIES}
            ${SHP_LIBRARIES}
            ${APPLE_STDLIBCXX})
    ELSE()
        TARGET_LINK_LIBRARIES(radolan_test
            radolan
            ${ZLIB_LIBRARIES}
            ${HDF5_LIBRARIES}
            ${APPLE_STDLIBCXX})
    ENDIF()
ENDIF()

# -------------------------------------
# documentation
# -------------------------------------

SET(FULL_VERSION_FOR_DOXYGEN 0.1.0)

INCLUDE(FindDoxygen)
IF(DOXYGEN_EXECUTABLE)
    IF(DOXYGEN_DOT_EXECUTABLE)
        SET(UBIQ_DOXYGEN_HAVE_DOT "YES")
    ELSE(DOXYGEN_DOT_EXECUTABLE)
        SET(UBIQ_DOXYGEN_HAVE_DOT "NO")
    ENDIF(DOXYGEN_DOT_EXECUTABLE)
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/doxy.in
                   ${PROJECT_BINARY_DIR}/Doxyfile)
    FILE(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/html/radolan/doxygen)
    ADD_CUSTOM_TARGET(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)
ENDIF(DOXYGEN_EXECUTABLE)

# -------------------------------------
# install
# -------------------------------------

INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/${PROJECT_NAME} DESTINATION "/usr/local/include")
INSTALL(DIRECTORY include/${PROJECT_NAME}
        DESTINATION "/usr/local/include"
        PATTERN ".svn" EXCLUDE
        PATTERN ".git" EXCLUDE
        PATTERN "*.swp" EXCLUDE)

INSTALL(TARGETS radolan LIBRARY DESTINATION "/usr/local/lib")
IF (SHP_FOUND)
    INSTALL(TARGETS radolan2shapefile RUNTIME DESTINATION "/usr/local/bin")
ENDIF()
