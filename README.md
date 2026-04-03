# quantum-computing-cpp

A C++23-compliant quantum computing simulation library for personal study.

## Features

- Single-qubit and multi-qubit statevector simulation
- Standard quantum gates: I, X, Y, Z, H (Hadamard), S, T, CNOT
- Fluent `QuantumCircuit` builder with method chaining
- Example circuits: superposition, Bell state, GHZ state, teleportation, phase kickback
- Cross-platform CMake build (Linux, macOS, Windows)

## Requirements

| Tool    | Minimum version |
|---------|----------------|
| CMake   | 3.25           |
| GCC     | 13 or later    |
| Clang   | 16 or later    |
| MSVC    | 19.35 (VS 2022 17.5) or later |

## Building

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run the demo
./build/src/quantum_demo

# Run unit tests
cmake --build build --target quantum_tests
ctest --test-dir build --output-on-failure
```

## Project Structure

```
quantum-computing-cpp/
├── CMakeLists.txt          # Root CMake (C++23, cross-platform)
├── include/
│   └── quantum/
│       ├── types.hpp       # Common type aliases (Complex)
│       ├── qubit.hpp       # Single-qubit state representation
│       ├── gate.hpp        # Quantum gate (2×2 unitary matrix)
│       └── circuit.hpp     # QuantumRegister & QuantumCircuit
├── src/
│   ├── CMakeLists.txt
│   ├── qubit.cpp
│   ├── gate.cpp
│   ├── circuit.cpp
│   └── main.cpp            # Demo entry point
└── tests/
    ├── CMakeLists.txt
    └── test_main.cpp       # Unit tests (no external framework)
```

## Quick Start

```cpp
#include "quantum/circuit.hpp"
#include <iostream>

int main() {
    // Create Bell state: (|00⟩ + |11⟩) / √2
    auto reg = quantum::QuantumCircuit{2}
                   .h(0)       // Hadamard on qubit 0
                   .cnot(0, 1) // CNOT: control=0, target=1
                   .run();

    reg.print_state();
    // Output:
    //   (+0.7071+0.0000i) |00⟩  [p = 0.5000]
    //   (+0.7071+0.0000i) |11⟩  [p = 0.5000]
}
```

