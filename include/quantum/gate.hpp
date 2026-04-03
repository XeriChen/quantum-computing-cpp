#pragma once

#include "types.hpp"

#include <array>
#include <string>
#include <string_view>

namespace quantum {

/// 2×2 complex matrix representing a single-qubit quantum gate.
using Matrix2x2 = std::array<std::array<Complex, 2>, 2>;

/// A single-qubit unitary gate.
class Gate {
public:
    Gate(Matrix2x2 matrix, std::string name);

    [[nodiscard]] const Matrix2x2&   matrix() const noexcept { return matrix_; }
    [[nodiscard]] std::string_view   name()   const noexcept { return name_;   }

    /// Compose two gates: (this * other) applied right-to-left (other first).
    [[nodiscard]] Gate operator*(const Gate& other) const;

    // ── Factory methods for standard gates ───────────────────────────────
    [[nodiscard]] static Gate identity();
    [[nodiscard]] static Gate pauli_x();     ///< Bit-flip  (NOT)
    [[nodiscard]] static Gate pauli_y();     ///< Y gate
    [[nodiscard]] static Gate pauli_z();     ///< Phase-flip
    [[nodiscard]] static Gate hadamard();    ///< H gate  (superposition)
    [[nodiscard]] static Gate phase_s();     ///< S gate  (π/2 phase)
    [[nodiscard]] static Gate phase_t();     ///< T gate  (π/4 phase)

private:
    Matrix2x2   matrix_;
    std::string name_;
};

} // namespace quantum
