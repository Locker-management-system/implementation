cmake_minimum_required(VERSION 3.27)
project(implementation C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(client)
include_directories(server)

add_executable(implementation
        client/cabinet_manager.h
        client/client.h
        client/user.h
        server/cabinet.h
        server/server.h
        server/user.h
        server/user_manger.h
        test/cabinet_manager_test.c
        cabinet.h
        cabinet_manager.c
        cabinet_manager.h
        client.h
        server.h
        user.h
        user_manger.h)
