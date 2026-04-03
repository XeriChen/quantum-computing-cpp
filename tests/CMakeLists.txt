add_executable(quantum_tests test_main.cpp)

target_link_libraries(quantum_tests PRIVATE quantum_lib)

add_test(NAME quantum_unit_tests COMMAND quantum_tests)
