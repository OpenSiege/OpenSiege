
#include "Widget.hpp"

#include "Shell.hpp"

#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/Texture2D>

#include <spdlog/spdlog.h>

namespace ehb
{
    //! need to fix console before we can do this
#if 0
    Widget::Widget(Shell& shell) : shell(shell)
    {

    }
#endif

    void Widget::setRect(int32_t left, int32_t top, int32_t right, int32_t bottom)
    {
        rect.top = top;
        rect.left = left;
        rect.right = right;
        rect.bottom = bottom;
    }

    void Widget::loadTexture(const std::string& textureFileName, bool resizeWidget)
    {
        // TODO: if common control do nothing?

        if (auto image = osgDB::readRefImageFile(textureFileName + ".raw"))
        {
            if (resizeWidget)
            {
                // TODO
            }

            if (baseComponent == nullptr)
            {
                baseComponent = new CenterComponent(image);
                addChild(baseComponent);
            }
            else
            {
                auto texture = new osg::Texture2D(image);
                auto geometry = static_cast<osg::Geometry*>(baseComponent->getChild(0));
                geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
            }

            baseComponent->resizeToWidget(*this);
        }
    }

    void Widget::setUVRect(float left, float top, float right, float bottom)
    {
        uv.left = left;
        uv.right = right;
        uv.top = top;
        uv.bottom = bottom;

        if (baseComponent)
        {
            if (auto data = dynamic_cast<osg::Vec2Array*>(baseComponent->getChild(0)->asGeometry()->getTexCoordArray(0)))
            {
                (*data)[0].set(left, bottom);
                (*data)[1].set(left, top);
                (*data)[2].set(right, top);
                (*data)[3].set(right, bottom);
            }
        }
        else
        {
            spdlog::get("log")->error("you tried to set the rect on a widget before setting the texture");
        }
    }

    //! before we can uncomment this I need to fix the console
#if 0
    inline uint32_t Widget::screenWidth() const
    {
        return shell.screenWidth();
    }

    inline uint32_t Widget::screenHeight() const
    {
        return shell.screenHeight();
    }
#endif

    void Widget::addDebugData()
    {
        osg::ref_ptr<osg::Geometry> lines = new osg::Geometry;

        // TODO: use a line loop?
        osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;

        spdlog::get("log")->info("widget ({}) rect {} {} {} {}", getName(), rect.left, rect.top, rect.right, rect.bottom);

        // top left to bottom left
        points->push_back(rect.topLeft());
        points->push_back(rect.bottomLeft());

        // bottom left to bottom right
        points->push_back(rect.bottomLeft());
        points->push_back(rect.bottomRight());

        // bottom right to top right
        points->push_back(rect.bottomRight());
        points->push_back(rect.topRight());

        // top right to top left
        points->push_back(rect.topRight());
        points->push_back(rect.topLeft());

        const osg::Vec4 color(1, 1, 1, 1);

        // per vertex coloring
        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
        for (size_t i = 0; i < points->size(); ++i)
        {
            colors->push_back(color);
        }

        lines->setVertexArray(points);
        lines->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
        lines->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, points->size()));

        addChild(lines);
    }
}