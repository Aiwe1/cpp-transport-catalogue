#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <typeinfo>

namespace json {

class Node;
// ��������� ���������� Dict � Array ��� ���������
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;
using Number = std::variant<double, int>;

// ��� ������ ������ ������������� ��� ������� �������� JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
public:
    using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
    /* ���������� Node, ��������� std::variant */
    Node() {};
    Node(std::nullptr_t v) : value_(v) {};
    Node(Array v) : value_(v) {};
    Node(Dict v) : value_(v) {};
    Node(double v) : value_(v) {};
    Node(int v) : value_(v) {};
    Node(bool v) : value_(v) {};
    Node(std::string v) : value_(v) {};

    //const Array& AsArray() const;
    //const Dict& AsMap() const;
    //int AsInt() const;
    //const std::string& AsString() const;
    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const; //.���������� �������� ���� double, ���� ������ �������� double ���� int.� ��������� ������ ������������ ���������� � double ��������.
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;

    const Value& GetValue() const { return value_; }

    bool IsInt() const;
    bool IsDouble() const; //���������� true, ���� � Node �������� int ���� double.
    bool IsPureDouble() const; //���������� true, ���� � Node �������� double.
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;

    bool operator==(const Node& n) const;
    bool operator!=(const Node& n) const;
private:
    Value value_;
};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

    bool operator==(const Document& d) const;
    bool operator!=(const Document& d) const;
private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

}  // namespace json