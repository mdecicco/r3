project(unit_tests)
add_subdirectory(deps/catch2)

file(GLOB test_sources "./*.cpp")
add_executable(tests ${test_sources})
set_property(TARGET tests PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
 
target_include_directories(tests PUBLIC ../include)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain r3)