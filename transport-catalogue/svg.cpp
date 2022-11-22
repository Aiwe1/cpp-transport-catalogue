#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center) {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius) {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    // Выводим атрибуты, унаследованные от PathProps
    RenderAttrs(context.out);
    out << "/>"sv;
}
// ---------- Polyline ------------

Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}
void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    // <polyline points="0,100 50,25 50,75 100,0" />
    out << "<polyline points=\""sv;

    for (auto it = points_.begin(); it != points_.end(); ++it) {
        out << it->x << ',' << it->y;
        if (it + 1 != points_.end()) {
            out << ' ';
        }
    }
    // Выводим атрибуты, унаследованные от PathProps
    //out << "\" />"sv;
    out << "\""sv;
    RenderAttrs(context.out);
    out << " />"sv;
}

//----------- Text ---------

Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}
// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}
// Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size) {
    size_ = size;
    return *this;
}
// Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}
// Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}
// Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(std::string data) {
    //Например, текст “Hello, <UserName>.Would you like some "M&M's" ? ” в SVG файле будет представлен так :
    //<text>Hello, &lt;UserName&gt;. Would you like some &quot;M&amp;M&apos;s&quot;?</text>

    for (auto c : data) {
        switch (c)
        {
        case '"':
            data_ += "&quot;"s;
            break;
        case '\'':
            data_ += "&apos;"s;
            break;
        case '<':
            data_ += "&lt;"s;
            break;
        case '>':
            data_ += "&gt;"s;
            break;
        case '&':
            data_ += "&amp;"s;
            break;
        default:
            data_.push_back(c);
        }
    }

    return *this;
}
void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    // <text x="35" y="20" dx="0" dy="6" font-size="12" font-family="Verdana" font-weight="bold">Hello C++</text>
    out << "<text x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y
        << "\" font-size=\""sv << size_;

    if (font_family_.size() > 0) {
        out << "\" font-family=\""sv << font_family_;
    }
    if (font_weight_.size() > 0) {
        out << "\" font-weight=\""sv << font_weight_;
    }
    out << "\""sv;
    RenderAttrs(context.out);
    //out  << "\">"sv << data_ << "</text>"sv;
    out << ">"sv << data_ << "</text>"sv;
}

// ---------- Doc ----------
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.emplace_back(std::move(obj));
}

void Document::Render(std::ostream& out) const {
    RenderContext ctx(out, 2, 2);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;

    for (const auto& obj : objects_) {
        obj->Render(ctx);
    }
    out << "</svg>"sv;
}

}  // namespace svg