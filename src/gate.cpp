#include "quantum/gate.hpp"

#include <numbers>

namespace quantum {

namespace {
constexpr Complex i{0.0, 1.0};
const double SQRT2_INV = 1.0 / std::numbers::sqrt2;
} // namespace

Gate::Gate(Matrix2x2 matrix, std::string name)
    : matrix_{std::move(matrix)}, name_{std::move(name)} {}

Gate Gate::operator*(const Gate& other) const {
    // Matrix product: result = this * other
    Matrix2x2 result{};
    for (std::size_t row = 0; row < 2; ++row) {
        for (std::size_t col = 0; col < 2; ++col) {
            result[row][col] = Complex{0.0, 0.0};
            for (std::size_t k = 0; k < 2; ++k) {
                result[row][col] += matrix_[row][k] * other.matrix_[k][col];
            }
        }
    }
    return Gate{result, name_ + "*" + other.name_};
}

Gate Gate::identity() {
    return Gate{{{
        {Complex{1, 0}, Complex{0, 0}},
        {Complex{0, 0}, Complex{1, 0}}
    }}, "I"};
}

Gate Gate::pauli_x() {
    return Gate{{{
        {Complex{0, 0}, Complex{1, 0}},
        {Complex{1, 0}, Complex{0, 0}}
    }}, "X"};
}

Gate Gate::pauli_y() {
    return Gate{{{
        {Complex{0, 0}, -i},
        {i,             Complex{0, 0}}
    }}, "Y"};
}

Gate Gate::pauli_z() {
    return Gate{{{
        {Complex{1, 0},  Complex{0, 0}},
        {Complex{0, 0}, Complex{-1, 0}}
    }}, "Z"};
}

Gate Gate::hadamard() {
    return Gate{{{
        {Complex{SQRT2_INV, 0},  Complex{SQRT2_INV, 0}},
        {Complex{SQRT2_INV, 0}, Complex{-SQRT2_INV, 0}}
    }}, "H"};
}

Gate Gate::phase_s() {
    return Gate{{{
        {Complex{1, 0}, Complex{0, 0}},
        {Complex{0, 0}, i}
    }}, "S"};
}

Gate Gate::phase_t() {
    const Complex t_phase{std::numbers::sqrt2 / 2.0, std::numbers::sqrt2 / 2.0};
    return Gate{{{
        {Complex{1, 0}, Complex{0, 0}},
        {Complex{0, 0}, t_phase}
    }}, "T"};
}

} // namespace quantum
