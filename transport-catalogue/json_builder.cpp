#include "json_builder.h"

namespace json {
Builder::Builder() {
	nodes_.push_back("first");
}

Builder::KeyItemContext Builder::Key(std::string s) {
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

Builder::DictItemContext Builder::StartDict() {
	if (prepared_) {
		throw std::logic_error("Already Build");
	}
	is_key_ = false;
	where_am_i_.push_back(DICT);
	nodes_stack_.push_back(&nodes_.back());

	return { *this };
}

Builder& Builder::EndDict() {
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

	return Value(d);
}

Builder::ArrayItemContext Builder::StartArray() {
	if (prepared_) {
		throw std::logic_error("Already Build");
	}
	where_am_i_.push_back(ARRAY);
	nodes_stack_.push_back(&nodes_.back());

	return { *this };
}

Builder& Builder::EndArray() {
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
}

Node Builder::Build() {
	if (!nodes_stack_.empty() || !keys_.empty() || nodes_.size() != 2 || prepared_ || !where_am_i_.empty()) {
		throw std::logic_error("Build error");
	}
	prepared_ = true;
	return nodes_.back();
}

} // end of namespase json