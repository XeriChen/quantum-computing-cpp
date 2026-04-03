#include "quantum/circuit.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <iostream>
#include <random>
#include <ranges>
#include <stdexcept>

namespace quantum {

// ── QuantumRegister ──────────────────────────────────────────────────────────

QuantumRegister::QuantumRegister(std::size_t n_qubits)
    : n_qubits_{n_qubits}, state_(std::size_t{1} << n_qubits, Complex{0.0, 0.0}) {
    if (n_qubits == 0) {
        throw std::invalid_argument("A quantum register must have at least one qubit.");
    }
    state_[0] = Complex{1.0, 0.0}; // initialise to |0…0⟩
}

void QuantumRegister::apply(const Gate& gate, std::size_t qubit_index) {
    if (qubit_index >= n_qubits_) {
        throw std::out_of_range(
            std::format("Qubit index {} is out of range for a {}-qubit register.",
                        qubit_index, n_qubits_));
    }

    const auto& m = gate.matrix();
    const std::size_t dim = state_.size();
    std::vector<Complex> new_state(dim, Complex{0.0, 0.0});

    for (std::size_t idx = 0; idx < dim; ++idx) {
        // Determine the bit at qubit_index for this basis state.
        const std::size_t bit = (idx >> qubit_index) & 1ULL;

        // The partner index has that bit flipped.
        const std::size_t partner = idx ^ (1ULL << qubit_index);

        const Complex amp0 = (bit == 0) ? state_[idx] : state_[partner];
        const Complex amp1 = (bit == 0) ? state_[partner] : state_[idx];

        // Row of the 2×2 matrix that corresponds to the output bit.
        const Complex contribution = m[bit][0] * amp0 + m[bit][1] * amp1;
        new_state[idx] += contribution;
    }

    state_ = std::move(new_state);
}

void QuantumRegister::apply_cnot(std::size_t control, std::size_t target) {
    if (control >= n_qubits_ || target >= n_qubits_) {
        throw std::out_of_range("CNOT qubit index out of range.");
    }
    if (control == target) {
        throw std::invalid_argument("CNOT control and target must differ.");
    }

    const std::size_t dim = state_.size();
    std::vector<Complex> new_state(dim, Complex{0.0, 0.0});

    for (std::size_t idx = 0; idx < dim; ++idx) {
        const std::size_t ctrl_bit   = (idx >> control) & 1ULL;
        const std::size_t target_bit = (idx >> target)  & 1ULL;

        if (ctrl_bit == 1) {
            // Flip target qubit.
            const std::size_t new_idx = idx ^ (1ULL << target);
            new_state[new_idx] += state_[idx];
        } else {
            new_state[idx] += state_[idx];
        }
        (void)target_bit; // used implicitly via XOR above
    }

    state_ = std::move(new_state);
}

std::vector<bool> QuantumRegister::measure_all() {
    // Build cumulative probability distribution.
    std::vector<double> cumulative;
    cumulative.reserve(state_.size());
    double sum = 0.0;
    for (const auto& amp : state_) {
        sum += std::norm(amp);
        cumulative.push_back(sum);
    }

    // Sample a random outcome.
    thread_local std::mt19937_64 rng{std::random_device{}()};
    std::uniform_real_distribution<double> dist{0.0, sum};
    const double sample = dist(rng);

    const auto it = std::ranges::lower_bound(cumulative, sample);
    const std::size_t outcome = static_cast<std::size_t>(
        std::distance(cumulative.begin(), it));

    // Collapse the statevector.
    std::fill(state_.begin(), state_.end(), Complex{0.0, 0.0});
    state_[outcome] = Complex{1.0, 0.0};

    // Decode outcome into individual qubit bits (LSB = qubit 0).
    std::vector<bool> bits(n_qubits_);
    for (std::size_t q = 0; q < n_qubits_; ++q) {
        bits[q] = static_cast<bool>((outcome >> q) & 1ULL);
    }
    return bits;
}

void QuantumRegister::print_state() const {
    std::cout << std::format("State of {}-qubit register:\n", n_qubits_);
    const std::size_t dim = state_.size();

    for (std::size_t idx = 0; idx < dim; ++idx) {
        const double mag_sq = std::norm(state_[idx]);
        if (mag_sq < 1e-12) continue; // skip zero-amplitude terms

        // Format basis state label as binary string (MSB on the left).
        std::string label(n_qubits_, '0');
        for (std::size_t q = 0; q < n_qubits_; ++q) {
            if ((idx >> q) & 1ULL) {
                label[n_qubits_ - 1 - q] = '1';
            }
        }

        const double re = state_[idx].real();
        const double im = state_[idx].imag();
        std::cout << std::format("  ({:+.4f}{:+.4f}i) |{}⟩  [p = {:.4f}]\n",
                                 re, im, label, mag_sq);
    }
}

// ── QuantumCircuit ────────────────────────────────────────────────────────────

QuantumCircuit::QuantumCircuit(std::size_t n_qubits) : n_qubits_{n_qubits} {
    if (n_qubits == 0) {
        throw std::invalid_argument("A quantum circuit must have at least one qubit.");
    }
}

QuantumCircuit& QuantumCircuit::h(std::size_t qubit) {
    ops_.emplace_back(GateOp{Gate::hadamard(), qubit});
    return *this;
}

QuantumCircuit& QuantumCircuit::x(std::size_t qubit) {
    ops_.emplace_back(GateOp{Gate::pauli_x(), qubit});
    return *this;
}

QuantumCircuit& QuantumCircuit::y(std::size_t qubit) {
    ops_.emplace_back(GateOp{Gate::pauli_y(), qubit});
    return *this;
}

QuantumCircuit& QuantumCircuit::z(std::size_t qubit) {
    ops_.emplace_back(GateOp{Gate::pauli_z(), qubit});
    return *this;
}

QuantumCircuit& QuantumCircuit::s(std::size_t qubit) {
    ops_.emplace_back(GateOp{Gate::phase_s(), qubit});
    return *this;
}

QuantumCircuit& QuantumCircuit::t(std::size_t qubit) {
    ops_.emplace_back(GateOp{Gate::phase_t(), qubit});
    return *this;
}

QuantumCircuit& QuantumCircuit::cnot(std::size_t control, std::size_t target) {
    ops_.emplace_back(CnotOp{control, target});
    return *this;
}

QuantumRegister QuantumCircuit::run() const {
    QuantumRegister reg{n_qubits_};
    for (const auto& op : ops_) {
        std::visit([&reg](const auto& o) {
            using T = std::decay_t<decltype(o)>;
            if constexpr (std::is_same_v<T, GateOp>) {
                reg.apply(o.gate, o.qubit);
            } else if constexpr (std::is_same_v<T, CnotOp>) {
                reg.apply_cnot(o.control, o.target);
            }
        }, op);
    }
    return reg;
}

void QuantumCircuit::print() const {
    std::cout << std::format("QuantumCircuit ({} qubits, {} operations):\n",
                             n_qubits_, ops_.size());
    for (std::size_t i = 0; i < ops_.size(); ++i) {
        if (const auto* g = std::get_if<GateOp>(&ops_[i])) {
            std::cout << std::format("  [{:2d}] {} on qubit {}\n",
                                     i, g->gate.name(), g->qubit);
        } else if (const auto* c = std::get_if<CnotOp>(&ops_[i])) {
            std::cout << std::format("  [{:2d}] CNOT  control={} target={}\n",
                                     i, c->control, c->target);
        }
    }
}

} // namespace quantum
