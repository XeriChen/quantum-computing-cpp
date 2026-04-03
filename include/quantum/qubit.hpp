#pragma once

#include "types.hpp"

#include <array>
#include <stdexcept>

namespace quantum {

/// A single qubit in state  α|0⟩ + β|1⟩.
///
/// The state is normalised at construction time, i.e. |α|² + |β|² == 1.
class Qubit {
public:
    /// Construct the |0⟩ computational basis state.
    Qubit();

    /// Construct a qubit with explicit amplitudes.
    /// Throws std::invalid_argument if the state is not normalisable.
    Qubit(Complex alpha, Complex beta);

    /// Amplitude of the |0⟩ basis state.
    [[nodiscard]] Complex alpha() const noexcept { return state_[0]; }

    /// Amplitude of the |1⟩ basis state.
    [[nodiscard]] Complex beta() const noexcept { return state_[1]; }

    /// Probability of measuring |0⟩.
    [[nodiscard]] double prob_zero() const noexcept;

    /// Probability of measuring |1⟩.
    [[nodiscard]] double prob_one() const noexcept;

    /// Raw state vector [α, β].
    [[nodiscard]] const std::array<Complex, 2>& state() const noexcept { return state_; }

private:
    std::array<Complex, 2> state_;

    void normalise();
};

} // namespace quantum
