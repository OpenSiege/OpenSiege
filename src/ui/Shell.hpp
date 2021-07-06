
#pragma once

#include <stdint.h>
#include <vector>

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
    class Button;
    class Widget;
    class Shell final
    {
    public:

        void init(IFileSys& fileSys, osg::Group* root, uint32_t width, uint32_t height);

        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action);

        std::string mapCtrlArt(const std::string& key) const;

        Widget* createDefaultWidgetOfType(const std::string& type);

        Widget* findWidget(const std::string& name, const std::string& interface) const;

        /*
         * not sure if this is required or not
         * in tooltips.gas there is an entry called help_textbox = frontend_help;
         * with no corresponding interface entry
         * so either those are hardcoded or there is a hidden function somewhere to find a
         * widget name regardless of interface parent name
         */
        Widget* findWidget(const std::string& name);

        void activateInterface(const std::string& path, bool show = true);
        void deactivateInterface(const std::string& interface);

        void showInterface(const std::string& interface);
        void hideInterface(const std::string& interface);

        int32_t mouseX() const;
        int32_t mouseY() const;

        uint32_t screenWidth() const;
        uint32_t screenHeight() const;

        void addWidget(Widget* widget);

        // TODO: remove
        void removeAllWidgets();

    private:

        // observer pointer?
        osg::Group* scene2d = nullptr;

        struct { int32_t x = 0, y = 0; } mouse;
        struct { uint32_t width = 0, height = 0; } screen;

        std::unordered_map<std::string, std::string> ctrlArt;

        //! this contains all the widgets that are loaded and sorted by their ds z order back to front
        std::vector<Widget*> backToFront;

        std::shared_ptr<spdlog::logger> log;

        void updateBackToFront();
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