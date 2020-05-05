target_include_directories(common INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_sources(common
        INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/System.h
            ${CMAKE_CURRENT_LIST_DIR}/System.cpp)