#ifndef FIELD_HEADER
#define FIELD_HEADER

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>

#include <rapidjson/document.h>

namespace json = rapidjson;

/**
 *  Forward declaration of the class to allow use it on the top level namespace functions to overload
 *  operators for input and output streams.
 */
namespace models {
	template<typename Type> class field;
}

/**
 *  Operator overloading to allow putting the internal value of the field in an output stream.
 *
 *  @type  Type aims to be the data type of the internal value of the field.
 *  @param std::ostream& Reference to the standard output stream.
 *  @param models::field<Type>& Reference to a generic field.
 *  @returns std::ostream& Reference to an output stream to allow chaining output.
 */
template<typename Type>
std::ostream& operator<<(std::ostream&, models::field<Type>&);

/**
 *  Operator overloading to allow getting the internal value of the field from an input stream.
 *
 *  @type  Type aims to be the data type of the internal value of the field.
 *  @param std::istream& Reference to the standard input stream.
 *  @param models::field<Type>& Reference to a generic field.
 *  @returns std::istream& Reference to an input stream to allow chaining input.
 */
template<typename Type>
std::istream& operator>>(std::istream&, models::field<Type>&);

namespace models {
	/**
	 *  Template class to model a field based on a JSON entry.
	 *
	 *  @type Type ype aims to be the data type of the internal value of the field.
	 */
	template<typename Type>
	class field {
	public:
		/**
		 *  Type definition for a function reference/pointer used to be the getter.
		 */
		typedef std::function<Type(json::Value&)> Getter;

		/**
		 *  Type definition for a function reference/pointer used to be the setter.
		 */
		typedef std::function<void(json::Value&, const Type&)> Setter;

		/**
		 *  Constructor based on the key of the field within the JSON node and to use default
		 *  getters and setters.
		 *
		 *  @param const std::string key Label for the field withing the JSON node.
		 */
		field(const std::string&);

		/**
		 *  Constructor based on the key of the field within the JSON node and to use custom
		 *  getters and setters.
		 *
		 *  @param const std::string& String reference for key Label for the field withing the JSON node.
		 *  @param const Getter& Reference to a function to be used as getter.
		 *  @param const Setter& Reference to a function to be used as setter.
		 */
		field(const std::string&, const Getter&, const Setter&);

		/**
		 *  Default getter to extract a value from JSON node and put in the internal value. For this
		 *  particular project de default behaviour is take an string from the JSON node and parse
		 *  (if needed) to the parameter type.
		 *
		 *  @param json::Value& Reference to the JSON node.
		 *  @returns Type Parsed internal value got it from the JSON node.
		 */
		Type default_getter(json::Value&);

		/**
		 *  Default setter to assign the current interal value to JSON node. For this particular project
		 *  de default behaviour is convert (if needed) the internal value to an string and put it on the
		 *  JSON entry labeled with the key.
		 *
		 *  @param json::Value& Reference to the JSON node.
		 *  @param const Type& Value to put in the JSON node.
		 *  @returns void
		 */
		void default_setter(json::Value&, const Type&);

		/**
		 *  The actual exposed get function which calls to getter (either default or custom).
		 *
		 *  @param json::Value& Reference to the JSON node.
		 *  @returns Type Parsed internal value got it from the JSON node.
		 */
		Type get(json::Value&);

		/**
		 *  The actual exposed set function which calls to setter (either default or custom).
		 *
		 *  @param json::Value& Reference to the JSON node.
		 *  @returns void
		 */
		void set(json::Value&);

		/**
		 *  Operator to assign a new internal value in syntactical-friendly way.
		 *
		 *  @param const Type& Reference to value as the same type of the internal one.
		 *  @returns field<Type>& A reference to current object.
		 */
		field<Type>& operator=(const Type&);

		/**
		 *  Operator to retrieve the internal value in syntactical-friendly way.
		 *
		 *  @returns Type The current internal value.
		 */
		operator Type();

		/**
		 *  Frienship to the top level function to interact with an output stream.
		 *  This is required since the output stream may need access to read private
		 *  and/or protected members.
		 */
		friend std::ostream& ::operator<<(std::ostream&, field<Type>&);

		/**
		 *  Frienship to the top level function to interact with an input stream.
		 *  This is required since the input stream may need access to write private
		 *  and/or protected members.
		 */
		friend std::istream& ::operator>>(std::istream&, field<Type>&);

	private:
		/**
		 *  String for the label/key of the field within the JSON node.
		 */
		std::string key;

		/**
		 *  Internal value of the field.
		 */
		Type value;

		/**
		 *  Final(custom or default) getter.
		 */
		Getter getter;

		/**
		 *  Final (custom or default) setter.
		 */
		Setter setter;
	};
}

/**
 *  Following lines are for the implementation which normally goes in a .cpp file. However, since the
 *  class is a template is better have the implementation in the header file to avoid compilation and
 *  linking errors due to missing reference to functions.
 */

using models::field;
template<typename Type>
std::ostream& operator<<(std::ostream& output, field<Type>& instance) {
	output << instance.value;
	return output;
}

template<typename Type>
std::istream& operator>>(std::istream& input, field<Type>& instance) {
	input >> instance.value;
	return input;
}

template<typename Type>
field<Type>::field(const std::string& key, const Getter& custom_getter, const Setter& custom_setter):
key(key), getter(custom_getter), setter(custom_setter) { };

template<typename Type>
field<Type>::field(const std::string& key): key(key),
getter(std::bind(&field<Type>::default_getter, this, std::placeholders::_1)),
setter(std::bind(&field<Type>::default_setter, this, std::placeholders::_1, std::placeholders::_2))
{ }

template<>
std::string field<std::string>::default_getter(json::Value& node) {
	value = node.GetString();
	return value;
}

template<>
int field<int>::default_getter(json::Value& node) {
	std::string text = node.GetString();
	value = std::stoi(text);
	return value;
}

template<typename Type>
void field<Type>::default_setter(json::Value& node, const Type& value) {
	std::string* buffer = new std::string(std::to_string(value));
	node = json::StringRef(buffer->c_str(), buffer->size());
}

template<>
void field<std::string>::default_setter(json::Value& node, const std::string& value) {
	node = json::StringRef(value.c_str(), value.size());
}

template<typename Type>
inline Type field<Type>::get(json::Value& node) { return value = getter(node[key.c_str()]); }

template<typename Type>
inline void field<Type>::set(json::Value& node) { setter(node[key.c_str()], value); }


template<typename Type>
field<Type>& field<Type>::operator=(const Type& value) {
	this->value = value;
	return *this;
}

template<typename Type>
inline field<Type>::operator Type() { return value; };

#endif // FIELD_HEADER
