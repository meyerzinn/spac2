target_include_directories(spacserver PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_sources(spacserver
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/PhysicsComponents.h
            ${CMAKE_CURRENT_LIST_DIR}/EntityComponents.h
            ${CMAKE_CURRENT_LIST_DIR}/ShipComponents.h
            ${CMAKE_CURRENT_LIST_DIR}/NetworkingComponents.h)
