#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char *argv[]) {
  
	int seed;
	// CLI: pass seed
	if (argc < 2) {
	    seed = 12345;
	} else {
	    seed = atoi(argv[1]);
	}

    // Random number generation
	std::mt19937 rand_gen(seed);
    std::uniform_real_distribution<> uniform_dist(-1.0, 1.0);
  
    std::cout << uniform_dist(rand_gen) << std::endl;
  
    return 0;
  
}
