#include <fstream>
#include <cstdlib>

class Parameters {
public:
  int n;
  int seed;
  float boxsize;
  void read_params_from_file(const char* filename);
private:
  void set_value(std::string var_name, std::string var_value);
  std::string erase_spaces_in_str(std::string s);
};

