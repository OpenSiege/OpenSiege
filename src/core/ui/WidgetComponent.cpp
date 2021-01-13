
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
            osg::Texture2D* texture = new osg::Texture2D(image);
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
        newRect.top -= border.horizontal;
        newRect.right += border.vertical;
        newRect.bottom -= border.vertical;

        // now actually position/scale the widget component
        setPosition(osg::Vec3(newRect.left, newRect.top, 0));
        setScale(osg::Vec3(newRect.width(), newRect.height(), 1));

        // used for draw order sorting
        //getChild(0)->setUserValue("draw_order", widget.drawOrder());
        //getChild(0)->setUserValue("layer", widget.layer());
    }
}