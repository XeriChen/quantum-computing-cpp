#include <cmath>
#include <complex>
#include <print>
#include <random>
#include <vector>

#include "types.hpp"
using std::println;
using std::scanf;
using std::vector;

using quantum::Complex;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);
void qbit_normalize(vector<Complex>& qbit) {
  // forall qubit, let the first index that not eq 0, change it to real number.
  // the let normalization of the qubit.'
  double sum = 0.0;
  Complex conjugate_first = 0.0;  // the first index that not eq 0.
  for (auto& x : qbit) {
    if (std::norm(x) > 1e-9) {
      conjugate_first = std::conj(x);
      break;
    }
  }
  for (auto& x : qbit) {
    x *= conjugate_first;
    sum += std::norm(x);
  }
  if (sum < 1e-9) {
    qbit[0] = 1.0;
  } else {
    for (auto& x : qbit) {
      x /= std::sqrt(sum);
    }
  }
}
void qbit_init(vector<Complex>& qbit) {
  for (auto& x : qbit) {
    x = Complex(dis(gen), dis(gen));
  }
  qbit_normalize(qbit);
}
// #define QBITS_DEBUG
bool qbit_measure(vector<Complex>& qbit, int it) {
  const int N = static_cast<int>(qbit.size());
  const int half = 1 << it;
  const int stride = 1 << (it + 1);

  double prob0 = 0.0;
  for (int base = 0; base < N; base += stride) {
    for (int offset = 0; offset < half; ++offset) {
      prob0 += std::norm(qbit[base + offset]);
#ifdef QBITS_DEBUG
      println("i+j = {:d}", base + offset);
#endif
    }
  }

#ifdef QBITS_DEBUG
  println("sum(prob0) = {:.6f}", prob0);
#endif
  // 健壮性保证
  if (prob0 <= 0.0) {
    prob0 = 0.0;
  } else if (prob0 >= 1.0) {
    prob0 = 1.0;
  }

  const double r = dis(gen);
  const bool outcome0 = (r < prob0);
  if (outcome0) {
    const double inv = 1.0 / std::sqrt(prob0);
    for (int i = 0; i < N; i += (1 << (it + 1))) {
      for (int j = 0; j < (1 << it); j++) {
        qbit[i + j] *= inv;
      }
    }
    for (int i = (1 << it); i < N; i += (1 << (it + 1))) {
      for (int j = 0; j < (1 << it); j++) {
        qbit[i + j] = 0.0;
      }
    }
    return 0;
  } else {
    const double prob1 = 1.0 - prob0;
    const double inv = 1.0 / std::sqrt(prob1);
    for (int i = 0; i < N; i += (1 << (it + 1))) {
      for (int j = 0; j < (1 << it); j++) {
        qbit[i + j] = 0.0;
      }
    }
    for (int i = (1 << it); i < N; i += (1 << (it + 1))) {
      for (int j = 0; j < (1 << it); j++) {
        qbit[i + j] *= inv;
      }
    }
    return 1;
  }
}

int qbit_measure_all(vector<Complex>& qbit) {
  const int N = static_cast<int>(qbit.size());
  double total = 0.0;
  vector<double> cdf(N);
  for (int i = 0; i < N; ++i) {
    total += std::norm(qbit[i]);
    cdf[i] = total;
  }
  if (total <= 0.0) {
    qbit.assign(N, 0.0);
    if (N > 0) {
      qbit[0] = 1.0;
    }
    return 0;
  }

  const double r = dis(gen) * total;
  int lo = 0;
  int hi = N - 1;
  while (lo < hi) {
    const int mid = lo + (hi - lo) / 2;
    if (r <= cdf[mid]) {
      hi = mid;
    } else {
      lo = mid + 1;
    }
  }

  qbit.assign(N, 0.0);
  qbit[lo] = 1.0;
  return lo;
}

int main() {
  int n;
  // cin >> n;
  n = 3;
  int bit_num = n;
  int dim_N = 1 << bit_num;
  vector<Complex> qbit(dim_N);
  qbit_init(qbit);

  // test the normalization of the qubit.
  double sum = 0.0;
  for (auto& x : qbit) {
    sum += std::norm(x);
  }
  println("The nomalization of {:.3f}", sum);
  for (auto& x : qbit) {
    println("real is {:.3f}, imag is {:.3f}", x.real(), x.imag());
  }
  for (int it = 0; it < bit_num; it++) {
    println("{}", qbit_measure(qbit, it));
  }

  return 0;
}