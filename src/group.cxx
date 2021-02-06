#include <Fl/fl_draw.H>
#include "group.h"

using namespace Solid;

Group::Group(int x, int y, int w, int h, const char *label, const char *name) :
        SolidBase(name),
        Fl_Group(x, y, w, h, label) {
    clip_children(true);
}

void Group::resize(int x, int y, int w, int h) {
    Fl_Group::resize(x, y, w, h); // NOLINT(bugprone-parent-virtual-call)
    refreshLayout = true;
    redraw();
}

void Group::draw() {
    layout();

    cairo_t *cc = Fl::cairo_cc();

    if (cc) {
        cairo_rectangle(cc, parent()->x(), parent()->y(), parent()->w(), parent()->h());
        cairo_clip(cc);

        set_cairo_color(cc, SolidSkin::current->Surface);
        cairo_rectangle(cc, x(), y(), w(), h());
        cairo_fill(cc);
    }

//    Fl_Group::draw();
    for (int i = 0; i < children(); ++i) {
        child(i)->draw();
    }
}
