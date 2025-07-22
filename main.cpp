#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <algorithm>
#include <random>

#define REAL float
#define STRTOREAL stof

void populate_array_with_random_values(REAL* rand_array, int n, 
		                               std::uniform_real_distribution<> uniform_dist,
		                               std::mt19937 rand_gen,
									   short print_array = 0) {
	REAL element;
	for (int i=0; i<n; i++) {
	    for (int j=0; j<n; j++) {
	        for (int k=0; k<n; k++) {
				*(rand_array + i * n * n + j * n + k) = uniform_dist(rand_gen);
				element = rand_array[i*n*n + j*n + k];
				if (print_array == 1) {
	                std::cout << element << " ";
				}
	        }
			if (print_array == 1) {
	            std::cout << std::endl;
			}
	    }
	}
}

class Parameters {
public:
  int n;
  int seed;
  float boxsize;
  void read_params_from_file(const char* filename) {
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
		  //remove_if(var_name.begin(), var_name.end(), isspace);
	  }
	  f.close();
  }
private:
  void set_value(std::string var_name, std::string var_value) {
	  if (var_name == "n") {
		  n = stoi(var_value);
	  } else if (var_name == "seed") {
		  seed = stoi(var_value);
	  } else if (var_name == "boxsize") {
		  boxsize = STRTOREAL(var_value);
	  }
  }
  std::string erase_spaces_in_str(std::string s) {
      s.erase(std::remove_if(s.begin(), s.end(),
		                     [](char c) { return std::isspace(c); }),
	          s.end());
	  return s;
  }
};


int main(int argc, char *argv[]) {
  
	short print_array = 0;
	const char* filename;

	// CLI: pass path to the parameter file 
	if (argc < 2)
		filename = "params.txt";
	else
	    filename = argv[1];

	Parameters params;
	params.read_params_from_file(filename);

    // Random number generation
	std::mt19937 rand_gen(params.seed);
    std::uniform_real_distribution<> uniform_dist(-1.0, 1.0);

	int n = params.n;
	REAL* rand_array = new REAL[n*n*n];
	populate_array_with_random_values(rand_array, n, uniform_dist, 
			                          rand_gen, print_array);

  
    return 0;
}
