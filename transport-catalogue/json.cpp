#include "json.h"

using namespace std;

namespace json {

namespace {

Node LoadNode(istream& input);

Node LoadArray(istream& input) {
    Array result;
    
    if (input.eof())
        throw ParsingError("Failed to read array from stream"s);
    char c;
    for (; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }
    if (c != ']')
        throw ParsingError("Failed to read array from stream"s);
    return Node{ result };
}

Number LoadNumber(std::istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    // ��������� � parsed_num ��������� ������ �� input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // ��������� ���� ��� ����� ���� � parsed_num �� input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // ������ ����� ����� �����
    if (input.peek() == '0') {
        read_char();
        // ����� 0 � JSON �� ����� ���� ������ �����
    }
    else {
        read_digits();
    }

    bool is_int = true;
    // ������ ������� ����� �����
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // ������ ���������������� ����� �����
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // ������� ������� ������������� ������ � int
            try {
                return std::stoi(parsed_num);
            }
            catch (...) {
                // � ������ �������, ��������, ��� ������������,
                // ��� ���� ��������� ������������� ������ � double
            }
        }
        return std::stod(parsed_num);
    }
    catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}
// ��������� ���������� ���������� �������� JSON-���������
// ������� ������� ������������ ����� ���������� ������������ ������� ":
std::string LoadString(std::istream& input) {
    using namespace std::literals;

    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // ����� ���������� �� ����, ��� ��������� ����������� �������?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // ��������� ����������� �������
            ++it;
            break;
        }
        else if (ch == '\\') {
            // ��������� ������ escape-������������������
            ++it;
            if (it == end) {
                // ����� ���������� ����� ����� ������� �������� ����� �����
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // ������������ ���� �� �������������������: \\, \n, \t, \r, \"
            switch (escaped_char) {
            case 'n':
                s.push_back('\n');
                break;
            case 't':
                s.push_back('\t');
                break;
            case 'r':
                s.push_back('\r');
                break;
            case '"':
                s.push_back('"');
                break;
            case '\\':
                s.push_back('\\');
                break;
            default:
                // ��������� ����������� escape-������������������
                throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        }
        else if (ch == '\n' || ch == '\r') {
            // ��������� ������� ������- JSON �� ����� ����������� ��������� \r ��� \n
            throw ParsingError("Unexpected end of line"s);
        }
        else {
            // ������ ��������� ��������� ������ � �������� ��� � �������������� ������
            s.push_back(ch);
        }
        ++it;
    }

    return s;
}

Node LoadStringN(std::string input) {
    return Node(move(input));
}
Node LoadDict(istream& input) {
    Dict result;
    if (input.eof())
        throw ParsingError("Failed to read map from stream"s);
    char c;
    for (; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input);
        input >> c;
        result.insert({ move(key), LoadNode(input) });
    }
    if (c != '}')
        throw ParsingError("Failed to read map from stream"s);
    return Node(move(result));
}
Node LoadNull(istream& input) {
    std::string s;
    for (int i = 0; i < 3; ++i) {
        if (!input) {
            throw ParsingError("Failed to read null from stream"s);
        }
        s.push_back(static_cast<char>(input.get()));
    }
    if (s == "ull"s)
        return Node(nullptr);
    
    throw ParsingError("Failed to read null from stream"s);
    
}
Node LoadBool(istream& input) {
    std::string s;
    for (int i = 0; i < 4; ++i) {
        if (!input) {
            throw ParsingError("Failed to read bool from stream"s);
        }
        s.push_back(static_cast<char>(input.get()));
    }
    if (s == "true"s)
        return Node(true);
    s.push_back(static_cast<char>(input.get()));
    if (s == "false"s)
        return Node(false);

    throw ParsingError("Failed to read bool from stream"s);
}

