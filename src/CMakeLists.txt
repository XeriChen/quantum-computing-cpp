add_library(quantum_lib STATIC
    qubit.cpp
    gate.cpp
    circuit.cpp
)

target_include_directories(quantum_lib
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(quantum_lib PUBLIC cxx_std_23)

add_executable(quantum_demo main.cpp)
target_link_libraries(quantum_demo PRIVATE quantum_lib)
