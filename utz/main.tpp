#include <utz.hpp>
#include <main.hpp>
#include <cstdlib>
#include <iostream>
#include <sstream>

void utz::test() {
	utz::log << "Starting main test cases..." << std::endl;
	const char* arguments[] = {"application", "--silent"};

	// Stream to inject the input.
	std::stringstream input;
	input << "list" << std::endl << "exit" << std::endl;
	// Stream to capture the output.
	std::stringstream output;

	// Save the default buffers before redirection.
	auto old_input_buffer = std::cin.rdbuf();
	auto old_output_buffer = std::cout.rdbuf();

	// Redirection for standard input and output.
	std::cin.rdbuf(input.rdbuf());
	std::cout.rdbuf(output.rdbuf());

	// Call to the actual main function.
	int result = __real_main(2, arguments);

	// Restoring standard input and output.
	std::cout.rdbuf(old_output_buffer);
	std::cin.rdbuf(old_input_buffer);

	std::stringstream expected_output;
	expected_output << "Hello list!" << std::endl << "Bye! :)" << std::endl;
	"main call puts the expected phrases to standard output with an end-of-line mark."
		| expect(output.str(), is::equal, expected_output.str());

	"main call success (result == EXIT_SUCCESS)."
		| expect(result, is::equal, EXIT_SUCCESS);

	utz::log << "All test cases have finished!" << std::endl;
}
