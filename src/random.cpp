#include <iostream>
#include <random>
#include "vars.hh"

void populate_array_with_random_values(REAL* rand_array, int n, 
									   std::uniform_real_distribution<> uniform_dist,
									   std::mt19937 rand_gen, int print_array) {
	REAL element;
	for (int i=0; i<n; i++) {
		for (int j=0; j<n; j++) {
			for (int k=0; k<n; k++) {
				*(rand_array + i * n * n + j * n + k) = uniform_dist(rand_gen);
				element = rand_array[i*n*n + j*n + k];
				if (print_array == 1)
					std::cout << element << " ";
			}
			if (print_array == 1)
				std::cout << std::endl;
		}
	}
}
