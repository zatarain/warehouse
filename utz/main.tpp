#include <utz.hpp>
#include <main.hpp>
#include <cstdlib>
#include <iostream>
#include <sstream>

void utz::test() {
	utz::log << "Starting main test cases..." << std::endl;
	const char* arguments[] = {"program", "argument"};
	std::stringstream output;
	auto old_buffer = std::cout.rdbuf();
	std::cout.rdbuf(output.rdbuf());
	int result = __real_main(2, arguments);
	std::cout.rdbuf(old_buffer);

	"main call success (result == EXIT_SUCCESS)."
		| expect(result, is::equal, EXIT_SUCCESS);

	"main call puts the phrase 'Hello World!' to standard output with an end-of-line mark."
		| expect(output.str(), is::equal, std::string("Hello world!\n"));

	utz::log << "All test cases have finished!" << std::endl;
}
