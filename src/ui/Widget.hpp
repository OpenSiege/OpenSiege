
#pragma once

#include <optional>
#include <osg/Group>
#include "Rect.hpp"

namespace ehb
{
    class Widget : public osg::Group
    {
    public:

        Widget() = default;

        void setRect(int32_t left, int32_t top, int32_t right, int32_t bottom);

        void addDebugData();

    private:

        Rect rect;
    };
}