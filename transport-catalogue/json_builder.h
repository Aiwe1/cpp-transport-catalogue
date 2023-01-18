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
	class ItemContext {
	public:
		ItemContext(Builder& b) : b_(b) {}
		DictItemContext StartDict() {
			return b_.StartDict();
		}
		ArrayItemContext StartArray() {
			return b_.StartArray();
		}
		Builder& EndDict() {
			return b_.EndDict();
		}
		Builder& EndArray() {
			return b_.EndArray();
		}
		KeyItemContext Key(std::string s) {
			return b_.Key(s);
		}
	protected:
		Builder& b_;
	};
	class KeyItemContext : public ItemContext {
	public:
		KeyItemContext(Builder& b) : ItemContext(b) {}

		template <typename T>
		DictItemContext Value(T v) {
			return b_.Value(v);
		}
	};
	class DictItemContext : public ItemContext {
	public:
		DictItemContext(Builder& b) : ItemContext(b) {}
	};	
	class ArrayItemContext : public ItemContext {
	public:
		ArrayItemContext(Builder& b) : ItemContext(b) {}

		template <typename T>
		ArrayItemContext Value(T v) {
			return b_.Value(v);
		}
	};

	template <typename T>
	Builder& Value(T v) {
		nodes_.emplace_back(v);

		return *this;
	}
	KeyItemContext Key(std::string s);
	DictItemContext StartDict();
	Builder& EndDict();
	ArrayItemContext StartArray();
	Builder& EndArray();
	Node Build();
private:
	std::vector<Node*> nodes_stack_;
	std::list<Node> nodes_;
	std::list<std::string> keys_;
};

} // end of namespace json