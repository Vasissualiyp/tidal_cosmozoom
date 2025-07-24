#include <random>
#include "vars.hh"

void populate_array_with_random_values(REAL* rand_array, int n, 
									   std::uniform_real_distribution<> uniform_dist,
									   std::mt19937 rand_gen,
									   short print_array = 0);
