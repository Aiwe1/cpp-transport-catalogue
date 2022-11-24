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

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
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
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    }
    else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
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
            // Сначала пробуем преобразовать строку в int
            try {
                return std::stoi(parsed_num);
            }
            catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return std::stod(parsed_num);
    }
    catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}
// Считывает содержимое строкового литерала JSON-документа
// Функцию следует использовать после считывания открывающего символа ":
std::string LoadString(std::istream& input) {
    using namespace std::literals;

    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        }
        else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
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
                // Встретили неизвестную escape-последовательность
                throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        }
        else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        }
        else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
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
    for (int i = 0; i < 3; ++i) { ///   3 = size of "ull" s
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
    if (s == "true"s) {
        return Node(true);
    }
    s.push_back(static_cast<char>(input.get()));
    if (s == "false"s) {
        return Node(false);
    }

    throw ParsingError("Failed to read bool from stream"s);
}

Node LoadNode(istream& input) {
    char c;
    input >> c;

    if (c == '[') {
        return LoadArray(input);
    }
    if (c == '{') {
        return LoadDict(input);
    }
    if (c == '"') {
        return LoadStringN(LoadString(input));
    }
    if (c == 'n') {
        return LoadNull(input);
    }
    if (c == 't' || c == 'f') {
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

int Node::AsInt() const {
    if (!IsInt()) {
        throw std::logic_error("Not an int"s);
    }
    return std::get<int>(*this);
}
bool Node::AsBool() const {
    if (!IsBool()) {
        throw std::logic_error("Not an bool"s);
    }
    return std::get<bool>(*this);
}
double Node::AsDouble() const {
    if (IsInt()) {
        return static_cast<double>(std::get<int>(*this));
    }
    if (IsDouble()) {
        return std::get<double>(*this);
    }
    throw std::logic_error("Not an int or double"s);
} //.Возвращает значение типа double, если внутри хранится double либо int.В последнем случае возвращается приведённое в double значение.
const std::string& Node::AsString() const {
    if (!IsString()) {
        throw std::logic_error("Not an string"s);
    }
    return std::get<string>(*this);
}
const Array& Node::AsArray() const {
    if (!IsArray()) {
        throw std::logic_error("Not an array"s);
    }
    return std::get<Array>(*this);
}
const Dict& Node::AsMap() const {
    if (!IsMap()) {
        throw std::logic_error("Not an map"s);
    }
    return std::get<Dict>(*this);
}

bool Node::IsInt() const {
    return std::holds_alternative<int>(*this);
}
bool Node::IsDouble() const {
    return std::holds_alternative<int>(*this) || std::holds_alternative<double>(*this);
}
bool Node::IsPureDouble() const {
    return std::holds_alternative<double>(*this);
} 
bool Node::IsBool() const {
    return std::holds_alternative<bool>(*this);
}
bool Node::IsString() const {
    return std::holds_alternative<std::string>(*this);
}
bool Node::IsNull() const {
    return std::holds_alternative<std::nullptr_t>(*this);
}
bool Node::IsArray() const {
    return std::holds_alternative<Array>(*this);
}
bool Node::IsMap() const {
    return std::holds_alternative<Dict>(*this);
}

bool Document::operator==(const Document& d) const {
    return (d.GetRoot() == this->GetRoot());
}
bool Document::operator!=(const Document& d) const {
    return (d.GetRoot() != this->GetRoot());
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
}// Перегрузка функции PrintValue для вывода значений null
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
}

void PrintNode(const Node& node, std::ostream& out) {
    std::visit([&out](const auto& value) { PrintValue(value, out); },
           node);
} 

void Print(const Document& doc, std::ostream& out) {
    PrintNode(doc.GetRoot(), out);
}

}  // namespace json