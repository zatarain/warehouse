#ifndef MODEL_HEADER
#define MODEL_HEADER

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

#include "field.hpp"

namespace json = rapidjson;

namespace models {
	struct case_insensitive_less {
		bool operator()(const unsigned char&, const unsigned char&) const;
		bool operator()(const std::string&, const std::string&) const;
	};

	template<typename PrimaryKey, typename Comparator = std::less<PrimaryKey>>
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
		std::map<PrimaryKey, json::Value, Comparator> dataset;

		model(const std::string&);
		model(const std::string&, const std::string&);

		void parse();
		virtual primary_key& get_primary_key() = 0;
		virtual void operator<<(json::Value&) = 0;
		virtual void operator>>(json::Value&) = 0;

		std::invalid_argument invalid_key(const PrimaryKey&) const;

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

bool models::case_insensitive_less::operator()(const unsigned char& a, const unsigned char& b) const {
	return std::tolower(a) < std::tolower(b);
}

bool models::case_insensitive_less::operator()(const std::string& a, const std::string& b) const {
	return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), *this);
}

template<typename PrimaryKey, typename Comparator>
const std::string model<PrimaryKey, Comparator>::path("data");
template<typename PrimaryKey, typename Comparator>
const std::string model<PrimaryKey, Comparator>::ds("/");
template<typename PrimaryKey, typename Comparator>
const std::string model<PrimaryKey, Comparator>::extension("json");

template<typename PrimaryKey, typename Comparator>
model<PrimaryKey, Comparator>::model(const std::string& source): model(source, source) { }

template<typename PrimaryKey, typename Comparator>
model<PrimaryKey, Comparator>::model(const std::string& source, const std::string& entry):
source(source), entry(entry) {
	std::stringstream filename;
	filename << path << ds << source << '.' << extension;
	this->filename = filename.str();
	parse();
}

template<typename PrimaryKey, typename Comparator>
void model<PrimaryKey, Comparator>::parse() {
	std::ifstream input(filename);
	json::IStreamWrapper reader(input);
	document.ParseStream(reader);
}

template<typename PrimaryKey, typename Comparator>
void model<PrimaryKey, Comparator>::fetch() {
	json::Value recordset;
	primary_key& key = get_primary_key();
	recordset = document[entry.c_str()];
	for (auto& node: recordset.GetArray()) {
		read(node, key);
		dataset[key] = node;
	}
}

template<typename PrimaryKey, typename Comparator>
std::invalid_argument model<PrimaryKey, Comparator>::invalid_key(const PrimaryKey& key) const {
	std::stringstream message;
	message << "Record with key = '" << key << "' not found!";
	return std::invalid_argument(message.str());
}

template<typename PrimaryKey, typename Comparator>
void model<PrimaryKey, Comparator>::read() {
	primary_key key = get_primary_key();
	if (!exists(key)) {
		throw invalid_key(key);
	}
	*this << dataset[key];
}

template<typename PrimaryKey, typename Comparator>
template<typename Field>
inline void model<PrimaryKey, Comparator>::read(json::Value& node, Field& field) { field.get(node); }

template<typename PrimaryKey, typename Comparator>
template<typename Field, typename... Arguments>
void model<PrimaryKey, Comparator>::read(json::Value& node, Field& field, Arguments&... arguments) {
	field.get(node);
	read(node, arguments...);
}

template<typename PrimaryKey, typename Comparator>
void model<PrimaryKey, Comparator>::commit() {
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

template<typename PrimaryKey, typename Comparator>
void model<PrimaryKey, Comparator>::write() {
	primary_key key = get_primary_key();
	if (!exists(key)) {
		throw invalid_key(key);
	}
	*this >> dataset[key];
}

template<typename PrimaryKey, typename Comparator>
template<typename Field>
inline void model<PrimaryKey, Comparator>::write(json::Value& node, Field& field) { field.set(node); }

template<typename PrimaryKey, typename Comparator>
template<typename Field, typename... Arguments>
void model<PrimaryKey, Comparator>::write(json::Value& node, Field& field, Arguments&... arguments) {
	field.set(node);
	write(node, arguments...);
}

template<typename PrimaryKey, typename Comparator>
std::set<PrimaryKey> model<PrimaryKey, Comparator>::get_all_keys() {
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

template<typename PrimaryKey, typename Comparator>
inline bool model<PrimaryKey, Comparator>::exists(const PrimaryKey& key) { return dataset.count(key); }

template<typename PrimaryKey, typename Comparator>
bool model<PrimaryKey, Comparator>::read(const PrimaryKey& key) {
	if (!exists(key)) return false;

	primary_key& primary = get_primary_key();
	primary = key;
	read();
	return true;
}

template<typename PrimaryKey, typename Comparator>
bool model<PrimaryKey, Comparator>::write(const PrimaryKey& key) {
	return true;
}

#endif // MODEL_HEADER
