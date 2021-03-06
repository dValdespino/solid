#include "button.h"
#include "Fl/fl_draw.H"
#include <Fl/Fl_Group.H>
#include <Fl/Fl_Cairo.H>
#include <cairo.h>

using namespace Solid;

Button::Button(int x, int y, int w, int h, const char *label, const char *name) :
        SolidBase(name),
        Fl_Button(x, y, w, h, label) {
    this->labelfont(1);
    this->labelsize(12);
    this->label(label);
    this->padding = {7, 7, 10, 10};
}

int Button::handle(int evt) {
    if (!active_r())
        return 0;

    switch (evt) {
        case FL_PUSH:
        case FL_RELEASE:
        case FL_LEAVE:
        case FL_ENTER:
            fl_cursor(Fl::belowmouse() == this ? FL_CURSOR_HAND : FL_CURSOR_ARROW);
            redraw();
            break;
    }

    return Fl_Button::handle(evt);
}

void drawSpacedText(cairo_t *cc, const char *text, float spacing) {
    char buff[2];

    for (int i = 0; i < strlen(text); i++) {
        buff[0] = text[i];

        cairo_show_text(cc, buff);
        cairo_rel_move_to(cc, spacing, 0);
    }
}

void Button::draw() {
    cairo_t *cc = get_cc();

//    cairo_rectangle(cc, parent()->x(), parent()->y(), parent()->w(), parent()->h());
//    cairo_clip(cc);

    cairo_rectangle(cc, x(), y(), w(), h());
    set_cairo_color(cc, SolidSkin::current->Surface);
    cairo_fill(cc);

    switch (Type) {
        case ButtonType::Outline:
            set_cairo_color(cc, outlineColor());

            cairo_set_line_width(cc, 0.6);
            round_rect(cc, x(), y(), w(), h(), 2);
            cairo_stroke(cc);
            break;

        case ButtonType::Primary:
            set_cairo_color(cc, buttonColor());

            cairo_set_line_width(cc, 0.1);
            round_rect(cc, x(), y(), w(), h(), 2);
            cairo_fill(cc);
            break;

        case ButtonType::Text:
            break;
    }

    draw_label(cc);

    if (Fl::belowmouse() != this->as_widget())
        return;

    debugDraw();
}

void Button::draw_label(cairo_t *cc) {
    set_cairo_color(cc, textColor());

    auto font = SolidSkin::fonts[0];

    cairo_set_line_width(cc, 1);
    cairo_set_font_face(cc, font);
    cairo_set_font_size(cc, labelsize());

    Measure m = layout();
    cairo_move_to(cc, x() + w() / 2.0 - (m.Width - padding.Horizontal()) / 2,
                  y() + h() / 2.0 + (m.Height - padding.Vertical()) / 2);

    drawSpacedText(cc, label(), fontSpacing);
}

#include "ctype.h"
#include "solidbase.h"

void Button::label(const char *l) {
    if (l == nullptr)return;

    char *ll = (char *) strdup(l);
    for (int i = 0; i < strlen(l); ++i) {
        ll[i] = toupper(ll[i]);
    }
    Fl_Button::label(ll);
}

const char *Button::label() {
    return Fl_Button::label();
}

Measure Button::layout() {
    cairo_text_extents_t extents;

    auto cc = Fl::cairo_cc();
    if (cc == nullptr || label() == nullptr) {
        return Measure{static_cast<float>(w()), static_cast<float>(h())};
    }

    cairo_set_font_face(cc, SolidSkin::fonts[1]);
    cairo_set_font_size(cc, labelsize());
    cairo_text_extents(cc, label(), &extents);

    float _spacing = 0;

    if (label() != nullptr) {
        int label_length = strlen(label());
        if (label_length > 0)
            _spacing = (static_cast<float>(label_length) - 1) * fontSpacing;
    }

    Measure m;
    m.Width = padding.Horizontal() + extents.x_advance + _spacing;
    m.Height = padding.Vertical() + extents.height;
    return m;
}

Button *Button::Primary(int x, int y, int w, int h, const char *label, const char *name) {
    auto b = new Button(x, y, w, h, label, name);
    b->Type = ButtonType::Primary;
    return b;
}

Button *Button::Outline(int x, int y, int w, int h, const char *label, const char *name) {
    auto b = new Button(x, y, w, h, label, name);
    b->Type = ButtonType::Outline;
    return b;
}

Button *Button::Text(int x, int y, int w, int h, const char *label, const char *name) {
    auto b = new Button(x, y, w, h, label, name);
    b->Type = ButtonType::Text;
    return b;
}

Fl_Color Button::buttonColor() {
    if (!active_r())
        return FL_GRAY;

    switch (Type) {
        case ButtonType::Text:
        case ButtonType::Outline:
            return SolidSkin::current->Surface;
        default:
            break;
    }

    bool hovered = Fl::belowmouse() == this;
    bool clicked = hovered && Fl::event_button1();

    if (!active()) {
        hovered = false;
        clicked = false;
    }

    Fl_Color ret = clicked ? fl_darker(SolidSkin::current->Primary) : fl_lighter(SolidSkin::current->Primary);
    ret = hovered ? ret : SolidSkin::current->Primary;

    if (!active()) {
        uchar r, g, b;

        Fl::get_color(ret, r, g, b);

        float sum = r + g + b;
        ret = fl_gray_ramp(sum / 3.0);
    }

    return ret;
}

Fl_Color Button::outlineColor() {
    auto ret = SolidSkin::current->Surface;
    Fl_Color target;

    ret = SolidSkin::current->Surface;
    target = fl_contrast(FL_WHITE, SolidSkin::current->Surface);

    ret = fl_color_average(ret, target, 0.8);

    return ret;
}

Fl_Color Button::textColor() {
    Fl_Color ret;
    bool hovered = Fl::belowmouse() == this;
    bool clicked = hovered && Fl::event_button1();

    if (!active_r()) {
        return this->Type == ButtonType::Primary ? SolidSkin::current->Surface : FL_GRAY;
//        hovered = false;
//        clicked = false;
    }

    if (Type == ButtonType::Primary) {
        ret = hovered ? fl_lighter(SolidSkin::current->OnPrimary)
                      : SolidSkin::current->OnPrimary;
    } else {
        ret = hovered ? fl_lighter(SolidSkin::current->Primary)
                      : SolidSkin::current->Primary;
    }
    if (Type != ButtonType::Primary && hovered && clicked) {
        ret = fl_darker(ret);
    }

    if (!active_r()) {
        uchar r, g, b;

        Fl::get_color(ret, r, g, b);

        float sum = r + g + b;
        ret = fl_gray_ramp(sum / 3.0);
    }

    return ret;
}

PrimaryButton::PrimaryButton(int x, int y, int w, int h, const char *label, const char *name) : Button(x, y, w, h,
                                                                                                       label, name) {
    this->Type = ButtonType::Primary;
}

OutlineButton::OutlineButton(int x, int y, int w, int h, const char *label, const char *name) : Button(x, y, w, h,
                                                                                                       label, name) {
    this->Type = ButtonType::Outline;
}

TextButton::TextButton(int x, int y, int w, int h, const char *label, const char *name) : Button(x, y, w, h,
                                                                                                 label, name) {
    this->Type = ButtonType::Text;
}
