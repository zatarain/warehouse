#include <utz.hpp>
#include <models/model.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>

// Class to test the model, since model is an abstract/virtual class.
class item: public models::model<int> {
public:
	field<int> id;
	field<std::string> name;
	field<std::string> description;
	field<float> quantity;

	item(): model("../utz/data/my", "items"), // Using utz/data/my.json where the main entry is 'items'
		id("id"), name("name"),
		description("desc"), // Field with short name.
		quantity("quantity",
			std::bind(&item::get_quantity_from, this, std::placeholders::_1),
			std::bind(&item::set_quantity_to, this, std::placeholders::_1, std::placeholders::_2)
		) {
		fetch();
	}

	inline primary_key& get_primary_key() override { return id; }

protected:
	void operator<<(json::Value& node) override {
		read(node, id, name, description, quantity);
	}

	void operator>>(json::Value& node) override {
		write(node, id, name, description, quantity);
	}

private:
	float get_quantity_from(json::Value& node) {
		return node.GetFloat();
	}

	void set_quantity_to(json::Value& node, float value) {
		node.SetFloat(value);
	}
};

void utz::test() {
	utz::log << "Test cases for model." << std::endl;
	item record;

	utz::log << "Cheking that the model reads the JSON file properly." << std::endl;
	"Reading an existent record based-on the key returns true."
		| expect(record.read(2), is::equal, true);

	"Field tagged with 'id' was read correctly."
		| expect((int)record.id, is::equal, 2);

	"Field tagged with 'name' was read correctly."
		| expect((std::string)record.name, is::equal, std::string("Two"));

	"Field tagged with 'desc' was read correctly."
		| expect((std::string)record.description, is::equal, std::string("Second item"));

	"Field tagged with 'quantity' was read correctly."
		| expect(std::abs(4.31f - record.quantity) <= 1e-6, is::equal, true);

	"Try to read an inexistent returns false."
		| expect(record.read(0), is::equal, false);

	utz::log << "End of test cases for model." << std::endl;
}