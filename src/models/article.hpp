#ifndef ARTICLE_HEADER
#define ARTICLE_HEADER

#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "field.hpp"
#include "model.hpp"

template<typename Type>
using hashset = std::unordered_set<Type>;

template<typename Key, typename Value>
using hashmap = std::unordered_map<Key, Value>;

namespace models {
	class article: public model<int> {
	public:
		article();
		inline primary_key& get_primary_key() override { return id; }
		int get_id();
		std::string get_name();
		int get_stock();
		void set_name(const std::string&);
		void set_stock(int);
		bool subscribe(int, const std::string&);
		hashset<std::string> get_subscribers();

	protected:
		void operator<<(json::Value&) override;
		void operator>>(json::Value&) override;

	private:
		field<int> id;
		field<std::string> name;
		field<int> stock;
		hashmap<int, hashset<std::string>> subscribers;
	};
}

using models::model;
using models::article;

article::article():
	model("inventory"), id("art_id"),
	name("name"), stock("stock") {

	fetch();
}

inline void article::operator<<(json::Value& node) { read(node, id, name, stock); }

inline void article::operator>>(json::Value& node) { write(node, id, stock); }

inline int article::get_id() { return id; }

inline std::string article::get_name() { return name; }

inline int article::get_stock() { return stock; }

inline void article::set_name(const std::string& name) { this->name = name; }

inline void article::set_stock(int stock) { this->stock = stock; }

hashset<std::string> article::get_subscribers() {
	return subscribers[get_id()];
}

bool article::subscribe(int id, const std::string& product_name) {
	if (!exists(id)) {
		throw invalid_key(id);
	}
	return subscribers[id].insert(product_name).second;
}

#endif // ARTICLE_HEADER
