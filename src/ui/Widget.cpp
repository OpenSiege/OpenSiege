
#include "Widget.hpp"

#include "Shell.hpp"
#include "gas/Fuel.hpp"

#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/Texture2D>

#include <spdlog/spdlog.h>

namespace ehb
{
    class ResizeWidgetVisitor : public osg::NodeVisitor
    {
    public:

        ResizeWidgetVisitor(Widget* widget) : NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN), widget(widget)
        {
        }

        virtual ~ResizeWidgetVisitor() = default;

        virtual void apply(osg::Node& node)
        {
            if (auto component = dynamic_cast<WidgetComponent*>(&node))
            {
                component->resizeToWidget(*widget);
            }
            else
            {
                traverse(node);
            }
        }

    private:

        Widget* widget;
    };
}

namespace ehb
{
    Widget::Widget(Shell& shell) : shell(shell)
    {

    }

    void Widget::setRect(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
    {
        rect.left = left;
        rect.top = top;
        rect.right = right;
        rect.bottom = bottom;

        // when we set our rect we have to respect a lot of UI rules
        resizeToScreenResolution(shell.screenWidth(), shell.screenHeight());
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
                osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);
                auto geometry = static_cast<osg::Geometry*>(baseComponent->getChild(0));
                geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
            }

            baseComponent->resizeToWidget(*this);
        }
    }

    /**
    * Define our UV rect for this widget
    * 
    * The ReaderWriterUI will read in all the parameters for this widget so
    */
    void Widget::setUVRect(float left, float top, float right, float bottom)
    {
        uv.left = left;
        uv.top = top;
        uv.right = right;
        uv.bottom = bottom;

        // there are instances where the uv will be set before the base component gets created
        // this is okay and it will be picked up on the resize
        if (baseComponent)
        {
            if (auto data = dynamic_cast<osg::Vec2Array*>(baseComponent->getChild(0)->asGeometry()->getTexCoordArray(0)))
            {
                (*data)[0].set(left, 1 - top);
                (*data)[1].set(left, 1 - bottom);
                (*data)[2].set(right, 1 - bottom);
                (*data)[3].set(right, 1 - top);
            }
        }
    }

    uint32_t Widget::screenWidth() const
    {
        return shell.screenWidth();
    }

    uint32_t Widget::screenHeight() const
    {
        return shell.screenHeight();
    }

    void Widget::createCommonCtrl(const std::string& value)
    {
        // if this widget has a parent widget then change the template to widget::type + _value
        if (attr.tmpl.empty() && !value.empty())
        {
            attr.tmpl = value;

            resizeToScreenResolution(shell.screenWidth(), shell.screenHeight());
        }
    }

    void Widget::setTiledTexture(bool value)
    {
        if (tiled == value) return;

        // this assumes that loadTexture has already been called
        if (baseComponent != nullptr)
        {
            tiled = value;

            if (auto blah = baseComponent->getChild(0)->getOrCreateStateSet()->getTextureAttribute(0, osg::StateAttribute::TEXTURE))
            {
                if (osg::ref_ptr<osg::Texture2D> texture = dynamic_cast<osg::Texture2D*>(blah); texture != nullptr)
                {
                    if (tiled)
                    {
                        texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
                        texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
                    }
                    else
                    {
                        texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP);
                        texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP);
                    }
                }
                else spdlog::get("log")->error("failed to retrieve texture on gui object {}", getName());
            }
            else spdlog::get("log")->error("failed to retrieve texture attribute on gui object {}", getName());
        }
        else spdlog::get("log")->error("failed to get a baseComponent on gui object {}", getName());
    }

    void Widget::resizeToScreenResolution(uint32_t newScreenWidth, uint32_t newScreenHeight)
    {
        auto log = spdlog::get("log");

        // resize based on our parent
        if (parentWidget != nullptr)
        {
            log->info("{} is attempting to do a resize using its parent widget", getName());
        }
        else if (base) // resize based on the optional base rect
        {
            log->info("{} is attempting to do a resize using 'base' rect with starting size of {}", getName(), effectiveRect());

            // const double stretchFactorX = resX && newScreenWidth > *resX ? static_cast<double>(newScreenWidth) / static_cast<double>(*resX) : 1.0;
            // const double stretchFactorY = resY && newScreenHeight > *resY ? static_cast<double>(newScreenHeight) / static_cast<double>(*resY) : 1.0;
            const double stretchFactorX = 1.0, stretchFactorY = 1.0;

            log->info("prior to resize the width of {} is {} and the height is {}\n\t this was calculated from rect {}", getName(), base->width(), base->height(), *base);

            uint32_t newWidgetWidth = stretch.x ? newScreenWidth : stretchFactorX * static_cast<double>(base->width());
            uint32_t newWidgetHeight = stretch.y ? newScreenHeight : stretchFactorY * static_cast<double>(base->height());

            if (stretch.x)
            {
                rect.left = 0;
                rect.right = newScreenWidth;
            }
            else
            {
                if (center.x) rect.left = newScreenWidth > newWidgetWidth ? (newScreenWidth - newWidgetWidth) / 2 : (newWidgetWidth - newScreenWidth) / 2;
                else if (anchor.left) rect.left = *anchor.left;
                else if (anchor.right) rect.left = newScreenWidth - *anchor.right;
                else rect.left = base->left;

                rect.left += shift.x;
            }

            if (stretch.y)
            {
                rect.right = 0;
                rect.bottom = newScreenHeight;
            }
            else
            {
                if (center.y) rect.top = newScreenHeight > newWidgetHeight ? (newScreenHeight - newWidgetHeight) / 2 : (newWidgetHeight - newScreenHeight) / 2;
                else if (anchor.top) rect.top = *anchor.top;
                else if (anchor.bottom) rect.top = newScreenHeight - *anchor.bottom;
                else rect.top = base->top;

                rect.top += shift.y;
            }

            rect.resize(newWidgetWidth, newWidgetHeight);
        }

        // TODO: cursor

        // children resize
        for (uint32_t i = 0; i < getNumChildren(); i++)
        {
            if (Widget* widget = dynamic_cast<Widget*>(getChild(i)))
            {
                widget->resizeToScreenResolution(newScreenWidth, newScreenHeight);
            }
        }

        // resize all internal components of this widget
        ResizeWidgetVisitor visitor(this);
        accept(visitor);
    }

    void Widget::addDebugData()
    {
        osg::ref_ptr<osg::Geometry> lines = new osg::Geometry;

        // TODO: use a line loop?
        osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;

        spdlog::get("log")->info("widget ({}) {}", getName(), rect);

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

        static osg::Vec4 color(1, 1, 1, 1);

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