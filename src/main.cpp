#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include "main.hpp"

using command_map = std::unordered_map<std::string, int>;

enum REQUEST_TYPE {
	NONE = 0,
	LIST = 1,
	SELL = 2,
	HELP = 3,
	EXIT = 4
};

const command_map commands{
	{"list", LIST},
	{"sell", SELL},
	{"help", HELP},
	{"exit", EXIT},
};

int main(int argc, char const *argv[]) {
	std::ios_base::sync_with_stdio(false);
	bool silent_mode = argc > 1;
	const std::string prompt(silent_mode ? "" : "Please type a request: ");
	int user_request = EXIT;
	std::string user_input;
	do {
		std::cout << prompt;
		std::getline(std::cin, user_input);
		std::stringstream command_line(user_input);
		std::string command_name;
		command_line >> command_name;
		command_map::const_iterator request = commands.find(command_name);
		user_request = request == commands.end() ? NONE : request->second;
		try {
			switch (user_request) {
				case LIST: std::cout << "Hello list!" << std::endl; break;
				case SELL: std::cout << "Hello sell!" << std::endl; break;
				case HELP: std::cout << "Hello help!" << std::endl; break;
				case EXIT: std::cout << "Bye! :)" << std::endl; break;
				default: throw std::invalid_argument("Error: unrecognized request, please try again.");
			}
		} catch(const std::exception& error) {
			std::cerr << error.what() << std::endl;
		}
	} while(user_request != EXIT);
	return EXIT_SUCCESS;
}
