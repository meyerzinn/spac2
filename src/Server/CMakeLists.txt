project(server)

add_executable(${PROJECT_NAME} main.cpp)
add_executable(spac::server ALIAS ${PROJECT_NAME})

target_include_directories(server PRIVATE ${PROJECT_SOURCE_DIR})

target_sources(server PRIVATE
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/Networking/WebsocketSession.cpp


    # TODO: Move these all into the headers
    ${PROJECT_SOURCE_DIR}/ECS/Systems/BoosterSystem.cpp
    ${PROJECT_SOURCE_DIR}/ECS/Systems/CollisionsSystem.cpp
    ${PROJECT_SOURCE_DIR}/ECS/Systems/DestructionSystem.cpp
    ${PROJECT_SOURCE_DIR}/ECS/Systems/LifetimeSystem.cpp
    ${PROJECT_SOURCE_DIR}/ECS/Systems/NetworkingSystem.cpp
    ${PROJECT_SOURCE_DIR}/ECS/Systems/PerceptionSystem.cpp
    ${PROJECT_SOURCE_DIR}/ECS/Systems/RefuelSystem.cpp
    ${PROJECT_SOURCE_DIR}/ECS/Systems/ShieldSystem.cpp
)

find_package(EnTT CONFIG REQUIRED)
find_package(unofficial-box2d CONFIG REQUIRED)
find_package(Flatbuffers CONFIG REQUIRED)
find_package(Boost 1.72 REQUIRED COMPONENTS log)
find_package(OpenSSL REQUIRED)

target_include_directories(server PRIVATE
        ${Boost_INCLUDE_DIR})

target_link_libraries(server PRIVATE
        spac::networking
        spac::ecs
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