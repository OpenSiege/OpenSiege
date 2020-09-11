
#include "Widget.hpp"

#include <osg/Geometry>

#include <spdlog/spdlog.h>

namespace ehb
{
    void Widget::setRect(int32_t left, int32_t top, int32_t right, int32_t bottom)
    {
        rect.top = top;
        rect.left = left;
        rect.right = right;
        rect.bottom = bottom;
    }

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