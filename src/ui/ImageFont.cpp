
#include "ImageFont.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    ImageFont::ImageFont(osg::ref_ptr<osg::Texture2D> texture, uint32_t height) : texture(texture), height(height)
    {
    }

    uint32_t ImageFont::getTextWidth(const std::string & text) const
    {
        uint32_t width = 0;

        for (uint32_t i = 0; i < text.size(); i++)
        {
            char code = text[i];

            if (eachGlyph[code])
            {
                // leave one pixel between each character?
                width += (eachGlyph[code]->width + 1);
            }
        }

        return width;
    }

    uint32_t ImageFont::getTextHeight() const
    {
        return height;
    }

    osg::Drawable * ImageFont::createText(const std::string & text, const osg::Vec4 & color) const
    {
        osg::ref_ptr<osg::Geometry> drawable = new osg::Geometry;

        // setup drawable data
        osg::Vec3Array * vertices = new osg::Vec3Array(6 * text.size());
        osg::Vec2Array * tcoords = new osg::Vec2Array(6 * text.size());
        osg::Vec4Array * colors = new osg::Vec4Array(6 * text.size());

        uint32_t x = 0;

        for (uint32_t i = 0; i < text.size(); i++)
        {
            unsigned char code = text[i];

            if (eachGlyph[code])
            {
                const uint32_t width = eachGlyph[code]->width;

                (*vertices)[(i * 6) + 0].set(osg::Vec3(x, height, 0));
                (*vertices)[(i * 6) + 1].set(osg::Vec3(x, 0, 0));
                (*vertices)[(i * 6) + 2].set(osg::Vec3(x + width, 0, 0));

                (*vertices)[(i * 6) + 3].set(osg::Vec3(x, height, 0));
                (*vertices)[(i * 6) + 4].set(osg::Vec3(x + width, 0, 0));
                (*vertices)[(i * 6) + 5].set(osg::Vec3(x + width, height, 0));

                float l = eachGlyph[code]->uv1;
                float b = eachGlyph[code]->uv2;
                float r = eachGlyph[code]->uv3;
                float t = eachGlyph[code]->uv4;

                (*tcoords)[(i * 6) + 0].set(osg::Vec2(l, t));
                (*tcoords)[(i * 6) + 1].set(osg::Vec2(l, b));
                (*tcoords)[(i * 6) + 2].set(osg::Vec2(r, b));

                (*tcoords)[(i * 6) + 3].set(osg::Vec2(l, t));
                (*tcoords)[(i * 6) + 4].set(osg::Vec2(r, b));
                (*tcoords)[(i * 6) + 5].set(osg::Vec2(r, t));

                // per pixel coloring
                (*colors)[(i * 6) + 0] = color;
                (*colors)[(i * 6) + 1] = color;
                (*colors)[(i * 6) + 2] = color;
                (*colors)[(i * 6) + 3] = color;
                (*colors)[(i * 6) + 4] = color;
                (*colors)[(i * 6) + 5] = color;

                // leave one pixel between each character?
                x += (width + 1);
            }
            else
            {
                spdlog::get("log")->error("could not find char {}", (char) code);
            }
        }

        drawable->setVertexArray(vertices);
        drawable->setTexCoordArray(0, tcoords);
        drawable->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
        drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, vertices->size()));

        drawable->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

        return drawable.release();
    }

    void ImageFont::addGlyph(uint8_t code, const Glyph & glyph)
    {
        if (code >= 0x00 && code < 0xff)
        {
            eachGlyph[code] = glyph;
        }
    }
}
