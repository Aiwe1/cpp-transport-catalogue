#include "json_builder.h"
#include <algorithm>

namespace json {
Builder::Builder() {
	nodes_.push_back("first");
}

Builder::KeyItemContext Builder::Key(std::string s) {
	keys_.push_back(s);

	return { *this };
}

Builder::DictItemContext Builder::StartDict() {
	nodes_stack_.push_back(&nodes_.back());

	return { *this };
}

Builder& Builder::EndDict() {
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
	nodes_stack_.push_back(&nodes_.back());

	return { *this };
}

Builder& Builder::EndArray() {
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
	if (!nodes_stack_.empty() || !keys_.empty() || nodes_.size() != 2) {
		throw std::logic_error("Build error");
	}
	return nodes_.back();
}

} // end of namespase json