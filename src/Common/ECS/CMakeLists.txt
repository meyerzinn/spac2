target_include_directories(spaccommon INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_sources(spaccommon
        INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/System.h
            ${CMAKE_CURRENT_LIST_DIR}/System.cpp)