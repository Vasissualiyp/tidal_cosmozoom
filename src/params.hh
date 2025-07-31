#pragma once
#include <fstream>
#include <cstdlib>
#include "vars.hh"

class Parameters {
public:
	Parameters();
	Parameters(const Parameters &obj);
	Parameters(Parameters&& obj) noexcept;
	Parameters& operator=(const Parameters& obj);
	Parameters& operator=(const Parameters&& obj) noexcept;
	void read_params_from_file(const char* filename);
	void reduce_meshsize();
	void increase_meshsize();
	void set_seed(int seed);
	// Parameter getter template
	template<typename T>
	T get(const std::string& param_name) {
		if constexpr (std::is_same_v<T, int>) {
			if (param_name == "n") return n;
			if (param_name == "n_eff") return n_eff;
			if (param_name == "dn") return dn;
			if (param_name == "num_bnd_cutoffs") return num_bnd_cutoffs;
			if (param_name == "padding") return padding;
			if (param_name == "min_n") return min_n;
			if (param_name == "seed") return seed;
			if (param_name == "current_padding") return current_padding;
			if (param_name == "output_logs") return output_logs;
			if (param_name == "write_fields_to_files") return write_fields_to_files;
			if (param_name == "print_array") return print_array;
		} else if constexpr (std::is_same_v<T, REAL>) {
			if (param_name == "boxsize") return boxsize;
			if (param_name == "Omega_m") return Omega_m;
			if (param_name == "h") return h;
			if (param_name == "dL") return dL;
			if (param_name == "dk") return dk;
		} else if constexpr (std::is_same_v<T, std::string>) {
		}
		throw std::invalid_argument("Unknown parameter: " + param_name);
	}
private:
	int n, n_eff, seed, dn, padding, min_n, current_padding, num_bnd_cutoffs;
	int print_array, output_logs, write_fields_to_files;
	REAL boxsize, Omega_m, h, dk, dL;
	void set_value(std::string var_name, std::string var_value);
	void calculate_derived_params();
	std::string erase_spaces_in_str(std::string s);
};
