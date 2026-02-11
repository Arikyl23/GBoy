# Add Core Unit Tests
add_executable(unit_test_string_core unit_test_core.c)
target_link_libraries(unit_test_string_core PRIVATE String_Internal_Test)
add_test(
    NAME "STRING | [UNIT] string/core"
    COMMAND unit_test_string_core
)