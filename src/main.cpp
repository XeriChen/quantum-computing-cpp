#include "quantum/circuit.hpp"

#include <format>
#include <iostream>
#include <vector>

// ────────────────────────────────────────────────────────────────────────────
// Helper: run a named example and print its statevector.
// ────────────────────────────────────────────────────────────────────────────
static void run_example(std::string_view title, quantum::QuantumCircuit circuit) {
    std::cout << "\n══════════════════════════════════════════\n";
    std::cout << std::format("  {}\n", title);
    std::cout << "══════════════════════════════════════════\n";
    circuit.print();
    std::cout << '\n';
    auto reg = circuit.run();
    reg.print_state();
}

// ────────────────────────────────────────────────────────────────────────────
// Example 1 – Single qubit in superposition
//   H|0⟩ = (|0⟩ + |1⟩) / √2
// ────────────────────────────────────────────────────────────────────────────
static void example_superposition() {
    run_example("Superposition: H|0⟩",
                quantum::QuantumCircuit{1}.h(0));
}

// ────────────────────────────────────────────────────────────────────────────
// Example 2 – Bell state (maximally entangled two-qubit state)
//   (H ⊗ I) · CNOT |00⟩ = (|00⟩ + |11⟩) / √2
// ────────────────────────────────────────────────────────────────────────────
static void example_bell_state() {
    run_example("Bell state: (|00⟩ + |11⟩) / √2",
                quantum::QuantumCircuit{2}.h(0).cnot(0, 1));
}

// ────────────────────────────────────────────────────────────────────────────
// Example 3 – GHZ state (three-qubit entanglement)
//   (|000⟩ + |111⟩) / √2
// ────────────────────────────────────────────────────────────────────────────
static void example_ghz_state() {
    run_example("GHZ state: (|000⟩ + |111⟩) / √2",
                quantum::QuantumCircuit{3}.h(0).cnot(0, 1).cnot(0, 2));
}

// ────────────────────────────────────────────────────────────────────────────
// Example 4 – Quantum teleportation circuit (circuit only, no classical ops)
//   Prepares |ψ⟩ = X|0⟩ on qubit 0, creates Bell pair on (1,2),
//   then performs the Bell measurement half of the protocol.
// ────────────────────────────────────────────────────────────────────────────
static void example_teleportation_prep() {
    run_example("Teleportation prep: X on q0, Bell pair on (q1, q2)",
                quantum::QuantumCircuit{3}
                    .x(0)           // prepare |ψ⟩ = |1⟩ on qubit 0
                    .h(1)           // entangle qubit 1 and 2
                    .cnot(1, 2)
                    .cnot(0, 1)     // Bell measurement (first half)
                    .h(0));
}

// ────────────────────────────────────────────────────────────────────────────
// Example 5 – Phase kickback: Hadamard + T + Hadamard
// ────────────────────────────────────────────────────────────────────────────
static void example_phase_kickback() {
    run_example("Phase kickback: H → T → H",
                quantum::QuantumCircuit{1}.h(0).t(0).h(0));
}

// ────────────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "╔══════════════════════════════════════════╗\n"
                 "║  Quantum Computing C++23 Simulator       ║\n"
                 "╚══════════════════════════════════════════╝\n";

    example_superposition();
    example_bell_state();
    example_ghz_state();
    example_teleportation_prep();
    example_phase_kickback();

    std::cout << "\nDone.\n";
    return 0;
}
