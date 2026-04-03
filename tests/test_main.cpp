#include "quantum/circuit.hpp"
#include "quantum/gate.hpp"
#include "quantum/qubit.hpp"

#include <cassert>
#include <cmath>
#include <format>
#include <iostream>
#include <numbers>
#include <string>

// ────────────────────────────────────────────────────────────────────────────
// Minimal test harness (no external framework required)
// ────────────────────────────────────────────────────────────────────────────

static int g_pass = 0;
static int g_fail = 0;

static void check(bool cond, std::string_view description) {
    if (cond) {
        std::cout << std::format("  [PASS] {}\n", description);
        ++g_pass;
    } else {
        std::cout << std::format("  [FAIL] {}\n", description);
        ++g_fail;
    }
}

static bool near(double a, double b, double tol = 1e-9) {
    return std::abs(a - b) < tol;
}

static bool near_complex(quantum::Complex a, quantum::Complex b, double tol = 1e-9) {
    return near(a.real(), b.real(), tol) && near(a.imag(), b.imag(), tol);
}

// ────────────────────────────────────────────────────────────────────────────
// Qubit tests
// ────────────────────────────────────────────────────────────────────────────
static void test_qubit() {
    std::cout << "\n[Qubit]\n";

    {
        quantum::Qubit q;
        check(near(q.prob_zero(), 1.0), "|0⟩ state: P(0) == 1");
        check(near(q.prob_one(),  0.0), "|0⟩ state: P(1) == 0");
    }

    {
        const double inv_sqrt2 = 1.0 / std::numbers::sqrt2;
        quantum::Qubit q{quantum::Complex{inv_sqrt2, 0}, quantum::Complex{inv_sqrt2, 0}};
        check(near(q.prob_zero(), 0.5, 1e-9), "Equal superposition: P(0) == 0.5");
        check(near(q.prob_one(),  0.5, 1e-9), "Equal superposition: P(1) == 0.5");
    }

    {
        // Test normalisation: constructor should auto-normalise.
        quantum::Qubit q{quantum::Complex{3.0, 0}, quantum::Complex{4.0, 0}};
        check(near(q.prob_zero(), 0.36, 1e-9), "Unnormalised input: P(0) == 0.36");
        check(near(q.prob_one(),  0.64, 1e-9), "Unnormalised input: P(1) == 0.64");
    }
}

// ────────────────────────────────────────────────────────────────────────────
// Gate tests
// ────────────────────────────────────────────────────────────────────────────
static void test_gates() {
    std::cout << "\n[Gate]\n";

    // Pauli-X should be its own inverse: X² = I
    {
        auto xx = quantum::Gate::pauli_x() * quantum::Gate::pauli_x();
        auto id = quantum::Gate::identity();
        bool ok = true;
        for (std::size_t r = 0; r < 2; ++r)
            for (std::size_t c = 0; c < 2; ++c)
                ok &= near_complex(xx.matrix()[r][c], id.matrix()[r][c]);
        check(ok, "X² = I");
    }

    // Hadamard is self-inverse: H² = I
    {
        auto hh = quantum::Gate::hadamard() * quantum::Gate::hadamard();
        auto id = quantum::Gate::identity();
        bool ok = true;
        for (std::size_t r = 0; r < 2; ++r)
            for (std::size_t c = 0; c < 2; ++c)
                ok &= near_complex(hh.matrix()[r][c], id.matrix()[r][c], 1e-9);
        check(ok, "H² = I");
    }

    // Pauli-Z squares to I
    {
        auto zz = quantum::Gate::pauli_z() * quantum::Gate::pauli_z();
        auto id = quantum::Gate::identity();
        bool ok = true;
        for (std::size_t r = 0; r < 2; ++r)
            for (std::size_t c = 0; c < 2; ++c)
                ok &= near_complex(zz.matrix()[r][c], id.matrix()[r][c]);
        check(ok, "Z² = I");
    }
}

