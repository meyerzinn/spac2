target_sources(spaccommon
        INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/packet_generated.h)

target_include_directories(spaccommon INTERFACE ${CMAKE_CURRENT_LIST_DIR})