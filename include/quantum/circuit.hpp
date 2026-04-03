#pragma once

#include "gate.hpp"
#include "types.hpp"

#include <cstddef>
#include <string>
#include <variant>
#include <vector>

namespace quantum {

// ── QuantumRegister ─────────────────────────────────────────────────────────

/// Statevector simulation of an n-qubit register.
///
/// The 2ⁿ amplitudes are stored in computational-basis order:
///   index k  ↔  |bₙ₋₁ … b₁ b₀⟩  where k = bₙ₋₁·2ⁿ⁻¹ + … + b₀
class QuantumRegister {
public:
    explicit QuantumRegister(std::size_t n_qubits);

    [[nodiscard]] std::size_t size()  const noexcept { return n_qubits_; }

    /// Full statevector (length 2ⁿ).
    [[nodiscard]] const std::vector<Complex>& state_vector() const noexcept { return state_; }

    /// Apply a single-qubit gate to qubit at position @p qubit_index.
    void apply(const Gate& gate, std::size_t qubit_index);

    /// Apply a two-qubit CNOT gate.
    void apply_cnot(std::size_t control, std::size_t target);

    /// Measure all qubits and return classical bits (true == |1⟩).
    /// The statevector collapses to the measured outcome.
    [[nodiscard]] std::vector<bool> measure_all();

    /// Print the statevector in Dirac notation.
    void print_state() const;

private:
    std::size_t         n_qubits_;
    std::vector<Complex> state_;
};

// ── QuantumCircuit ───────────────────────────────────────────────────────────

/// Builder for a sequence of quantum gate operations.
///
/// Example – Bell state preparation:
/// @code
///   auto result = QuantumCircuit{2}
///                     .h(0)
///                     .cnot(0, 1)
///                     .run();
///   result.print_state();
/// @endcode
class QuantumCircuit {
public:
    explicit QuantumCircuit(std::size_t n_qubits);

    // ── Gate application (returns *this for chaining) ─────────────────────
    QuantumCircuit& h   (std::size_t qubit);                          ///< Hadamard
    QuantumCircuit& x   (std::size_t qubit);                          ///< Pauli-X
    QuantumCircuit& y   (std::size_t qubit);                          ///< Pauli-Y
    QuantumCircuit& z   (std::size_t qubit);                          ///< Pauli-Z
    QuantumCircuit& s   (std::size_t qubit);                          ///< Phase S
    QuantumCircuit& t   (std::size_t qubit);                          ///< Phase T
    QuantumCircuit& cnot(std::size_t control, std::size_t target);    ///< CNOT

    /// Execute the circuit on the |0…0⟩ initial state.
    [[nodiscard]] QuantumRegister run() const;

    /// Print a human-readable summary of the circuit.
    void print() const;

private:
    struct GateOp  { Gate gate; std::size_t qubit; };
    struct CnotOp  { std::size_t control; std::size_t target; };
    using  Op = std::variant<GateOp, CnotOp>;

    std::size_t   n_qubits_;
    std::vector<Op> ops_;
};

} // namespace quantum
