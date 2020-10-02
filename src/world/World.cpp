
#include "World.hpp"
#include "Region.hpp"

#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osgText/Text>
#include <osg/LineStipple>

namespace ehb
{
    class YAxisRotator : public osg::NodeCallback
    {
    public:

        YAxisRotator(osg::PositionAttitudeTransform* pat) : mPat(pat)
        {
        }

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) override
        {
            double currTime = nv->getFrameStamp()->getSimulationTime();
            double delta = prevTime - currTime;

            osg::Quat rot;
            rot.makeRotate(speed * delta, osg::Y_AXIS);

            mPat->setAttitude(mPat->getAttitude() * rot);

            prevTime = currTime;
        }

    private:

        double prevTime = 0;
        float speed = 1;

        osg::observer_ptr<osg::PositionAttitudeTransform> mPat;
    };

    void World::drawDebugCircle(osg::Group& scene, Region& region, const SiegePos& pos, float radius, osg::Vec4 color, const std::string& name)
    {
        // increase segments to get a smoother circle
        static uint32_t segments = 25;
        static const osg::Vec3 offset(0, 0.1f, 0);

        auto siegePos = region.convertFromSiegePos(pos);

        osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform;
        transform->setUpdateCallback(new YAxisRotator(transform));
        transform->setPosition(siegePos + offset);

        osg::ref_ptr<osg::Geometry> drawable = new osg::Geometry;
        osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

        for (uint32_t i = 0; i < segments; ++i)
        {
            float theta = 2 * osg::PI * i / segments;

            float x = radius * cosf(theta);
            float y = radius * sinf(theta);

            points->push_back(osg::Vec3(x, 0, y));

            // since per vertex need a color per vertex
            colors->push_back(color);
        }

        drawable->setVertexArray(points);
        drawable->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
        drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, points->size()));

        transform->addChild(drawable);

        // the transform takes care of our positioning so lets offset the text to the edge of the circle
        drawDebugScreenLabel(*transform, osg::Vec3(0, 0, radius), name);

        scene.addChild(transform);
    }

    void World::drawDebugDashedLine(osg::Group& scene, Region& region, const SiegePos& a, const SiegePos& b, osg::Vec4 color, const std::string& name)
    {
        auto siegePosA = region.convertFromSiegePos(a);
        auto siegePosB = region.convertFromSiegePos(b);

        osg::ref_ptr<osg::Geometry> beam = new osg::Geometry;
        osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;

        // offset our line on the y so it doesn't z-fight if its on the ground
        static osg::Vec3 offset(0, 0.1f, 0);
        points->push_back(siegePosA + offset);
        points->push_back(siegePosB + offset);

        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
        colors->push_back(color);
        colors->push_back(color);

        beam->setVertexArray(points);
        beam->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
        beam->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, points->size()));

        beam->getOrCreateStateSet()->setAttributeAndModes(new osg::LineStipple(2, 0x00FF));

        osg::ref_ptr<osg::Group> group = new osg::Group;
        group->addChild(beam);

        drawDebugScreenLabel(*group, ((siegePosA + offset) + (siegePosB + offset)) / 2, name);

        scene.addChild(group);
    }

    void World::drawDebugLine(osg::Group& scene, Region& region, const SiegePos& a, const SiegePos& b, osg::Vec4 color, const std::string& name)
    {
        auto siegePosA = region.convertFromSiegePos(a);
        auto siegePosB = region.convertFromSiegePos(b);

        osg::ref_ptr<osg::Geometry> beam = new osg::Geometry;
        osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;

        // offset our line on the y so it doesn't z-fight if its on the ground
        static const osg::Vec3 offset(0, 0.1f, 0);
        points->push_back(siegePosA + offset);
        points->push_back(siegePosB + offset);

        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
        colors->push_back(color);
        colors->push_back(color);

        beam->setVertexArray(points);
        beam->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
        beam->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, points->size()));

        osg::ref_ptr<osg::Group> group = new osg::Group;
        group->addChild(beam);        

        drawDebugScreenLabel(*group, ((siegePosA + offset) + (siegePosB + offset)) / 2, name);

        scene.addChild(group);
    }

    void World::drawDebugScreenLabel(osg::Group& scene, const osg::Vec3& position, const std::string& text)
    {
        if (osg::ref_ptr<osgText::Text> node = new osgText::Text)
        {
            node->setText(text);
            node->setCharacterSize(12);
            node->setPosition(position);
            node->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
            node->setAxisAlignment(osgText::Text::XY_PLANE);
            node->setAlignment(osgText::TextBase::CENTER_BASE_LINE);
            node->setAutoRotateToScreen(true);

            scene.addChild(node);
        }
    }
}
