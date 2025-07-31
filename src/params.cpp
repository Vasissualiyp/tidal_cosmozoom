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
	n = obj.n;
	seed = obj.seed;
	boxsize = obj.boxsize;
	Omega_m = obj.Omega_m;
	h = obj.h;
	calculate_derived_params();
}
Parameters::Parameters(Parameters&& obj) noexcept {
    n       = obj.n;        // No std::move needed for primitives
    seed    = obj.seed;
    boxsize = obj.boxsize;
    Omega_m = obj.Omega_m;
    h       = obj.h;
    calculate_derived_params();
}
Parameters& Parameters::operator=(const Parameters& obj) {
    if (this != &obj) {     // Self-assignment check
        n       = obj.n;
        seed    = obj.seed;
        boxsize = obj.boxsize;
        Omega_m = obj.Omega_m;
        h       = obj.h;
        calculate_derived_params();
    }
    return *this;
}
Parameters& Parameters::operator=(const Parameters&& obj) noexcept {
    if (this != &obj) {     // Self-assignment check
        n       = obj.n;
        seed    = obj.seed;
        boxsize = obj.boxsize;
        Omega_m = obj.Omega_m;
        h       = obj.h;
        calculate_derived_params();
    }
    return *this;
}

void Parameters::set_value(std::string var_name, std::string var_value) {
	if (var_name == "n") {
		n = stoi(var_value);
	} else if (var_name == "seed") {
		seed = stoi(var_value);
	} else if (var_name == "boxsize") {
		boxsize = STRTOREAL(var_value);
	} else if (var_name == "Omega_m") {
		Omega_m = STRTOREAL(var_value);
	} else if (var_name == "h") {
		h = STRTOREAL(var_value);
	}
}
void Parameters::set_seed(int seed) {
	seed = seed;
}
void Parameters::reduce_meshsize(int dn) {
	n -= dn;
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
	calculate_derived_params();
}
void Parameters::calculate_derived_params() {
	dL = boxsize / n;
	dk = 2.0 * M_PI / boxsize;
}
std::string Parameters::erase_spaces_in_str(std::string s) {
s.erase(std::remove_if(s.begin(), s.end(),
						 [](char c) { return std::isspace(c); }),
		s.end());
	return s;
}

