#pragma once

#include "json.h"

#include <vector>
#include <list>

namespace json {

class Builder {
public:
	Builder();
	class DictItemContext;
	class ArrayItemContext;
	class KeyItemContext;
	class ValueKeyItemContext;
	class ValueArrayItemContext;
	class KeyItemContext {
	public:
		KeyItemContext(Builder& b) : b_(b) {}

		DictItemContext StartDict() {
			return b_.StartDict();
		}
		ArrayItemContext StartArray() {
			return b_.StartArray();
		}
		template <typename T>
		ValueKeyItemContext Value(T v) {
			return b_.Value(v);
		}
	private:
		Builder& b_;
	};
	class ValueKeyItemContext {
	public:
		ValueKeyItemContext(Builder& b) : b_(b) {}

		Builder& EndDict() {
			return b_.EndDict();
		}
		KeyItemContext Key(std::string s) {
			return b_.Key(s);
		}
	private:
		Builder& b_;
	};
	class DictItemContext {
	public:
		DictItemContext(Builder& b) : b_(b) {}

		Builder& EndDict() {
			return b_.EndDict();
		}
		KeyItemContext Key(std::string s) {
			return b_.Key(s);
		}
	private:
		Builder& b_;
	};
	class ValueArrayItemContext {
	public:
		ValueArrayItemContext(Builder& b) : b_(b) {}

		Builder& EndArray() {
			return b_.EndArray();
		}
		DictItemContext StartDict() {
			return b_.StartDict();
		}
		ArrayItemContext StartArray() {
			return b_.StartArray();
		}
		template <typename T>
		ValueArrayItemContext Value(T v) {
			return { b_.Value(v) };
		}
	private:
		Builder& b_;
	};
	class ArrayItemContext {
	public:
		ArrayItemContext(Builder& b) : b_(b) {}

		Builder& EndArray() {
			return b_.EndArray();
		}
		DictItemContext StartDict() {
			return b_.StartDict();
		}
		ArrayItemContext StartArray() {
			return b_.StartArray();
		}
		template <typename T>
		ValueArrayItemContext Value(T v) {
			return b_.Value(v);
		}
	private:
		Builder& b_;
	};

	template <typename T>
	Builder& Value(T v) {
		if (prepared_) {
			throw std::logic_error("Already Build");
		}
		if (!where_am_i_.empty() && where_am_i_.back() == DICT) {
			is_key_ = false;
		}
		nodes_.emplace_back(v);
		//nodes_stack_.emplace_back(&(nodes.emplace_back(v)));
		return *this;
	}
	KeyItemContext Key(std::string s);

	DictItemContext StartDict();

	Builder& EndDict();

	ArrayItemContext StartArray();

	Builder& EndArray();

	Node Build();
private:
	enum Where_Am_I {
		START,
		KEY,
		ARRAY,
		DICT
	};
	std::vector<Where_Am_I>where_am_i_;
	bool prepared_ = false;
	bool is_key_ = true;

	std::vector<Node*> nodes_stack_;
	std::list<Node> nodes_;
	std::list<std::string> keys_;
};

} // end of namespace json