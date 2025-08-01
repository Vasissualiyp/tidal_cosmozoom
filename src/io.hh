#pragma once
#include <string>
#include "fft.hh"
#include "params.hh"
#include "vars.hh"

void write_field_to_binary_file(REAL* rand_array, int n, 
								std::string filename="array3d.bin",
								int write_output=1);
void print_complex_array(FFTW::complex_type* array, int n0, int n1, int n2);
Parameters obtain_params_from_cli(int argc, char *argv[]);
std::string contruct_Tij_table_fname(Parameters params);
