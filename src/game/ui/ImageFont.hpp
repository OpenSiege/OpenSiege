
#pragma once

#include <optional>
#include <osg/ref_ptr>
#include "Font.hpp"

namespace osg
{
    class Texture2D;
}

namespace ehb
{
    struct Glyph
    {
        uint32_t width;
        float uv1, uv2, uv3, uv4;
    };

    class ImageFont : public Font
    {
    public:

        ImageFont(osg::ref_ptr<osg::Texture2D> texture, uint32_t height);

        virtual ~ImageFont() = default;

        virtual uint32_t getTextWidth(const std::string & text) const override;
        virtual uint32_t getTextHeight() const override;

        virtual osg::Drawable * createText(const std::string & text, const osg::Vec4 & color = osg::Vec4(1.f, 1.f, 1.f, 1.f)) const override;

        void addGlyph(uint8_t code, const Glyph & glyph);

    private:

        std::optional<Glyph> eachGlyph[0xff];
        osg::ref_ptr<osg::Texture2D> texture;
        uint32_t height;
    };
}
