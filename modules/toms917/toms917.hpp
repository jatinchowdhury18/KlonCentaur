#pragma once

#include <complex>

std::complex <double> wrightomega ( std::complex <double> z );
int wrightomega_ext ( std::complex <double> z, std::complex <double> &w, 
  std::complex <double> &e, std::complex <double> &r, std::complex <double> &condest );
void timestamp ( );
