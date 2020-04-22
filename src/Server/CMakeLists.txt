project(spacserver)

add_executable(${PROJECT_NAME} main.cpp)
add_executable(spac::server ALIAS ${PROJECT_NAME})

set_property(TARGET spacserver PROPERTY CXX_STANDARD 17)

target_include_directories(spacserver PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Utilities)

add_subdirectory(ECS)

find_package(Threads REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(unofficial-box2d CONFIG REQUIRED)
find_package(Flatbuffers CONFIG REQUIRED)
find_package(Boost 1.72 REQUIRED)
find_package(ZLIB REQUIRED)

# uwebsockets
find_path(USOCKETS_INCLUDE_DIR libusockets.h)
find_path(UWEBSOCKETS_INCLUDE_DIR uwebsockets/App.h)
find_library(USOCKETS_LIBRARY libuSockets.a)

target_include_directories(spacserver PRIVATE
        ${USOCKETS_INCLUDE_DIR}
        ${UWEBSOCKETS_INCLUDE_DIR}
        ${Boost_INCLUDE_DIR})

target_link_libraries(spacserver PRIVATE
        spac::common
        ${CMAKE_THREAD_LIBS_INIT}
        EnTT::EnTT
        unofficial::box2d::box2d
        flatbuffers::flatbuffers
        ${USOCKETS_LIBRARY}
        ZLIB::ZLIB
        ${Boost_LIBRARIES})