target_include_directories(server PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_sources(server
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/PhysicsComponents.h
        ${CMAKE_CURRENT_LIST_DIR}/EntityComponents.h
        ${CMAKE_CURRENT_LIST_DIR}/ShipComponents.h
        ${CMAKE_CURRENT_LIST_DIR}/PerceptionComponents.h
        ${CMAKE_CURRENT_LIST_DIR}/Session.h
        ${CMAKE_CURRENT_LIST_DIR}/Session.cpp)
