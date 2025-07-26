#pragma once
#include "fft.hh"
#include "params.hh"
#include "tidal.hh"

void solve_poisson_in_fourier_space(FFTW::complex_type* overdensity_fft, 
									FFTW::complex_type* potential_fft,
									Parameters params);
void tidal_tensor_from_potential(FFTW::complex_type* potential_fft,
								 TensorField<std::complex<REAL>>& TidalTensor_fft,
								 Parameters params);
void ifft_tidal_tensor(TensorField<std::complex<REAL>>& TidalTensor_fft,
					   TensorField<REAL>& TidalTensor,
					   Parameters params);
void perform_single_tensor_component_fft(REAL* T_ptr, 
										 std::complex<REAL>* T_ptr_fft, 
										 int n);
void renormalize_post_fft_array(REAL* rand_array, int n);
