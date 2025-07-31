#pragma once
#include <string>
#include "fft.hh"
#include "vars.hh"

void write_field_to_binary_file(REAL* rand_array, int n, 
								std::string filename="array3d.bin",
								bool write_output=true);
void print_complex_array(FFTW::complex_type* array, int n0, int n1, int n2);
