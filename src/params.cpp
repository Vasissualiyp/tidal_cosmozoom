#include "params.hh"
#include "vars.hh"
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <iomanip>

  void Parameters::set_value(std::string var_name, std::string var_value) {
	  if (var_name == "n") {
		  n = stoi(var_value);
	  } else if (var_name == "seed") {
		  seed = stoi(var_value);
	  } else if (var_name == "boxsize") {
		  boxsize = STRTOREAL(var_value);
	  }
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
  }
  std::string Parameters::erase_spaces_in_str(std::string s) {
      s.erase(std::remove_if(s.begin(), s.end(),
		                     [](char c) { return std::isspace(c); }),
	          s.end());
	  return s;
  }

