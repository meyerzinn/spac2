add_executable(server main.cpp)
target_include_directories(server PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Utilities)

add_subdirectory(ECS)

find_package(Threads REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(unofficial-box2d CONFIG REQUIRED)
find_package(Flatbuffers CONFIG REQUIRED)
find_package(Boost 1.72 REQUIRED COMPONENTS log)
find_package(ZLIB REQUIRED)

# uwebsockets
find_path(USOCKETS_INCLUDE_DIR libusockets.h)
find_path(UWEBSOCKETS_INCLUDE_DIR uwebsockets/App.h)
find_library(USOCKETS_LIBRARY NAMES uSockets.lib libuSockets.a)

target_include_directories(server PRIVATE
        ${USOCKETS_INCLUDE_DIR}
        ${UWEBSOCKETS_INCLUDE_DIR}
        ${Boost_INCLUDE_DIR})

target_link_libraries(server PRIVATE
        common
        ${CMAKE_THREAD_LIBS_INIT}
        EnTT::EnTT
        unofficial::box2d::box2d
        flatbuffers::flatbuffers
        ${USOCKETS_LIBRARY}
        ZLIB::ZLIB
        ${Boost_LIBRARIES})

configure_file(${CMAKE_CURRENT_LIST_DIR}/../../certs/key.pem ${CMAKE_CURRENT_BINARY_DIR}/certs/key.pem COPYONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/../../certs/cert.pem ${CMAKE_CURRENT_BINARY_DIR}/certs/cert.pem COPYONLY)