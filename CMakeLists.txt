cmake_minimum_required(VERSION 3.5.1)
project(magic_event)

set(CMAKE_CXX_STANDARD 14)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

IF (WIN32)
    add_definitions(-DWIN32)
    MESSAGE(STATUS "Now is windows")
ELSEIF (APPLE)
    add_definitions(-DAPPLE)
    MESSAGE(STATUS "Now is Apple systems.")
    set(PLATFORM_SRCS
            src/magic_event/poller/poller_kqueue_impl.cc)
ELSEIF (UNIX)
    add_definitions(-DUNIX)
    MESSAGE(STATUS "Now is UNIX-like OS's.")
    set(PLATFORM_SRCS
            src/magic_event/poller/poller_epoll_impl.cc)
ENDIF ()


set(SRCS
        src/magic_event/looper.cc
        src/magic_event/timer_list.cc
        src/magic_event/utils/time_utils.cc
        src/magic_event/connection.cc
        src/magic_event/acceptor.cc
        src/magic_event/server.cc)

add_library(${PROJECT_NAME} SHARED
    ${SRCS}
    ${PLATFORM_SRCS})

message(CMAKE_BUILD_TYPE:${CMAKE_BUILD_TYPE})

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
    add_executable(debug_${PROJECT_NAME} test/debug.cc)
    target_link_libraries(debug_${PROJECT_NAME}
        ${PROJECT_NAME}
        pthread)

    add_executable(looper_test test/looper_test.cc)
    target_link_libraries(looper_test
            ${PROJECT_NAME}
            pthread)

    add_executable(acceptor_test test/acceptor_test.cc)
    target_link_libraries(acceptor_test
            ${PROJECT_NAME}
            pthread)

    add_executable(server_test test/server_test.cc)
    target_link_libraries(server_test
            ${PROJECT_NAME}
            pthread)
endif ()
