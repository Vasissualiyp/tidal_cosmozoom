#pragma once
#include "fft.hh"
#include "params.hh"
#include "tidal.hh"

void solve_poisson_in_fourier_space(FFTW::complex_type* overdensity_fft, 
									FFTW::complex_type* potential_fft,
									Parameters params);
void ifft_tidal_tensor(TensorField<std::complex<REAL>>& TidalTensor_fft,
					   TensorField<REAL>& TidalTensor,
					   Parameters params);
void calculate_and_save_fields_from_overdensity(REAL* overdensity, 
												Parameters params,
												bool print_header=false);
void generate_overdensity_field(REAL*& overdensity, Parameters& params);
int* get_new_center_from_largest_potential(FFTW::complex_type* overdensity_fft, 
										   Parameters params);
