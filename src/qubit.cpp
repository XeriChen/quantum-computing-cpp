#include "quantum/qubit.hpp"

#include <cmath>
#include <stdexcept>

namespace quantum {

Qubit::Qubit() : state_{Complex{1.0, 0.0}, Complex{0.0, 0.0}} {}

Qubit::Qubit(Complex alpha, Complex beta) : state_{alpha, beta} {
    normalise();
}

void Qubit::normalise() {
    const double norm = std::sqrt(std::norm(state_[0]) + std::norm(state_[1]));
    if (norm < 1e-12) {
        throw std::invalid_argument("Qubit state has zero norm and cannot be normalised.");
    }
    state_[0] /= norm;
    state_[1] /= norm;
}

double Qubit::prob_zero() const noexcept {
    return std::norm(state_[0]);
}

double Qubit::prob_one() const noexcept {
    return std::norm(state_[1]);
}

} // namespace quantum
