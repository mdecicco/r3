project(db_pg)

file(GLOB_RECURSE 5_db_pg_src "*.h" "*.cpp")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_executable(5_db_pg ${5_db_pg_src})
set_property(TARGET 5_db_pg PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
 
SOURCE_GROUP("" FILES ${5_db_pg_src})

target_include_directories(5_db_pg PUBLIC ../../include)
target_link_libraries(5_db_pg r3)
set_target_properties(5_db_pg PROPERTIES FOLDER playgrounds)
