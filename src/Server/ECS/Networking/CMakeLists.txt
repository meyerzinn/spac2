target_include_directories(server PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_sources(server
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/WebsocketSession.h
        ${CMAKE_CURRENT_LIST_DIR}/WebsocketSession.cpp)