Node LoadNode(istream& input) {
    char c;
    input >> c;

    if (c == '[') {
        return LoadArray(input);
    }
    else if (c == '{') {
        return LoadDict(input);
    }
    else if (c == '"') {
        return LoadStringN(LoadString(input));
    }
    else if (c == 'n') {
        return LoadNull(input);
    }
    else if (c == 't' || c == 'f') {
        input.putback(c);
        return LoadBool(input);
    }
    else {
        input.putback(c); 

        Number Num = LoadNumber(input);
        if (std::holds_alternative<double>(Num))
            return Node{ std::get<double>(Num)};
        if (std::holds_alternative<int>(Num))
            return Node{ std::get<int>(Num) };
    }
    return Node{ nullptr };
}

}  // namespace
/*
Node::Node(Array array)
    : as_array_(move(array)) {
}

Node::Node(Dict map)
    : as_map_(move(map)) {
}

Node::Node(int value)
    : as_int_(value) {
}

Node::Node(string value)
    : as_string_(move(value)) {
}

const Array& Node::AsArray() const {
    return as_array_;
}

const Dict& Node::AsMap() const {
    return as_map_;
}

int Node::AsInt() const {
    return as_int_;
}

const string& Node::AsString() const {
    return as_string_;
} */
int Node::AsInt() const {
    if (std::holds_alternative<int>(value_))
        return std::get<int>(value_);

    throw std::logic_error("type");
}
bool Node::AsBool() const {
    if (std::holds_alternative<bool>(value_))
        return std::get<bool>(value_);

    throw std::logic_error("type");
}
double Node::AsDouble() const {
    if (std::holds_alternative<int>(value_))
        return static_cast<double>(std::get<int>(value_));
    if (std::holds_alternative<double>(value_))
        return std::get<double>(value_);

    throw std::logic_error("type");
} //.���������� �������� ���� double, ���� ������ �������� double ���� int.� ��������� ������ ������������ ���������� � double ��������.
const std::string& Node::AsString() const {
    if (std::holds_alternative<std::string>(value_))
        return std::get<std::string>(value_);

    throw std::logic_error("type");
}
const Array& Node::AsArray() const {
    if (std::holds_alternative<Array>(value_))
        return std::get<Array>(value_);

    throw std::logic_error("type");
}
const Dict& Node::AsMap() const {
    if (std::holds_alternative<Dict>(value_))
        return std::get<Dict>(value_);

    throw std::logic_error("type");
}

bool Node::IsInt() const {
    if (std::holds_alternative<int>(value_))
        return true;
    return false;
}
bool Node::IsDouble() const {
    if (std::holds_alternative<int>(value_) || std::holds_alternative<double>(value_))
        return true;   //���������� true, ���� � Node �������� int ���� double.
    return false;
}
bool Node::IsPureDouble() const {
    if (std::holds_alternative<double>(value_))
        return true;    //���������� true, ���� � Node �������� double.
    return false;
} 
bool Node::IsBool() const {
    if (std::holds_alternative<bool>(value_))
        return true;
    return false;
}
bool Node::IsString() const {
    if (std::holds_alternative<std::string>(value_))
        return true;
    return false;
}
bool Node::IsNull() const {
    if (std::holds_alternative<std::nullptr_t>(value_))
        return true;
    return false;
}
bool Node::IsArray() const {
    if (std::holds_alternative<Array>(value_))
        return true;
    return false;
}
bool Node::IsMap() const {
    if (std::holds_alternative<Dict>(value_))
        return true;
    return false;
}

bool Node::operator==(const Node& n) const {
    if (n.GetValue() == this->GetValue())
        return true;
    return false;
}
bool Node::operator!=(const Node& n) const {
    if (n.GetValue() != this->GetValue())
        return true;
    return false;
}

bool Document::operator==(const Document& d) const {
    if (d.GetRoot() == this->GetRoot())
        return true;
    return false;
}
bool Document::operator!=(const Document& d) const {
    if (d.GetRoot() != this->GetRoot())
        return true;
    return false;
}

Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{ LoadNode(input) };
}

void PrintNode(const Node& node, std::ostream& out);
template <typename Value>
void PrintValue(const Value& value, std::ostream& out) {
    out << std::boolalpha;
    out << value;
}// ���������� ������� PrintValue ��� ������ �������� null
void PrintValue(std::nullptr_t, std::ostream& out) {
    out << "null"sv;
}
void PrintValue(const Array& a, std::ostream& out) {
    out << '[' << endl;
    for (auto it = a.begin(); it != a.end();) 
    {
        PrintNode(*it, out);
        ++it;
        if (it == a.end())
            break;
        out << ", "s;
    }
    out << endl << ']';
}
void PrintValue(const std::string& s, std::ostream& out) {
    using namespace std::literals;
    std::string s1;
    s1.push_back('\"');
    for (auto ch : s) {
        switch (ch) {
        case '"':
            s1.push_back('\\');
            s1.push_back('\"');
            break;
        case '\\':
            s1.push_back('\\');
            s1.push_back('\\');
            break;
        case '\n':
            s1.push_back('\\');
            s1.push_back('n');
            break;
        //case '\t':
           // s1.push_back('\\');
           // s1.push_back('t');
           // break;
        case '\r':
            s1.push_back('\\');
            s1.push_back('r');
            break;
        default:
            s1.push_back(ch);
        }
    }
    s1.push_back('\"');
    out << s1;
    return;
}
void PrintValue(const Dict& d, std::ostream& out) {
    out << '{' << endl;
    for (auto it = d.begin(); it != d.end();)
    {
        PrintValue(it->first, out);
        out << ": "s;
        PrintNode(it->second, out);
        ++it;
        if (it == d.end())
            break;
        out << ", "s;
    }
    out << endl << '}';
    /*
    for (const auto& [s, n] : d) {
        PrintValue(s, out);
        PrintNode(n, out);
    } */
}

void PrintNode(const Node& node, std::ostream& out) {
    std::visit(
        [&out](const auto& value) { PrintValue(value, out); },
        node.GetValue());
}

void Print(const Document& doc, std::ostream& out) {
    PrintNode(doc.GetRoot(), out);
}

}  // namespace json