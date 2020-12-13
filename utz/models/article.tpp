#include <utz.hpp>
#include <models/article.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>
#include <unordered_set>

using models::article;

std::string subscribe_to_inexistent() {
	article mut; // model under test
	try {
		mut.subscribe(0, "dinning chair");
	} catch(const std::exception& not_found) {
		return not_found.what();
	}
	return "";
}

void test_subscribe() {
	article mut; // model under test
	mut.subscribe(1, "dinning chair");
	mut.read(1);
	std::unordered_set<std::string> actual = mut.get_subscribers();
	std::unordered_set<std::string> expected{"dinning chair"};
	"Subscribing to an article add the subscription to the correspondent set."
		| expect(actual.size(), is::equal, expected.size());

	mut.subscribe(1, "dinning chair");
	for(auto& subscriber: expected)
		"Subscribing to an article add the subscription exactly once to the correspondent set."
			| expect(actual.count(subscriber), is::equal, 1);

	"Subscribing to an inexistent article throws the proper exception."
		| expect(subscribe_to_inexistent(), is::equal, std::string("Record with key = '0' not found!"));
}

void test_read() {
	article mut; // model under test
	mut.read(1);
	"Id was read properly."
		| expect(mut.get_id(), is::equal, 1);
	"Name was read properly."
		| expect(mut.get_name(), is::equal, std::string("leg"));
	"Stock was read properly."
		| expect(mut.get_stock(), is::equal, 12);
}

void utz::test() {
	test_read();
	test_subscribe();
}