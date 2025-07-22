#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <random>

#define REAL float

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
  	  string s, t1, t2;
  	  ifstream f(filename);
	  char del = '='; // Equation delimiter
  	  if (!f.is_open()) {
  	  	cerr << "Error opening the file " << filename << "!\n";
  	  }
	  while (getline(f, s)) {
	      stringstream ss(s);
		  getline(ss, t1, del);
		  getline(ss, t2, del);
	      cout << t1 << " = " << t2 << endl;
	  }
	  f.close();
  }
//private:
  // private members
};


int main(int argc, char *argv[]) {
  
	int seed, n;
	short print_array = 0;
	const char* filename;

	// CLI: pass seed and box size (in # of cells)
	if (argc < 2) {
	    seed = 12345;
		n = 16;
		filename = "params.txt";
	} else {
	    seed = atoi(argv[1]);
	    n = atoi(argv[2]);
	    filename = argv[3];
	}


    // Random number generation
	std::mt19937 rand_gen(seed);
    std::uniform_real_distribution<> uniform_dist(-1.0, 1.0);

	REAL* rand_array = new REAL[n*n*n];
	populate_array_with_random_values(rand_array, n, uniform_dist, 
			                          rand_gen, print_array);

	Parameters params;
	params.read_params_from_file(filename);
  
    return 0;
}
