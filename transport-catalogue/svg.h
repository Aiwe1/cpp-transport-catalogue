#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

namespace svg {
using namespace std::literals;
struct Rgb {
    Rgb() = default;
    Rgb(uint8_t r, uint8_t g, uint8_t b) :red(r), green(g), blue(b) {}
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};
struct Rgba {
    Rgba() = default;
    Rgba(uint8_t r, uint8_t g, uint8_t b, double o) :red(r), green(g), blue(b), opacity(o) {}
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

// ������� � ������������ ����� ��������� �� �������������� inline,
// �� ������� ���, ��� ��� ����� ����� �� ��� ������� ����������,
// ������� ���������� ���� ���������.
// � ��������� ������ ������ ������� ���������� ����� ������������ ���� ����� ���� ���������
inline const Color NoneColor{  };

struct PrintColor {
    std::ostream& out;

    void operator()(std::monostate) const {
        out << "none"s;
    }
    void operator()(std::string s) const {
        out << s;
    }
    void operator()(Rgb c) const {
        out << "rgb("sv << (int)c.red << ","sv << (int)c.green << "," << (int)c.blue << ")"sv;
    }
    void operator()(Rgba c) const {
        out << "rgba("sv << (int)c.red << ","sv << (int)c.green << "," << (int)c.blue << "," << c.opacity << ")"sv;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Color& col) {

    std::visit(PrintColor{ os }, col);
    return os;
}

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};
enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

inline std::ostream& operator<<(std::ostream& os, const StrokeLineCap& slc) {

    using namespace std::string_literals;
    switch (slc) {
    case (svg::StrokeLineCap::BUTT):
        os << "butt"s;
        break;
    case (svg::StrokeLineCap::ROUND):
        os << "round"s;
        break;
    case (svg::StrokeLineCap::SQUARE):
        os << "square"s;
        break;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& slj) {

    using namespace std::string_literals;
    switch (slj) {
    case (svg::StrokeLineJoin::ARCS):
        os << "arcs"s;
        break;
    case (svg::StrokeLineJoin::BEVEL):
        os << "bevel"s;
        break;
    case (svg::StrokeLineJoin::MITER):
        os << "miter"s;
        break;
    case (svg::StrokeLineJoin::MITER_CLIP):
        os << "miter-clip"s;
        break;
    case (svg::StrokeLineJoin::ROUND):
        os << "round"s;
        break;
    }
    return os;
}
struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

/*
 * ��������������� ���������, �������� �������� ��� ������ SVG-��������� � ���������.
 * ������ ������ �� ����� ������, ������� �������� � ��� ������� ��� ������ ��������
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return { out, indent_step, indent + indent_step };
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

/*
 * ����������� ������� ����� Object ������ ��� ���������������� ��������
 * ���������� ����� SVG-���������
 * ��������� ������� "��������� �����" ��� ������ ����������� ����
 */
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeWidth(double width) {
        stroke_width_ = std::move(width);
        return AsOwner();
    }
    Owner& SetStrokeLineCap(StrokeLineCap cap) {
        stroke_line_cap_ = std::move(cap);
        return AsOwner();
    }
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        stroke_line_join_ = std::move(line_join);
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_color_) {
            out << " fill=\""sv << *fill_color_ << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv << *stroke_color_ << "\""sv;
        }
        if (stroke_width_) {
            out << " stroke-width=\""sv << *stroke_width_ << "\""sv; ///
        }
        if (stroke_line_cap_) {
            out << " stroke-linecap=\""sv << *stroke_line_cap_ << "\""sv; ///
        }
        if (stroke_line_join_) {
            out << " stroke-linejoin=\""sv << *stroke_line_join_ << "\""sv; ///
        }
    }

private:
    Owner& AsOwner() {
        // static_cast ��������� ����������� *this � Owner&,
        // ���� ����� Owner � ��������� PathProps
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> stroke_line_cap_;
    std::optional<StrokeLineJoin> stroke_line_join_;
};

/*
 * ����� Circle ���������� ������� <circle> ��� ����������� �����
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

/*
 * ����� Polyline ���������� ������� <polyline> ��� ����������� ������� �����
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object, public PathProps<Polyline> {
public:
    // ��������� ��������� ������� � ������� �����
    Polyline& AddPoint(Point point);

private:
    void RenderObject(const RenderContext& context) const override;

    std::vector<Point> points_;
};

/*
 * ����� Text ���������� ������� <text> ��� ����������� ������
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text final : public Object, public PathProps<Text> {
public:
    // ����� ���������� ������� ����� (�������� x � y)
    Text& SetPosition(Point pos);
    // ����� �������� ������������ ������� ����� (�������� dx, dy)
    Text& SetOffset(Point offset);
    // ����� ������� ������ (������� font-size)
    Text& SetFontSize(uint32_t size);
    // ����� �������� ������ (������� font-family)
    Text& SetFontFamily(std::string font_family);
    // ����� ������� ������ (������� font-weight)
    Text& SetFontWeight(std::string font_weight);
    // ����� ��������� ���������� ������� (������������ ������ ���� text)
    Text& SetData(std::string data);
    // ������ ������ � ������, ����������� ��� ���������� �������� <text>
private:
    void RenderObject(const RenderContext& context) const override;

    Point pos_ = { 0.0, 0.0 };
    Point offset_ = { 0.0, 0.0 };
    uint32_t size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;
};

class ObjectContainer {
public:
    template <typename Obj>
    void Add(Obj obj) {
        //objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
        AddPtr(std::make_unique<Obj>(std::move(obj)));
    }
    // ��������� � svg-�������� ������-��������� svg::Object
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    virtual ~ObjectContainer() = default;
};

class Drawable {
public:
    virtual void Draw(svg::ObjectContainer& container) const = 0;

    virtual ~Drawable() = default;
};

class Document : public ObjectContainer {
public:
    /*
     ����� Add ��������� � svg-�������� ����� ������-��������� svg::Object.
     ������ �������������:
     Document doc;
     doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
    */
    //template <typename Obj>
    //void Add(Obj obj) {
     //   objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
    //}
    // ��������� � svg-�������� ������-��������� svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj);

    // ������� � ostream svg-������������� ���������
    void Render(std::ostream& out) const;
private:
    std::vector<std::unique_ptr<Object>> objects_;
};

}  // namespace svg