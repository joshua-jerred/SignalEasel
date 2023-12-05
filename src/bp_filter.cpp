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

#define PI 3.14159

namespace signal_easel {

std::vector<double> trinomialMultiply(int filter_order,
                                      const std::vector<double> &b,
                                      const std::vector<double> &c) {
  int i, j;
  std::vector<double> return_vector(4 * filter_order);

  return_vector[2] = c.at(0);
  return_vector[3] = c.at(1);
  return_vector[0] = b.at(0);
  return_vector[1] = b.at(1);

  for (i = 1; i < filter_order; ++i) {
    return_vector[2 * (2 * i + 1)] +=
        c[2 * i] * return_vector[2 * (2 * i - 1)] -
        c[2 * i + 1] * return_vector[2 * (2 * i - 1) + 1];
    return_vector[2 * (2 * i + 1) + 1] +=
        c[2 * i] * return_vector[2 * (2 * i - 1) + 1] +
        c[2 * i + 1] * return_vector[2 * (2 * i - 1)];

    for (j = 2 * i; j > 1; --j) {
      return_vector[2 * j] += b[2 * i] * return_vector[2 * (j - 1)] -
                              b[2 * i + 1] * return_vector[2 * (j - 1) + 1] +
                              c[2 * i] * return_vector[2 * (j - 2)] -
                              c[2 * i + 1] * return_vector[2 * (j - 2) + 1];
      return_vector[2 * j + 1] += b[2 * i] * return_vector[2 * (j - 1) + 1] +
                                  b[2 * i + 1] * return_vector[2 * (j - 1)] +
                                  c[2 * i] * return_vector[2 * (j - 2) + 1] +
                                  c[2 * i + 1] * return_vector[2 * (j - 2)];
    }

    return_vector[2] += b[2 * i] * return_vector[0] -
                        b[2 * i + 1] * return_vector[1] + c[2 * i];
    return_vector[3] += b[2 * i] * return_vector[1] +
                        b[2 * i + 1] * return_vector[0] + c[2 * i + 1];
    return_vector[0] += b[2 * i];
    return_vector[1] += b[2 * i + 1];
  }

  return return_vector;
}

std::vector<double> ComputeLP(int filter_order) {
  std::vector<double> num_coeffs(filter_order + 1);
  int m;
  int i;

  num_coeffs[0] = 1;
  num_coeffs[1] = filter_order;
  m = filter_order / 2;
  for (i = 2; i <= m; ++i) {
    num_coeffs[i] = (double)(filter_order - i + 1) * num_coeffs[i - 1] / i;
    num_coeffs[filter_order - i] = num_coeffs[i];
  }
  num_coeffs[filter_order - 1] = filter_order;
  num_coeffs[filter_order] = 1;

  return num_coeffs;
}

std::vector<double> ComputeHP(int FilterOrder) {
  std::vector<double> NumCoeffs;
  int i;

  NumCoeffs = ComputeLP(FilterOrder);

  for (i = 0; i <= FilterOrder; ++i)
    if (i % 2)
      NumCoeffs[i] = -NumCoeffs[i];

  return NumCoeffs;
}

std::vector<double> ComputeDenCoeffs(int FilterOrder, double Lcutoff,
                                     double Ucutoff) {
  int k;        // loop variables
  double theta; // PI * (Ucutoff - Lcutoff) / 2.0
  double cp;    // cosine of phi
  double st;    // sine of theta
  double ct;    // cosine of theta
  double s2t;   // sine of 2*theta
  double c2t;   // cosine 0f 2*theta
  std::vector<double> RCoeffs(2 * FilterOrder); // z^-2 coefficients
  std::vector<double> TCoeffs(2 * FilterOrder); // z^-1 coefficients
  std::vector<double> DenomCoeffs;              // dk coefficients
  double PoleAngle;                             // pole angle
  double SinPoleAngle;                          // sine of pole angle
  double CosPoleAngle;                          // cosine of pole angle
  double a;                                     // workspace variables

  cp = cos(PI * (Ucutoff + Lcutoff) / 2.0);
  theta = PI * (Ucutoff - Lcutoff) / 2.0;
  st = sin(theta);
  ct = cos(theta);
  s2t = 2.0 * st * ct;       // sine of 2*theta
  c2t = 2.0 * ct * ct - 1.0; // cosine of 2*theta

  for (k = 0; k < FilterOrder; ++k) {
    PoleAngle = PI * (double)(2 * k + 1) / (double)(2 * FilterOrder);
    SinPoleAngle = sin(PoleAngle);
    CosPoleAngle = cos(PoleAngle);
    a = 1.0 + s2t * SinPoleAngle;
    RCoeffs[2 * k] = c2t / a;
    RCoeffs[2 * k + 1] = s2t * CosPoleAngle / a;
    TCoeffs[2 * k] = -2.0 * cp * (ct + st * SinPoleAngle) / a;
    TCoeffs[2 * k + 1] = -2.0 * cp * st * CosPoleAngle / a;
  }

  DenomCoeffs = trinomialMultiply(FilterOrder, TCoeffs, RCoeffs);

  DenomCoeffs[1] = DenomCoeffs[0];
  DenomCoeffs[0] = 1.0;
  for (k = 3; k <= 2 * FilterOrder; ++k)
    DenomCoeffs[k] = DenomCoeffs[2 * k - 2];

  for (int i = DenomCoeffs.size() - 1; i > FilterOrder * 2 + 1; i--)
    DenomCoeffs.pop_back();

  return DenomCoeffs;
}

std::vector<double> ComputeNumCoeffs(int FilterOrder, double Lcutoff,
                                     double Ucutoff, std::vector<double> DenC) {
  std::vector<double> TCoeffs;
  std::vector<double> NumCoeffs(2 * FilterOrder + 1);
  std::vector<std::complex<double>> NormalizedKernel(2 * FilterOrder + 1);

  std::vector<double> Numbers;
  for (double n = 0; n < FilterOrder * 2 + 1; n++)
    Numbers.push_back(n);
  int i;

  TCoeffs = ComputeHP(FilterOrder);

  for (i = 0; i < FilterOrder; ++i) {
    NumCoeffs[2 * i] = TCoeffs[i];
    NumCoeffs[2 * i + 1] = 0.0;
  }
  NumCoeffs[2 * FilterOrder] = TCoeffs[FilterOrder];

  double cp[2];
  double Wn;
  cp[0] = 2 * 2.0 * tan(PI * Lcutoff / 2.0);
  cp[1] = 2 * 2.0 * tan(PI * Ucutoff / 2.0);

  // Bw = cp[1] - cp[0];
  // center frequency
  Wn = sqrt(cp[0] * cp[1]);
  Wn = 2 * atan2(Wn, 4);
  // double kern;
  const std::complex<double> result = std::complex<double>(-1, 0);

  for (int k = 0; k < FilterOrder * 2 + 1; k++) {
    NormalizedKernel[k] = std::exp(-sqrt(result) * Wn * Numbers[k]);
  }
  double b = 0;
  double den = 0;
  for (int d = 0; d < FilterOrder * 2 + 1; d++) {
    b += real(NormalizedKernel[d] * NumCoeffs[d]);
    den += real(NormalizedKernel[d] * DenC[d]);
  }
  for (int c = 0; c < FilterOrder * 2 + 1; c++) {
    NumCoeffs[c] = (NumCoeffs[c] * den) / b;
  }

  for (int i = NumCoeffs.size() - 1; i > FilterOrder * 2 + 1; i--)
    NumCoeffs.pop_back();

  return NumCoeffs;
}

std::vector<double> filter(const std::vector<double> &input_vec,
                           std::vector<double> coeff_b,
                           std::vector<double> coeff_a) {
  int len_x = input_vec.size();
  int len_b = coeff_b.size();
  int len_a = coeff_a.size();

  std::vector<double> zi(len_b);

  std::vector<double> filter_x(len_x);

  if (len_a == 1) {
    for (int m = 0; m < len_x; m++) {
      filter_x[m] = coeff_b[0] * input_vec[m] + zi[0];
      for (int i = 1; i < len_b; i++) {
        zi[i - 1] = coeff_b[i] * input_vec[m] + zi[i];
      }
    }
  } else {
    for (int m = 0; m < len_x; m++) {
      filter_x[m] = coeff_b[0] * input_vec[m] + zi[0];
      for (int i = 1; i < len_b; i++) {
        zi[i - 1] =
            coeff_b[i] * input_vec[m] + zi[i] - coeff_a[i] * filter_x[m];
      }
    }
  }

  return filter_x;
}

std::vector<double> bandPassFilter(std::vector<double> &input,
                                   double sample_rate, double lower_cutoff,
                                   double upper_cutoff, int order) {
  double frequency_bands[2] = {lower_cutoff / sample_rate * 2,
                               upper_cutoff / sample_rate * 2};

  auto a = ComputeDenCoeffs(order, frequency_bands[0], frequency_bands[1]);
  auto b = ComputeNumCoeffs(order, frequency_bands[0], frequency_bands[1], a);

  return filter(input, b, a);
}

} // namespace signal_easel

int main() {
  std::ifstream ifile;
  ifile.open("input_data.txt");
  std::vector<double> input, output;

  double sample_rate = 48000;
  int order = 4;
  double lower_cutoff = 1000;
  double upper_cutoff = 2000;

  while (ifile.good()) {
    double x;
    ifile >> x;
    input.push_back(x);
  }

  output = signal_easel::bandPassFilter(input, sample_rate, lower_cutoff,
                                        upper_cutoff, order);

  std::ofstream ofile;
  ofile.open("output_data.csv");

  for (double out_val : output) {
    ofile << out_val << std::endl;
  }
  ofile.close();

  return 0;
}