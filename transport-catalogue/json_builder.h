#pragma once

#include "json.h"

#include <vector>
#include <list>

namespace json {

class Builder {
public:
	Builder() {
		nodes_.push_back("first");
	}
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
	KeyItemContext Key(std::string s) {
		if (prepared_) {
			throw std::logic_error("Already Build");
		}
		if (where_am_i_.empty() || where_am_i_.back() != DICT || is_key_) {
			throw std::logic_error("Key");
		}
		is_key_ = true;
		keys_.push_back(s);

		return { *this };
	}

	DictItemContext StartDict() {
		if (prepared_) {
			throw std::logic_error("Already Build");
		}
		is_key_ = false;
		where_am_i_.push_back(DICT);
		nodes_stack_.push_back(&nodes_.back());

		return { *this };
	}

	Builder& EndDict() {
		if (prepared_) {
			throw std::logic_error("Already Build");
		}
		if (where_am_i_.empty() || where_am_i_.back() != DICT) {
			throw std::logic_error("Not Dict");
		}
		where_am_i_.pop_back();
		Dict d;
		while (&nodes_.back() != nodes_stack_.back()) {
			d.insert({ keys_.back(), nodes_.back() });
			nodes_.pop_back();
			keys_.pop_back();
		}
		nodes_stack_.pop_back();
		//std::reverse(d.begin(), d.end());
		return Value(d);
	}

	ArrayItemContext StartArray() {
		if (prepared_) {
			throw std::logic_error("Already Build");
		}
		where_am_i_.push_back(ARRAY);
		nodes_stack_.push_back(&nodes_.back());

		return { *this };
	}
	Builder& EndArray() {
		if (prepared_) {
			throw std::logic_error("Already Build");
		}
		if (where_am_i_.empty() || where_am_i_.back() != ARRAY) {
			throw std::logic_error("Not Array");
		}
		where_am_i_.pop_back();
		Array a;

		while (&nodes_.back() != nodes_stack_.back()) {
			a.push_back(nodes_.back());
			nodes_.pop_back();
		}
		nodes_stack_.pop_back();
		std::reverse(a.begin(), a.end());
		return Value(a);
		//return *this;
	}
	Node Build() {
		if (!nodes_stack_.empty() || !keys_.empty() || nodes_.size() != 2 || prepared_ || !where_am_i_.empty()) {
			throw std::logic_error("Build error");
		}
		prepared_ = true;
		return nodes_.back();
	}

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

	Node root_;

	std::vector<Node*> nodes_stack_;
	std::list<Node> nodes_;
	std::list<std::string> keys_;
};

} // end of namespace json