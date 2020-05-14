target_include_directories(client PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_sources(client PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/WebsocketClient.h
        ${CMAKE_CURRENT_LIST_DIR}/WebsocketClient.cpp)