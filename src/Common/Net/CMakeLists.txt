target_sources(common
        INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/packet_generated.h
        ${CMAKE_CURRENT_LIST_DIR}/Connection.h)

target_include_directories(common INTERFACE ${CMAKE_CURRENT_LIST_DIR})