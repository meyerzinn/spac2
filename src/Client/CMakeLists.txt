#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
project(spacclient)

add_executable(spacclient main.cpp)

target_compile_definitions(spacclient PRIVATE SUPPORT_HIGH_DPI=1)

add_subdirectory(ECS)
add_subdirectory(Scenes)

find_package(EnTT CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(raylib CONFIG REQUIRED)
find_package(websocketpp CONFIG REQUIRED)
find_package(Boost 1.72 REQUIRED)

target_compile_options(spacclient PRIVATE -fsanitize=address -fno-omit-frame-pointer)
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined)

#target_link_options(spacclient PRIVATE -fsanitize=address -fno-omit-frame-pointer)

target_include_directories(spacclient PRIVATE
        ${raylib_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS})
target_link_libraries(spacclient PRIVATE
        spac::common
        raylib
        EnTT::EnTT
        glfw
        websocketpp::websocketpp
        ${Boost_LIBRARIES}
        -fsanitize=address)