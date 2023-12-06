/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   bp_filter.cpp
 * @date   2023-12-05
 * @brief  See
 * https://github.com/nxsEdson/Butterworth-Filter/blob/master/butterworth.cpp
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <complex>
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>

#include "band_pass_filter.hpp"

#include <SignalEasel/signal_easel.hpp>

#define PI 3.14159

namespace signal_easel {

std::vector<double> trinomialMultiply(unsigned int filter_order,
                                      const std::vector<double> &b_coeffs,
                                      const std::vector<double> &c_coeffs) {
  std::vector<double> return_vector(static_cast<size_t>(4) * filter_order);

  return_vector[2] = c_coeffs.at(0);
  return_vector[3] = c_coeffs.at(1);
  return_vector[0] = b_coeffs.at(0);
  return_vector[1] = b_coeffs.at(1);

  for (size_t i = 1; i < filter_order; ++i) {
    return_vector[2 * (2 * i + 1)] +=
        c_coeffs[2 * i] * return_vector[2 * (2 * i - 1)] -
        c_coeffs[2 * i + 1] * return_vector[2 * (2 * i - 1) + 1];
    return_vector[2 * (2 * i + 1) + 1] +=
        c_coeffs[2 * i] * return_vector[2 * (2 * i - 1) + 1] +
        c_coeffs[2 * i + 1] * return_vector[2 * (2 * i - 1)];

    for (size_t j = 2 * i; j > 1; --j) {
      return_vector[2 * j] +=
          b_coeffs[2 * i] * return_vector[2 * (j - 1)] -
          b_coeffs[2 * i + 1] * return_vector[2 * (j - 1) + 1] +
          c_coeffs[2 * i] * return_vector[2 * (j - 2)] -
          c_coeffs[2 * i + 1] * return_vector[2 * (j - 2) + 1];
      return_vector[2 * j + 1] +=
          b_coeffs[2 * i] * return_vector[2 * (j - 1) + 1] +
          b_coeffs[2 * i + 1] * return_vector[2 * (j - 1)] +
          c_coeffs[2 * i] * return_vector[2 * (j - 2) + 1] +
          c_coeffs[2 * i + 1] * return_vector[2 * (j - 2)];
    }

    return_vector[2] += b_coeffs[2 * i] * return_vector[0] -
                        b_coeffs[2 * i + 1] * return_vector[1] +
                        c_coeffs[2 * i];
    return_vector[3] += b_coeffs[2 * i] * return_vector[1] +
                        b_coeffs[2 * i + 1] * return_vector[0] +
                        c_coeffs[2 * i + 1];
    return_vector[0] += b_coeffs[2 * i];
    return_vector[1] += b_coeffs[2 * i + 1];
  }

  return return_vector;
}

std::vector<double> computeLP(unsigned int filter_order) {
  std::vector<double> num_coeffs(filter_order + 1);

  num_coeffs[0] = 1;
  num_coeffs[1] = filter_order;
  auto half_order = filter_order / 2;
  for (size_t i = 2; i <= half_order; ++i) {
    num_coeffs[i] = (double)(filter_order - i + 1) * num_coeffs[i - 1] / i;
    num_coeffs[filter_order - i] = num_coeffs[i];
  }
  num_coeffs[filter_order - 1] = filter_order;
  num_coeffs[filter_order] = 1;

  return num_coeffs;
}

std::vector<double> computeHP(unsigned int filter_order) {
  std::vector<double> num_coeffs;

  num_coeffs = computeLP(filter_order);

  for (size_t i = 0; i <= filter_order; ++i)
    if (i % 2 != 0)
      num_coeffs[i] = -num_coeffs[i];

  return num_coeffs;
}

std::vector<double> computeDenCoeffs(unsigned int filter_order,
                                     double low_cutoff, double upper_cutoff) {
  double theta;     // PI * (Ucutoff - Lcutoff) / 2.0
  double cos_phi;   // cosine of phi
  double sin_theta; // sine of theta
  double cos_theta; // cosine of theta
  double s2t;       // sine of 2*theta
  double c2t;       // cosine 0f 2*theta
  std::vector<double> r_coeffs(static_cast<size_t>(2) *
                               filter_order); // z^-2 coefficients
  std::vector<double> t_coeffs(static_cast<size_t>(2) *
                               filter_order); // z^-1 coefficients
  std::vector<double> denom_coeffs;           // dk coefficients
  double pole_angle;                          // pole angle
  double sin_pole_angle;                      // sine of pole angle
  double cos_pole_angle;                      // cosine of pole angle

  cos_phi = cos(PI * (upper_cutoff + low_cutoff) / 2.0);
  theta = PI * (upper_cutoff - low_cutoff) / 2.0;
  sin_theta = sin(theta);
  cos_theta = cos(theta);
  s2t = 2.0 * sin_theta * cos_theta;       // sine of 2*theta
  c2t = 2.0 * cos_theta * cos_theta - 1.0; // cosine of 2*theta

  for (size_t k = 0; k < filter_order; ++k) {
    pole_angle = PI * (double)(2 * k + 1) / (double)(2 * filter_order);
    sin_pole_angle = sin(pole_angle);
    cos_pole_angle = cos(pole_angle);
    double divisor = 1.0 + s2t * sin_pole_angle;
    r_coeffs[2 * k] = c2t / divisor;
    r_coeffs[2 * k + 1] = s2t * cos_pole_angle / divisor;
    t_coeffs[2 * k] =
        -2.0 * cos_phi * (cos_theta + sin_theta * sin_pole_angle) / divisor;
    t_coeffs[2 * k + 1] = -2.0 * cos_phi * sin_theta * cos_pole_angle / divisor;
  }

  denom_coeffs = trinomialMultiply(filter_order, t_coeffs, r_coeffs);

  denom_coeffs[1] = denom_coeffs[0];
  denom_coeffs[0] = 1.0;
  for (size_t k = 3; k <= 2 * filter_order; ++k)
    denom_coeffs[k] = denom_coeffs[2 * k - 2];

  for (size_t i = denom_coeffs.size() - 1; i > filter_order * 2 + 1; i--)
    denom_coeffs.pop_back();

  return denom_coeffs;
}

std::vector<double> computeNumCoeffs(unsigned int filter_order,
                                     double lower_cutoff, double upper_cutoff,
                                     std::vector<double> den_c) {
  std::vector<double> t_coeffs;
  std::vector<double> num_coeffs(2 * filter_order + 1);
  std::vector<std::complex<double>> normalized_kernel(2 * filter_order + 1);

  std::vector<double> numbers;
  for (double i = 0; i < filter_order * 2 + 1; i++)
    numbers.push_back(i);

  t_coeffs = computeHP(filter_order);

  for (size_t i = 0; i < filter_order; ++i) {
    num_coeffs[2 * i] = t_coeffs[i];
    num_coeffs[2 * i + 1] = 0.0;
  }
  num_coeffs[static_cast<size_t>(2) * filter_order] = t_coeffs[filter_order];

  double cos_p[2];
  double wn_intermediate;
  cos_p[0] = 2 * 2.0 * tan(PI * lower_cutoff / 2.0);
  cos_p[1] = 2 * 2.0 * tan(PI * upper_cutoff / 2.0);

  // Bw = cp[1] - cp[0];
  // center frequency
  wn_intermediate = sqrt(cos_p[0] * cos_p[1]);
  wn_intermediate = 2 * atan2(wn_intermediate, 4);
  // double kern;
  const std::complex<double> k_result = std::complex<double>(-1, 0);

  for (size_t k = 0; k < filter_order * 2 + 1; k++) {
    normalized_kernel[k] =
        std::exp(-sqrt(k_result) * wn_intermediate * numbers[k]);
  }
  double intermediate = 0;
  double den = 0;
  for (size_t i = 0; i < filter_order * 2 + 1; i++) {
    intermediate += real(normalized_kernel[i] * num_coeffs[i]);
    den += real(normalized_kernel[i] * den_c[i]);
  }
  for (size_t j = 0; j < filter_order * 2 + 1; j++) {
    num_coeffs[j] = (num_coeffs[j] * den) / intermediate;
  }

  for (size_t i = num_coeffs.size() - 1; i > filter_order * 2 + 1; i--)
    num_coeffs.pop_back();

  return num_coeffs;
}

std::vector<double> filter(const std::vector<double> &input_vec,
                           std::vector<double> coeff_b,
                           std::vector<double> coeff_a) {
  size_t len_x = input_vec.size();
  size_t len_b = coeff_b.size();
  size_t len_a = coeff_a.size();

  std::vector<double> vec_zi(len_b);

  std::vector<double> filter_x(len_x);

  if (len_a == 1) {
    for (size_t j = 0; j < len_x; j++) {
      filter_x[j] = coeff_b[0] * input_vec[j] + vec_zi[0];
      for (size_t i = 1; i < len_b; i++) {
        vec_zi[i - 1] = coeff_b[i] * input_vec[j] + vec_zi[i];
      }
    }
  } else {
    for (size_t j = 0; j < len_x; j++) {
      filter_x[j] = coeff_b[0] * input_vec[j] + vec_zi[0];
      for (size_t i = 1; i < len_b; i++) {
        vec_zi[i - 1] =
            coeff_b[i] * input_vec[j] + vec_zi[i] - coeff_a[i] * filter_x[j];
      }
    }
  }

  return filter_x;
}

std::vector<double> bandPassFilter(const std::vector<double> &input,
                                   double sample_rate, double lower_cutoff,
                                   double upper_cutoff, size_t order) {
  double lower_frequency_band = lower_cutoff / sample_rate * 2;
  double upper_frequency_band = upper_cutoff / sample_rate * 2;

  auto a_coeffs =
      computeDenCoeffs(order, lower_frequency_band, upper_frequency_band);
  auto b_coeffs = computeNumCoeffs(order, lower_frequency_band,
                                   upper_frequency_band, a_coeffs);

  return filter(input, b_coeffs, a_coeffs);
}

} // namespace signal_easel