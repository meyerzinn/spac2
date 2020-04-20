project(spacserver)

add_executable(${PROJECT_NAME} main.cpp)
add_executable(spac::server ALIAS ${PROJECT_NAME})

set_property(TARGET spacserver PROPERTY CXX_STANDARD 17)

target_include_directories(spacserver PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Utilities)

add_subdirectory(ECS)

find_package(EnTT CONFIG REQUIRED)
find_package(unofficial-box2d CONFIG REQUIRED)
find_package(Flatbuffers CONFIG REQUIRED)

# uwebsockets
find_path(USOCKETS_INCLUDE_DIR libusockets.h)
find_path(UWEBSOCKETS_INCLUDE_DIR uwebsockets/App.h)

target_include_directories(spacserver PRIVATE
        ${USOCKETS_INCLUDE_DIR}
        ${UWEBSOCKETS_INCLUDE_DIR})

target_link_libraries(spacserver PRIVATE
        spac::common
        EnTT::EnTT
        unofficial::box2d::box2d
        flatbuffers::flatbuffers)