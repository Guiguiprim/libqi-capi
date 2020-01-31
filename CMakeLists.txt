##
## Author(s):
##  - Cedric GESTES      <gestes@aldebaran-robotics.com>
##
## Copyright (C) 2012 Aldebaran Robotics
##
cmake_minimum_required(VERSION 2.6.4)
project(libqi-capi)
find_package(qibuild)

enable_testing()

qi_sanitize_compile_flags(HIDDEN_SYMBOLS)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

qi_create_lib("qic" SHARED qic/api.h
                     qic/application.h
                     qic/session.h
                     qic/object.h
                     qic/future.h
                     qic/value.h
                     qic/type.h

                     src/object_p.h
                     src/object.cpp
                     src/future_p.h
                     src/future.cpp
                     src/session.cpp
                     src/application.cpp
                     src/value_p.h
                     src/value.cpp
                     src/type.cpp)

find_package(QI REQUIRED)
target_link_libraries(qic -Wl,--whole-archive ${QI_LIBRARIES} -Wl,--no-whole-archive)
include_directories(${QI_INCLUDE_DIRS})

find_package(OPENSSL REQUIRED)
target_link_libraries(qic ${OPENSSL_LIBRARIES})

find_package(BOOST_FILESYSTEM REQUIRED)
target_link_libraries(qic ${BOOST_FILESYSTEM_LIBRARIES})

find_package(BOOST_SYSTEM REQUIRED)
target_link_libraries(qic ${BOOST_SYSTEM_LIBRARIES})

find_package(BOOST_CHRONO REQUIRED)
target_link_libraries(qic ${BOOST_CHRONO_LIBRARIES})

find_package(BOOST_RANDOM REQUIRED)
target_link_libraries(qic ${BOOST_RANDOM_LIBRARIES})

find_package(BOOST_CHRONO REQUIRED)
target_link_libraries(qic ${BOOST_CHRONO_LIBRARIES})

find_package(BOOST_PROGRAM_OPTIONS REQUIRED)
target_link_libraries(qic ${BOOST_PROGRAM_OPTIONS_LIBRARIES})

find_package(BOOST_THREAD REQUIRED)
target_link_libraries(qic ${BOOST_THREAD_LIBRARIES})

find_package(BOOST_REGEX REQUIRED)
target_link_libraries(qic ${BOOST_REGEX_LIBRARIES})

qi_stage_lib(qic)

add_subdirectory("test")
