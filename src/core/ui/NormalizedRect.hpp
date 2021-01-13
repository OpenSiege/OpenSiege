
#pragma once

#include <string>

namespace ehb
{
    struct NormalizedRect
    {
        float left, right, top, bottom;

    public:

        NormalizedRect();
        NormalizedRect(float left, float top, float right, float bottom);
    };

    inline NormalizedRect::NormalizedRect() : left(0), top(0), right(0), bottom(0)
    {
    }

    inline NormalizedRect::NormalizedRect(float left, float top, float right, float bottom) : left(left), top(top), right(right), bottom(bottom)
    {
    }

    bool fromString(const std::string& value, NormalizedRect& result);
}
