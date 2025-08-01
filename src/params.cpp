#include "params.hh"
#include "vars.hh"
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <math.h>
#include <iomanip>
#include <ostream>

Parameters::Parameters(){};
Parameters::Parameters(const Parameters &obj) {
	write_fields_to_files = obj.write_fields_to_files;
	current_padding = obj.current_padding;
	num_bnd_cutoffs = obj.num_bnd_cutoffs;
	pad_with_box_avg = obj.pad_with_box_avg;
	n           = obj.n;
	dn          = obj.dn;
	n_eff       = obj.n_eff;
	padding     = obj.padding;
	min_n       = obj.min_n;
	seed        = obj.seed;
	seeds_num   = obj.seeds_num;
	output_logs = obj.output_logs;
	print_array = obj.print_array;
	boxsize     = obj.boxsize;
	Omega_m     = obj.Omega_m;
	h           = obj.h;
	calculate_derived_params();
}
Parameters::Parameters(Parameters&& obj) noexcept {
    write_fields_to_files = obj.write_fields_to_files;
	current_padding = obj.current_padding;
	num_bnd_cutoffs = obj.num_bnd_cutoffs;
    pad_with_box_avg = obj.pad_with_box_avg;
    n           = obj.n;        // No std::move needed for primitives
	dn          = obj.dn;
	n_eff       = obj.n_eff;
	padding     = obj.padding;
	min_n       = obj.min_n;
    seed        = obj.seed;
    seeds_num   = obj.seeds_num;
    output_logs = obj.output_logs;
    print_array = obj.print_array;
    boxsize     = obj.boxsize;
    Omega_m     = obj.Omega_m;
    h           = obj.h;
    calculate_derived_params();
}
Parameters& Parameters::operator=(const Parameters& obj) {
    if (this != &obj) {     // Self-assignment check
        write_fields_to_files = obj.write_fields_to_files;
		current_padding = obj.current_padding;
		num_bnd_cutoffs = obj.num_bnd_cutoffs;
        pad_with_box_avg = obj.pad_with_box_avg;
        n           = obj.n;
		dn          = obj.dn;
		n_eff       = obj.n_eff;
		padding     = obj.padding;
		min_n       = obj.min_n;
        seed        = obj.seed;
        seeds_num   = obj.seeds_num;
        output_logs = obj.output_logs;
        print_array = obj.print_array;
        boxsize     = obj.boxsize;
        Omega_m     = obj.Omega_m;
        h           = obj.h;
        calculate_derived_params();
    }
    return *this;
}
Parameters& Parameters::operator=(const Parameters&& obj) noexcept {
    if (this != &obj) {     // Self-assignment check
        write_fields_to_files = obj.write_fields_to_files;
		current_padding = obj.current_padding;
		num_bnd_cutoffs = obj.num_bnd_cutoffs;
        pad_with_box_avg = obj.pad_with_box_avg;
        n           = obj.n;
		dn          = obj.dn;
		n_eff       = obj.n_eff;
		padding     = obj.padding;
		min_n       = obj.min_n;
        seed        = obj.seed;
        seeds_num   = obj.seeds_num;
        output_logs = obj.output_logs;
        print_array = obj.print_array;
        boxsize     = obj.boxsize;
        Omega_m     = obj.Omega_m;
        h           = obj.h;
        calculate_derived_params();
    }
    return *this;
}

void Parameters::set_value(std::string var_name, std::string var_value) {
	if (var_name == "n") {
		n = stoi(var_value);
	} else if (var_name == "dn") {
		dn = stoi(var_value);
	} else if (var_name == "padding") {
		padding = stoi(var_value);
	} else if (var_name == "min_n") {
		min_n = stoi(var_value);
	} else if (var_name == "seed") {
		seed = stoi(var_value);
	} else if (var_name == "seeds_num") {
		seeds_num = stoi(var_value);
	} else if (var_name == "pad_with_box_avg") {
		pad_with_box_avg = stoi(var_value);
	} else if (var_name == "write_fields_to_files") {
		write_fields_to_files = stoi(var_value);
	} else if (var_name == "output_logs") {
		output_logs = stoi(var_value);
	} else if (var_name == "print_array") {
		print_array = stoi(var_value);
	} else if (var_name == "boxsize") {
		boxsize = STRTOREAL(var_value);
	} else if (var_name == "Omega_m") {
		Omega_m = STRTOREAL(var_value);
	} else if (var_name == "h") {
		h = STRTOREAL(var_value);
	}
}
void Parameters::set_seed(int new_seed) {
	seed = new_seed;
}
void Parameters::reset(int initial_n) {
	current_padding = 0;
	n = initial_n;
	n_eff = initial_n;
	calculate_derived_params();
}
void Parameters::reduce_meshsize() {
	n_eff = n_eff - 2*dn;
	n -= 2*(dn + current_padding);
	calculate_derived_params();
}
void Parameters::increase_meshsize() {
	n += 2*(current_padding + padding);
	current_padding += padding;
	calculate_derived_params();
}
void Parameters::read_params_from_file(const char* filename) {
	using namespace std;
	string s, var_name, var_value;
	ifstream f(filename);
	char del = '='; // Equation delimiter
		if (!f.is_open()) {
			cerr << "Error opening the file " << filename << "!\n";
		}
	while (getline(f, s)) {
		stringstream ss(s);
		getline(ss, var_name, del);
		getline(ss, var_value, del);
		var_name = erase_spaces_in_str(var_name);
		var_value = erase_spaces_in_str(var_value);
		set_value(var_name, var_value);
	}
	f.close();
	reset(n);
}
void Parameters::calculate_derived_params() {
	dL = boxsize / n;
	dk = 2.0 * M_PI / boxsize;
	num_bnd_cutoffs = int((n - min_n) / dn / 2);
}
std::string Parameters::erase_spaces_in_str(std::string s) {
s.erase(std::remove_if(s.begin(), s.end(),
						 [](char c) { return std::isspace(c); }),
		s.end());
	return s;
}
