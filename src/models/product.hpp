#ifndef PRODUCT_HEADER
#define PRODUCT_HEADER

#include <iostream>
#include <string>
#include <map>
#include <limits>
#include <functional>

#include "field.hpp"
#include "model.hpp"
#include "article.hpp"

namespace models {
	using list_of_articles = std::map<int, int>;

	class product: public model<std::string> {
	public:
		product(article*);
		inline primary_key& get_primary_key() override { return name; }
		std::string get_name();
		list_of_articles get_requirements();
		int get_availability();
		void list();
		bool is_available();
		void update_availability(int);

	protected:
		void operator<<(json::Value&) override;
		void operator>>(json::Value&) override;

	private:
		static const std::string article_id_key;
		static const std::string amount_key;
		models::article* inventory;
		std::map<std::string, int> availability;
		field<std::string> name;
		field<list_of_articles> requirements;

		list_of_articles get_requirements_from(json::Value&);
		void set_requirements_to(json::Value&, const list_of_articles&);
		void compute_initial_availabilities();
		void update_availability(const std::string&);
	};
}

using models::model;
using models::product;
using models::list_of_articles;

const std::string product::article_id_key("art_id");
const std::string product::amount_key("amount_of");

product::product(article* inventory):
	model("products"), inventory(inventory), name("name"),
	requirements("contain_articles",
		std::bind(&product::get_requirements_from, this, std::placeholders::_1),
		std::bind(&product::set_requirements_to, this, std::placeholders::_1, std::placeholders::_2)
	) {

	if (inventory == NULL) {
		throw std::invalid_argument("Invalid inventory.");
	}

	fetch();
	compute_initial_availabilities();
}

inline void product::operator<<(json::Value& node) { read(node, name, requirements); }

inline void product::operator>>(json::Value& node) { write(node, name, requirements); }

inline std::string product::get_name() { return name; }

inline list_of_articles product::get_requirements() { return requirements; };

int product::get_availability() {
	if (!exists(name)) {
		throw invalid_key(name);
	}

	return availability[name];
}

inline bool product::is_available() { return get_availability() > 0; }

list_of_articles product::get_requirements_from(json::Value& node) {
	list_of_articles materials;
	for (auto& material: node.GetArray()) {
		int article_id = std::stoi(material[article_id_key.c_str()].GetString());
		int amount = std::stoi(material[amount_key.c_str()].GetString());
		if (amount <= 0) continue;
		materials[article_id] = amount;
		inventory->subscribe(article_id, name);
	}
	return materials;
};

void product::set_requirements_to(json::Value& node, const list_of_articles& requirements) {
	json::Document::AllocatorType& allocator = document.GetAllocator();
	json::Value list(json::kArrayType);
	for (auto& material: requirements) {
		std::string* article_id = new std::string(std::to_string(material.first));
		std::string* amount = new std::string(std::to_string(material.second));
		json::Value entry(json::kObjectType);
		entry.AddMember(json::StringRef(article_id_key.c_str()), json::StringRef(article_id->c_str()), allocator);
		entry.AddMember(json::StringRef(amount_key.c_str()), json::StringRef(amount->c_str()), allocator);
		list.PushBack(entry, allocator);
	}
	node = list;
};

void product::compute_initial_availabilities() {
	for (auto& record: dataset) {
		update_availability(record.first);
	}
}

void product::update_availability(const std::string& name) {
	if (!read(name) ) {
		throw invalid_key(name);
	}
	availability[name] = std::numeric_limits<int>::max();
	list_of_articles requirements = this->requirements;
	for (auto& material: requirements) {
		update_availability(material.first);
	}
}

void product::update_availability(int article_id) {
	const list_of_articles& requirements = get_requirements();
	list_of_articles::const_iterator result = requirements.find(article_id);
	if (result != requirements.end()) {
		inventory->read(article_id);
		int can_afford = inventory->get_stock() / result->second;
		std::clog << "Updating availability for '" << name << "' based-on article ["
			<< "id=" << inventory->get_id() << ", "
			<< "name='" << inventory->get_name() << "', "
			<< "stock=" << inventory->get_stock() << ", "
			<< "required=" << result->second << "]; "
			<< "can afford: " << can_afford
		<< std::endl;
		availability[name] = std::min(can_afford, availability[name]);
	}
}

#endif // PRODUCT_HEADER