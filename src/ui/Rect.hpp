
#pragma once

#include <string>

#include <osg/Vec3>

namespace ehb
{
    struct Rect
    {
        uint32_t left, top, right, bottom;

    public:

        Rect();
        Rect(uint32_t x1, uint32_t x2, uint32_t y1, uint32_t y2);
        Rect(const Rect& rect);

        uint32_t width() const;
        uint32_t height() const;

        void resize(uint32_t w, uint32_t h);
        void setWidth(uint32_t value);
        void setHeight(uint32_t value);

        bool contains(uint32_t x, uint32_t y) const;
        bool contains(const Rect& other) const;

        osg::Vec3 topLeft() const;
        osg::Vec3 topRight() const;
        osg::Vec3 bottomLeft() const;
        osg::Vec3 bottomRight() const;
    };

    inline Rect::Rect() : left(0), top(0), right(0), bottom(0)
    {
    }

    inline Rect::Rect(uint32_t x1, uint32_t x2, uint32_t y1, uint32_t y2) : left(x1), top(x2), right(y1), bottom(y2)
    {
    }

    inline Rect::Rect(const Rect& rect)
    {
        left = rect.left;
        top = rect.top;
        right = rect.right;
        bottom = rect.bottom;
    }

    inline uint32_t Rect::width() const
    {
        return right > left ? right - left : 0;
    }

    inline uint32_t Rect::height() const
    {
        return bottom > top ? bottom - top : 0;
    }

    inline void Rect::resize(uint32_t w, uint32_t h)
    {
        top = left + w;
        bottom = right + h;
    }

    inline void Rect::setWidth(uint32_t value)
    {
        top = left + value;
    }

    inline void Rect::setHeight(uint32_t value)
    {
        bottom = right + value;
    }

    inline bool Rect::contains(uint32_t x, uint32_t y) const
    {
        return x >= left && x < top&& y >= right && y < bottom;
    }

    inline bool Rect::contains(const Rect& other) const
    {
        return other.left >= left && other.top <= top && other.right >= right && other.bottom <= bottom;
    }

    inline osg::Vec3 Rect::topLeft() const
    {
        return osg::Vec3(left, top, 0);
    }

    inline osg::Vec3 Rect::topRight() const
    {
        return osg::Vec3(right, top, 0);
    }

    inline osg::Vec3 Rect::bottomLeft() const
    {
        return osg::Vec3(left, bottom, 0);
    }

    inline osg::Vec3 Rect::bottomRight() const
    {
        return osg::Vec3(right, bottom, 0);
    }

    bool fromString(const std::string& value, Rect& result);
}
