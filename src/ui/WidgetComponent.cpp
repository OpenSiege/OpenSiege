
#include "WidgetComponent.hpp"
#include "Widget.hpp"

#include <osg/Texture2D>
#include <osgDB/ReadFile>

namespace ehb
{
    CenterComponent::CenterComponent(osg::Image* image, uint32_t horizontalBorder, uint32_t verticalBorder)
    {
        if (auto geometry = osg::createTexturedQuadGeometry(osg::Vec3(), osg::Vec3(1, 0, 0), osg::Vec3(0, 1, 0)))
        {
            osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);
            texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
            texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

            geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

            addChild(geometry);
        }

        border.horizontal = horizontalBorder;
        border.vertical = verticalBorder;
    }

    void CenterComponent::resizeToWidget(const Widget& widget)
    {
        Rect newRect = widget.effectiveRect();

        newRect.left += border.horizontal;
        newRect.right -= border.horizontal;
        newRect.top += border.vertical;
        newRect.bottom -= border.vertical;

        // now actually position/scale the widget component
        setPosition(osg::Vec3(newRect.left, newRect.top, 0));
        setScale(osg::Vec3(newRect.width(), newRect.height(), 1));

        // used for draw order sorting
        getChild(0)->setUserValue("draw_order", widget.drawOrder());
        getChild(0)->setUserValue("layer", widget.layer());
    }

    CornerComponent::CornerComponent(Corner corner, osg::Image* image) : corner(corner), width(image->s()), height(image->t())
    {
        osg::Texture2D* texture = new osg::Texture2D(image);

        auto geometry = osg::createTexturedQuadGeometry(osg::Vec3(), osg::Vec3(1, 0, 0), osg::Vec3(0, 1, 0));
        geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

        addChild(geometry);
    }

    void CornerComponent::resizeToWidget(const Widget& widget)
    {
        Rect rect = widget.effectiveRect();
        Rect newRect;

        switch (corner)
        {
        case Corner::TopLeft:
        {
            newRect.left = rect.left;
            newRect.right = rect.left + width;
            newRect.top = rect.top;
            newRect.bottom = rect.top + height;
        }
        break;

        case Corner::TopRight:
        {
            newRect.left = rect.right - width;
            newRect.right = rect.right;
            newRect.top = rect.top;
            newRect.bottom = rect.top + height;
        }
        break;

        case Corner::BottomLeft:
        {
            newRect.left = rect.left;
            newRect.right = rect.left + width;
            newRect.top = rect.bottom - height;
            newRect.bottom = rect.bottom;
        }
        break;

        case Corner::BottomRight:
        {
            newRect.left = rect.right - width;
            newRect.right = rect.right;
            newRect.top = rect.bottom - height;
            newRect.bottom = rect.bottom;
        }
        break;
        }

        // now actually position/scale the widget component
        setPosition(osg::Vec3(newRect.left, newRect.top, 0));
        setScale(osg::Vec3(newRect.width(), newRect.height(), 1));

        // used for draw order sorting
        getChild(0)->setUserValue("draw_order", widget.drawOrder());
        getChild(0)->setUserValue("layer", widget.layer());
    }

    SideComponent::SideComponent(Side side, osg::Image* image, uint32_t border) : side(side), border(border), width(image->s()), height(image->t())
    {
        osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);
        texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
        texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

        osg::ref_ptr<osg::Geometry> geometry = osg::createTexturedQuadGeometry(osg::Vec3(0, 0, 0), osg::Vec3(1, 0, 0), osg::Vec3(0, 1, 0));
        geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

        addChild(geometry);
    }

    void SideComponent::resizeToWidget(const Widget& widget)
    {
        Rect rect = widget.effectiveRect();
        Rect newRect;

        switch (side)
        {
        case Side::Left:
        {
            newRect.left = rect.left;
            newRect.right = rect.left + width;
            newRect.top = rect.top + border;
            newRect.bottom = rect.bottom - border;
        }
        break;

        case Side::Right:
        {
            newRect.left = rect.right - width;
            newRect.right = rect.right;
            newRect.top = rect.top + border;
            newRect.bottom = rect.bottom - border;
        }
        break;

        case Side::Top:
        {
            newRect.left = rect.left + border;
            newRect.right = rect.right - border;
            newRect.top = rect.top;
            newRect.bottom = rect.top + height;
        }
        break;

        case Side::Bottom:
        {
            newRect.left = rect.left + border;
            newRect.right = rect.right - border;
            newRect.top = rect.bottom - height;
            newRect.bottom = rect.bottom;
        }
        break;
        }

        // now actually position/scale the widget component
        setPosition(osg::Vec3(newRect.left, newRect.top, 0));
        setScale(osg::Vec3(newRect.width(), newRect.height(), 1));

        // used for draw order sorting
        getChild(0)->setUserValue("draw_order", widget.drawOrder());
        getChild(0)->setUserValue("layer", widget.layer());
    }
}