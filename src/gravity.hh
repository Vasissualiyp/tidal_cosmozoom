#pragma once
#include "fft.hh"
#include "params.hh"
#include "tidal.hh"

void solve_poisson_in_fourier_space(FFTW::complex_type* overdensity_fft, 
									FFTW::complex_type* potential_fft,
									Parameters params);
void tidal_tensor_from_potential(FFTW::complex_type* potential_fft,
								 TensorField<std::complex<REAL>>& TidalTensor,
								 Parameters params);