// ────────────────────────────────────────────────────────────────────────────
// QuantumRegister tests
// ────────────────────────────────────────────────────────────────────────────
static void test_register() {
    std::cout << "\n[QuantumRegister]\n";

    // Initial state is |0⟩
    {
        quantum::QuantumRegister reg{1};
        const auto& sv = reg.state_vector();
        check(near_complex(sv[0], {1.0, 0.0}), "1-qubit register initial |0⟩: sv[0]=1");
        check(near_complex(sv[1], {0.0, 0.0}), "1-qubit register initial |0⟩: sv[1]=0");
    }

    // H on |0⟩ → equal superposition
    {
        quantum::QuantumRegister reg{1};
        reg.apply(quantum::Gate::hadamard(), 0);
        const auto& sv = reg.state_vector();
        const double inv_sqrt2 = 1.0 / std::numbers::sqrt2;
        check(near_complex(sv[0], {inv_sqrt2, 0}, 1e-9), "H|0⟩: sv[0] = 1/√2");
        check(near_complex(sv[1], {inv_sqrt2, 0}, 1e-9), "H|0⟩: sv[1] = 1/√2");
    }

    // X|0⟩ → |1⟩
    {
        quantum::QuantumRegister reg{1};
        reg.apply(quantum::Gate::pauli_x(), 0);
        const auto& sv = reg.state_vector();
        check(near_complex(sv[0], {0.0, 0.0}), "X|0⟩: sv[0]=0");
        check(near_complex(sv[1], {1.0, 0.0}), "X|0⟩: sv[1]=1");
    }

    // Bell state: H on q0 then CNOT(0,1) → (|00⟩+|11⟩)/√2
    {
        quantum::QuantumRegister reg{2};
        reg.apply(quantum::Gate::hadamard(), 0);
        reg.apply_cnot(0, 1);
        const auto& sv = reg.state_vector();
        const double inv_sqrt2 = 1.0 / std::numbers::sqrt2;
        check(near_complex(sv[0], {inv_sqrt2, 0}, 1e-9), "Bell: sv[|00⟩] = 1/√2");
        check(near_complex(sv[1], {0.0,       0}, 1e-9), "Bell: sv[|01⟩] = 0");
        check(near_complex(sv[2], {0.0,       0}, 1e-9), "Bell: sv[|10⟩] = 0");
        check(near_complex(sv[3], {inv_sqrt2, 0}, 1e-9), "Bell: sv[|11⟩] = 1/√2");
    }
}

// ────────────────────────────────────────────────────────────────────────────
// QuantumCircuit tests
// ────────────────────────────────────────────────────────────────────────────
static void test_circuit() {
    std::cout << "\n[QuantumCircuit]\n";

    // Circuit API mirrors QuantumRegister behaviour.
    {
        auto reg = quantum::QuantumCircuit{1}.x(0).run();
        const auto& sv = reg.state_vector();
        check(near_complex(sv[1], {1.0, 0.0}), "Circuit X|0⟩ → |1⟩");
    }

    // H + H = identity
    {
        auto reg = quantum::QuantumCircuit{1}.h(0).h(0).run();
        const auto& sv = reg.state_vector();
        check(near_complex(sv[0], {1.0, 0.0}, 1e-9), "Circuit H→H → |0⟩");
    }

    // Bell state via circuit builder
    {
        auto reg = quantum::QuantumCircuit{2}.h(0).cnot(0, 1).run();
        const auto& sv = reg.state_vector();
        const double inv_sqrt2 = 1.0 / std::numbers::sqrt2;
        check(near_complex(sv[0], {inv_sqrt2, 0}, 1e-9), "Circuit Bell: sv[|00⟩]=1/√2");
        check(near_complex(sv[3], {inv_sqrt2, 0}, 1e-9), "Circuit Bell: sv[|11⟩]=1/√2");
    }
}

// ────────────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "Running quantum-computing-cpp unit tests\n";
    std::cout << "=========================================\n";

    test_qubit();
    test_gates();
    test_register();
    test_circuit();

    std::cout << std::format("\n=========================================\n"
                             "Results: {} passed, {} failed\n",
                             g_pass, g_fail);

    return (g_fail == 0) ? 0 : 1;
}
