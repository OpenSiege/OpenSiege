
#pragma once

#include <stdint.h>

#include <spdlog/spdlog.h>

namespace osg
{
    class Group;
}

namespace osgGA
{
    class GUIActionAdapter;
    class GUIEventAdapter;
}

namespace ehb
{
    class IFileSys;
    class Shell
    {
    public:

        void init(IFileSys& fileSys, osg::Group* root, uint32_t width, uint32_t height);

        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action);

        int32_t mouseX() const;
        int32_t mouseY() const;

        uint32_t screenWidth() const;
        uint32_t screenHeight() const;

    private:

        // observer pointer?
        osg::Group* scene2d = nullptr;

        struct { int32_t x, y; } mouse;
        struct { uint32_t width, height; } screen;

        std::shared_ptr<spdlog::logger> log;
    };

    inline int32_t Shell::mouseX() const
    {
        return mouse.x;
    }

    inline int32_t Shell::mouseY() const
    {
        return mouse.y;
    }

    inline uint32_t Shell::screenWidth() const
    {
        return screen.width;
    }

    inline uint32_t Shell::screenHeight() const
    {
        return screen.height;
    }
}