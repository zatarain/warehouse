#ifndef WAREHOUSE_CONTROLLER_HEADER
#define WAREHOUSE_CONTROLLER_HEADER

#include <iostream>
#include <fstream>
#include <sstream>

#include "models/article.hpp"
#include "models/product.hpp"

namespace controllers {
	class warehouse {
		private:
			models::product* product;
			models::article* article;
			void dump(std::ostream&, const std::string&);
		public:
			warehouse();
			void list(std::stringstream&);
			void sell(std::stringstream&);
			void help(std::stringstream&);
			void exit(std::stringstream&);
	};
}

using controllers::warehouse;

warehouse::warehouse() {
	article = new models::article();
	product = new models::product(article);
}

void warehouse::list(std::stringstream& arguments) {
	for (auto& name: product->get_all_keys()) {
		product->read(name);
		std::cout << name << ": " << product->get_availability() << std::endl;
	}
}

void warehouse::sell(std::stringstream& arguments) {
	std::string name;
	std::getline(arguments, name);
	std::clog << "Trying to sell a '" << name << "'..." << std::endl;

	if (!product->read(name)) {
		throw std::invalid_argument("Product doesn't exists!");
	}

	if (!product->is_available()) {
		throw std::invalid_argument("Product is not available!");
	}

	for (auto& [article_id, amount]: product->get_requirements()) {
		article->read(article_id);
		article->set_stock(article->get_stock() - amount);
		article->write();
		for ( auto& name: article->get_subscribers() ) {
			product->read(name);
			product->update_availability(article_id);
		}
	}

	std::clog << "We just sold a '" << name << "', yaaay!! :)" << std::endl;
}

void warehouse::dump(std::ostream& output, const std::string& filename) {
	std::ifstream file(filename);
	std::string line;
	while(std::getline(file, line)) {
		output << line << std::endl;
	}
	file.close();
}


void warehouse::help(std::stringstream& arguments) {
	dump(std::cout, "docs/help.md");
}

void warehouse::exit(std::stringstream& arguments) {
	article->commit();
	dump(std::clog, "data/inventory.json");
	std::cout << "Bye! :)" << std::endl;
}

#endif // WAREHOUSE_CONTROLLER_HEADER
