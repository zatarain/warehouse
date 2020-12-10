#ifndef MODEL_HEADER
#define MODEL_HEADER

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

#include "field.hpp"

namespace json = rapidjson;

namespace models {
	template<typename PrimaryKey>
	class model {
	public:
		model() = delete;
		void fetch();
		void read();
		void write();
		void commit();
		std::set<PrimaryKey> get_all_keys();
		bool exists(const PrimaryKey&);
		bool read(const PrimaryKey&);
		bool write(const PrimaryKey&);

	protected:
		typedef field<PrimaryKey> primary_key;
		std::string source;
		std::string entry;
		std::string filename;
		json::Document document;
		std::map<PrimaryKey, json::Value> dataset;

		model(const std::string&);
		model(const std::string&, const std::string&);

		void parse();
		virtual primary_key& get_primary_key() = 0;
		virtual void operator<<(json::Value&) = 0;
		virtual void operator>>(json::Value&) = 0;

		std::string not_found(const PrimaryKey&);

		template<typename Field>
		void read(json::Value&, Field&);
		template<typename Field, typename... Arguments>
		void read(json::Value&, Field&, Arguments&...);

		template<typename Field>
		void write(json::Value&, Field&);
		template<typename Field, typename... Arguments>
		void write(json::Value&, Field&, Arguments&...);

	private:
		static const std::string path;
		static const std::string ds;
		static const std::string extension;
	};
}

using models::model;

template<typename PrimaryKey>
const std::string model<PrimaryKey>::path("data");
template<typename PrimaryKey>
const std::string model<PrimaryKey>::ds("/");
template<typename PrimaryKey>
const std::string model<PrimaryKey>::extension("json");

template<typename PrimaryKey>
model<PrimaryKey>::model(const std::string& source): model(source, source) { }

template<typename PrimaryKey>
model<PrimaryKey>::model(const std::string& source, const std::string& entry):
source(source), entry(entry) {
	std::stringstream filename;
	filename << path << ds << source << '.' << extension;
	this->filename = filename.str();
	parse();
}

template<typename PrimaryKey>
void model<PrimaryKey>::parse() {
	std::ifstream input(filename);
	json::IStreamWrapper reader(input);
	document.ParseStream(reader);
}

template<typename PrimaryKey>
void model<PrimaryKey>::fetch() {
	json::Value recordset;
	primary_key& key = get_primary_key();
	recordset = document[entry.c_str()];
	for (auto& node: recordset.GetArray()) {
		read(node, key);
		dataset[key] = node;
	}
}

template<typename PrimaryKey>
std::string model<PrimaryKey>::not_found(const PrimaryKey& key) {
	std::stringstream error;
	error << "Record with key = '" << key << "' not found!";
	return error.str();
}

template<typename PrimaryKey>
void model<PrimaryKey>::read() {
	primary_key key = get_primary_key();
	if (!exists(key)) {
		throw not_found(key);
	}
	*this << dataset[key];
}

template<typename PrimaryKey>
template<typename Field>
inline void model<PrimaryKey>::read(json::Value& node, Field& field) { field.get(node); }

template<typename PrimaryKey>
template<typename Field, typename... Arguments>
void model<PrimaryKey>::read(json::Value& node, Field& field, Arguments&... arguments) {
	field.get(node);
	read(node, arguments...);
}

template<typename PrimaryKey>
void model<PrimaryKey>::commit() {
	json::Value list(json::kArrayType);
	json::Document::AllocatorType& allocator = document.GetAllocator();
	for (auto& [id, node]: dataset) {
		list.PushBack(node, allocator);
	}

	document[entry.c_str()] = list;
	std::ofstream output(filename);
	json::OStreamWrapper wrapper(output);
	json::Writer<json::OStreamWrapper> writer(wrapper);
	document.Accept(writer);
}

template<typename PrimaryKey>
void model<PrimaryKey>::write() {
	primary_key key = get_primary_key();
	if (!exists(key)) {
		throw not_found(key);
	}
	*this >> dataset[key];
}

template<typename PrimaryKey>
template<typename Field>
inline void model<PrimaryKey>::write(json::Value& node, Field& field) { field.set(node); }

template<typename PrimaryKey>
template<typename Field, typename... Arguments>
void model<PrimaryKey>::write(json::Value& node, Field& field, Arguments&... arguments) {
	field.set(node);
	write(node, arguments...);
}

template<typename PrimaryKey>
std::set<PrimaryKey> model<PrimaryKey>::get_all_keys() {
	std::set<PrimaryKey> keys;
	auto key_extractor = [](const std::pair<const PrimaryKey, json::Value>& entry) {
		return entry.first;
	};
	std::transform(
		dataset.begin(), dataset.end(),
		std::inserter(keys, keys.begin()),
		key_extractor
	);
	return keys;
}

template<typename PrimaryKey>
inline bool model<PrimaryKey>::exists(const PrimaryKey& key) { return dataset.count(key); }

template<typename PrimaryKey>
bool model<PrimaryKey>::read(const PrimaryKey& key) {
	if (!exists(key)) return false;

	primary_key& primary = get_primary_key();
	primary = key;
	read();
	return true;
}

template<typename PrimaryKey>
bool model<PrimaryKey>::write(const PrimaryKey& key) {
	return true;
}

#endif // MODEL_HEADER
