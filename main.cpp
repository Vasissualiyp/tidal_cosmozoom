#include <cstdlib>
#include <iostream>
#include <iomanip>
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

int main(int argc, char *argv[]) {
  
	int seed, n;
	// CLI: pass seed
	if (argc < 2) {
	    seed = 12345;
		n = 16;
	} else {
	    seed = atoi(argv[1]);
	    n = atoi(argv[2]);
	}


    // Random number generation
	std::mt19937 rand_gen(seed);
    std::uniform_real_distribution<> uniform_dist(-1.0, 1.0);

    // Populate the array with random values
	REAL* rand_array = new REAL[n*n*n];
	populate_array_with_random_values(rand_array, n, uniform_dist, rand_gen, 0);
  
    return 0;
}
