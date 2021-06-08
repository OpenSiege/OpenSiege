
#pragma once

#include <string>

namespace ehb
{
    struct NormalizedRect final
    {
        float left = 0.0f, right = 0.0f, top = 0.0f, bottom = 0.0f;

    public:

        NormalizedRect() = default;
        NormalizedRect(float left, float top, float right, float bottom);
    };

    inline NormalizedRect::NormalizedRect(float left, float top, float right, float bottom) : left(left), top(top), right(right), bottom(bottom)
    {
    }

    bool fromString(const std::string& value, NormalizedRect& result);
}
