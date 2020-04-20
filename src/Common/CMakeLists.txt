project (spaccommon)

add_library(${PROJECT_NAME} INTERFACE)
add_library(spac::common ALIAS ${PROJECT_NAME})

add_subdirectory(ECS)
add_subdirectory(Net)