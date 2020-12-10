#include <utz.hpp>
#include <models/field.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>
#include <rapidjson/document.h>

namespace json = rapidjson;

int read_integer_from(json::Value& node) {
	return node.GetInt();
}

void write_integer_to(json::Value& node, int value) {
	node.SetInt(value);
}

void utz::test() {
	utz::log << "Test cases for field." << std::endl;
	json::Document document;
	json::Document::AllocatorType& allocator = document.GetAllocator();
	json::Value object(json::kObjectType);

	utz::log << "Test default getters and setters (parse from string):" << std::endl;
	models::field<int> parsed_integer("parsed");
	object.AddMember("parsed", "2", allocator);
	int parse_value = parsed_integer.get(object);
	"field::get reads the correct internal value from JSON node using the defauilt getter."
		| expect(parse_value, is::equal, 2);

	parsed_integer = 4;
	parsed_integer.set(object);
	"field::set writes the correct internal value to JSON node using the default setter."
		| expect(object["parsed"].GetString(), is::equal, std::string("4"));

	utz::log << "Test custom getters and setters (extract integer value):" << std::endl;
	models::field<int> custom_integer("custom", &read_integer_from, &write_integer_to);
	object.AddMember("custom", 5, allocator);
	int custom_value = custom_integer.get(object);
	"field::get reads the correct internal value from JSON node using the custom getter."
		| expect(custom_value, is::equal, 5);

	custom_integer = 7;
	custom_integer.set(object);
	"field::set writes the correct internal value to JSON node using the custom setter."
		| expect(object["custom"].GetInt(), is::equal, 7);

	utz::log << "End of test cases for field." << std::endl;
}