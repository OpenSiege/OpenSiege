
#pragma once

#include <osg/Object>
#include <osg/Vec4>
#include <osgText/Font>

namespace osg
{
    class Drawable;
}

namespace ehb
{
    class Font : public osg::Object
    {
    public:

        virtual ~Font() = default;

        virtual osg::Object * cloneType() const override { return nullptr; }
        virtual osg::Object* clone(const osg::CopyOp &) const override { return nullptr; }
        virtual bool isSameKindAs(const osg::Object * object) const override { return dynamic_cast<const Font *>(object) != nullptr; }
        virtual const char * className() const override { return "Font"; }
        virtual const char * libraryName() const override { return "ehb"; }

        virtual unsigned int getTextWidth(const std::string & text) const = 0;
        virtual unsigned int getTextHeight() const = 0;

        virtual osg::Drawable * createText(const std::string & text, const osg::Vec4 & color = osg::Vec4(1.f, 1.f, 1.f, 1.f)) const = 0;
    };
}
