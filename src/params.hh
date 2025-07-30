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
  void reduce_meshsize(int dn);
  // Parameter getter template
  template<typename T>
  T get(const std::string& param_name) {
    if constexpr (std::is_same_v<T, int>) {
      if (param_name == "n") return n;
      if (param_name == "seed") return seed;
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
  int n;
  int seed;
  REAL boxsize;
  REAL Omega_m;
  REAL h;
  REAL dk;
  REAL dL;
  void set_value(std::string var_name, std::string var_value);
  void calculate_derived_params();
  std::string erase_spaces_in_str(std::string s);
};
