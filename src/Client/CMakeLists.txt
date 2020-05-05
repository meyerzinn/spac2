add_executable(client main.cpp)

add_subdirectory(ECS)
add_subdirectory(Scenes)
add_subdirectory(Networking)

find_package(EnTT CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(raylib CONFIG REQUIRED)
find_package(Boost 1.72 REQUIRED COMPONENTS system thread log REQUIRED)
find_package(OpenSSL REQUIRED)

target_compile_options(client
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
        $<$<CXX_COMPILER_ID:Clang>:-fsanitize=address -fno-omit-frame-pointer>)

target_include_directories(client PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${raylib_INCLUDE_DIRS}
        ${CMAKE_CURRENT_LIST_DIR})

target_sources(client PRIVATE ${CMAKE_CURRENT_LIST_DIR}/GameLoop.h ${CMAKE_CURRENT_LIST_DIR}/GameLoop.cpp)

target_link_libraries(client PRIVATE
        common
        OpenSSL::SSL
        OpenSSL::Crypto
        ${raylib_LIBRARIES}
        EnTT::EnTT
        ${Boost_LIBRARIES}
        ${Boost_SYSTEM_LIBRARY}
        glfw)

if(WIN32)
    target_link_libraries(client PRIVATE crypt32)
endif()