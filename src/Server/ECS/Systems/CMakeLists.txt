target_include_directories(server PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_sources(server
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/BoosterSystem.h
        ${CMAKE_CURRENT_LIST_DIR}/BoosterSystem.cpp
        ${CMAKE_CURRENT_LIST_DIR}/CollisionsSystem.h
        ${CMAKE_CURRENT_LIST_DIR}/CollisionsSystem.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DestructionSystem.h
        ${CMAKE_CURRENT_LIST_DIR}/DestructionSystem.cpp
        ${CMAKE_CURRENT_LIST_DIR}/LifetimeSystem.h
        ${CMAKE_CURRENT_LIST_DIR}/LifetimeSystem.cpp
        ${CMAKE_CURRENT_LIST_DIR}/NetworkingSystem.h
        ${CMAKE_CURRENT_LIST_DIR}/NetworkingSystem.cpp
        ${CMAKE_CURRENT_LIST_DIR}/PerceptionsSystem.h
        ${CMAKE_CURRENT_LIST_DIR}/PerceptionsSystem.cpp
        ${CMAKE_CURRENT_LIST_DIR}/RefuelSystem.h
        ${CMAKE_CURRENT_LIST_DIR}/RefuelSystem.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ShieldSystem.h
        ${CMAKE_CURRENT_LIST_DIR}/ShieldSystem.cpp)
