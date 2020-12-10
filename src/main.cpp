#include <iostream>
#include <cstdlib>

#include "main.hpp"

int main(int argc, char const *argv[]) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL); std::cout.tie(NULL);
	std::cerr.tie(NULL); std::clog.tie(NULL);

	std::cout << "Hello world!" << std::endl;
	return EXIT_SUCCESS;
}
