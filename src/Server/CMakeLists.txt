add_executable(server main.cpp)
target_include_directories(server PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Utilities)

add_subdirectory(ECS)

find_package(EnTT CONFIG REQUIRED)
find_package(unofficial-box2d CONFIG REQUIRED)
find_package(Flatbuffers CONFIG REQUIRED)
find_package(Boost 1.72 REQUIRED COMPONENTS log)
find_package(OpenSSL REQUIRED)

target_include_directories(server PRIVATE
        ${Boost_INCLUDE_DIR})

target_link_libraries(server PRIVATE
        common
        ${CMAKE_THREAD_LIBS_INIT}
        EnTT::EnTT
        unofficial::box2d::box2d
        flatbuffers::flatbuffers
        OpenSSL::SSL
        OpenSSL::Crypto
        ${Boost_LIBRARIES})

target_compile_options(server PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/bigobj>)

if(WIN32)
    target_link_libraries(server PRIVATE crypt32)
endif()

configure_file(${CMAKE_CURRENT_LIST_DIR}/../../certs/key.pem ${CMAKE_CURRENT_BINARY_DIR}/certs/key.pem COPYONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/../../certs/cert.pem ${CMAKE_CURRENT_BINARY_DIR}/certs/cert.pem COPYONLY)