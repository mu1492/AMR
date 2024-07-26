cmake_minimum_required(VERSION 3.5)

project(RadioModTx VERSION 0.1 LANGUAGES CXX)

#########################
# Build flags
#########################
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions( -DOC_NEW_STYLE_INCLUDES=1 )

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_COMPILER /usr/bin/g++)
execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GXX_VERSION)
if(GXX_VERSION VERSION_LESS 12)
    message(FATAL_ERROR "Minimum required g++ is 12.0")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#########################
# Packages
#########################
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

#########################
# Include paths
#########################
include_directories(SYSTEM)
# HDF5
include_directories("/usr/include/hdf5/serial")
# PKL
include_directories("/usr/include/PTools170")
include_directories("/usr/include/PTools170/opencontainers_1_8_5/include")

#########################
# Project files
#########################
set(PROJECT_SOURCES
        main.cpp
        RadioModTx.cpp
        RadioModTx.h
        RadioModTx.ui
        Signal.cpp
        Signal.h
        Modulation.cpp
        Modulation.h
        Dataset.cpp
        Dataset.h
        DatasetParser.cpp
        DatasetParser.h
        Hdf5Parser.cpp
        Hdf5Parser.h
        PklParser.cpp
        PklParser.h
        CsvParser.cpp
        CsvParser.h
        TxHal.cpp
        TxHal.h
        AdiTrx.cpp
        AdiTrx.h
        AdiTrxAd9361.cpp
        AdiTrxAd9361.h
        AdiTrxAdrv9009.cpp
        AdiTrxAdrv9009.h
        AdiTrxAd9081.cpp
        AdiTrxAd9081.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RadioModTx MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(RadioModTx ${PROJECT_SOURCES})
endif()

#########################
# Linker and libraries
#########################
# HDF5
target_link_directories(RadioModTx PRIVATE "/usr/lib/x86_64-linux-gnu/hdf5/serial")
set(HDF5_LIBRARIES "libhdf5.so" "libhdf5_cpp.so" "libz.so")
# PKL
set(PKL_LIBRARIES "libptools.so")
# IIO
set(IIO_LIBRARIES "libiio.so")
# project libraries
target_link_libraries(RadioModTx PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${HDF5_LIBRARIES} ${PKL_LIBRARIES} ${IIO_LIBRARIES})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RadioModTx)
endif()
