project(ecs_pg)

file(GLOB_RECURSE 6_ecs_pg_src "*.h" "*.cpp")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_executable(6_ecs_pg ${6_ecs_pg_src})
set_property(TARGET 6_ecs_pg PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
 
SOURCE_GROUP("" FILES ${6_ecs_pg_src})

target_include_directories(6_ecs_pg PUBLIC ../../include)
target_link_libraries(6_ecs_pg r3)
set_target_properties(6_ecs_pg PROPERTIES FOLDER playgrounds)
