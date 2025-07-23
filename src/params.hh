#include <fstream>
#include <cstdlib>
#include "vars.hh"

class Parameters {
public:
  void read_params_from_file(const char* filename);
  int  get_n();
  int  get_seed();
  REAL get_boxsize();
private:
  int n;
  int seed;
  float boxsize;
  void set_value(std::string var_name, std::string var_value);
  std::string erase_spaces_in_str(std::string s);
};

