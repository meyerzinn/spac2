target_include_directories(spacclient PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_sources(spacclient PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Starfield.h
        ${CMAKE_CURRENT_LIST_DIR}/Starfield.cpp)