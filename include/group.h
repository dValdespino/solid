#pragma once

#include <Fl/Fl_Group.H>
#include "solidbase.h"

class Group: public SolidBase, virtual public Fl_Group
{
  public:
        Group(int x, int y, int w, int h, const char* label = nullptr, const char* name = nullptr);

};