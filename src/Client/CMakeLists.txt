project(client)

add_executable(${PROJECT_NAME} main.cpp)
add_executable(spac::client ALIAS ${PROJECT_NAME})

add_subdirectory(ECS)
add_subdirectory(Scenes)
add_subdirectory(Networking)

find_package(EnTT CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(raylib CONFIG REQUIRED)
find_package(Boost 1.73 REQUIRED COMPONENTS system thread log REQUIRED)
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
        spac::networking
        spac::ecs
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