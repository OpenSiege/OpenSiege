
#pragma once

#include <string>

#include <osg/Vec3>

#include <spdlog/fmt/ostr.h>

namespace ehb
{
    /**
     * Rect assumes your viewport starts drawing (0,0) from the top left
     * this is accomplished in OpenGL by using the ClipControl::UPPER_LEFT on the Shell camera StateSet (see Game.cpp)
    */
    struct Rect final
    {
        uint32_t left = 0.0f, top = 0.0f, right = 0.0f, bottom = 0.0f;

    public:

        Rect() = default;
        Rect(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);

        uint32_t width() const;
        uint32_t height() const;

        void resize(uint32_t w, uint32_t h);
        void setWidth(uint32_t value);
        void setHeight(uint32_t value);

        bool contains(uint32_t x, uint32_t y) const;
        bool contains(const Rect& other) const;

        const osg::Vec3 topLeft() const;
        const osg::Vec3 topRight() const;
        const osg::Vec3 bottomLeft() const;
        const osg::Vec3 bottomRight() const;

        template<typename ostream>
        friend ostream& operator<<(ostream& os, const Rect& rect)
        {
            return os << "rect: " << rect.left << ", " << rect.top << ", " << rect.right << ", " << rect.bottom;
        }
    };

    inline Rect::Rect(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) : left(left), top(top), right(right), bottom(bottom)
    {
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
        right = left + w;
        bottom = top + h;
    }

    /**
     * Compute the new width of our rect
     * It's important to remember DS GUI starts drawing from the top left of the viewport (0, 0).
     * When calculating height we start at the "left" of the widget and add our value as that will move across the viewport
    */
    inline void Rect::setWidth(uint32_t value)
    {
        right = left + value;
    }

    /**
     * Compute the new height of our rect
     * It's important to remember DS GUI starts drawing from the top left of the viewport (0, 0).
     * When calculating height we start at the "top" of the widget and add our value as that will move down the viewport
    */
    inline void Rect::setHeight(uint32_t value)
    {
        bottom = top + value;
    }

    inline bool Rect::contains(uint32_t x, uint32_t y) const
    {
        return x >= left && x < top&& y >= right && y < bottom;
    }

    inline bool Rect::contains(const Rect& other) const
    {
        return other.left >= left && other.top <= top && other.right >= right && other.bottom <= bottom;
    }

    inline const osg::Vec3 Rect::topLeft() const
    {
        return osg::Vec3(left, top, 0);
    }

    inline const osg::Vec3 Rect::topRight() const
    {
        return osg::Vec3(right, top, 0);
    }

    inline const osg::Vec3 Rect::bottomLeft() const
    {
        return osg::Vec3(left, bottom, 0);
    }

    inline const osg::Vec3 Rect::bottomRight() const
    {
        return osg::Vec3(right, bottom, 0);
    }

    bool fromString(const std::string& value, Rect& result);
}
