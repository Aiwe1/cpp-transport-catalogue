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

class Node final : public std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
public:
    using variant::variant;
    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const; //.���������� �������� ���� double, ���� ������ �������� double ���� int.� ��������� ������ ������������ ���������� � double ��������.
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;

    bool IsInt() const;
    bool IsDouble() const; //���������� true, ���� � Node �������� int ���� double.
    bool IsPureDouble() const; //���������� true, ���� � Node �������� double.
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;
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