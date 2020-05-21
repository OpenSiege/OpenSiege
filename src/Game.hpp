
#pragma once

#include <osgGA/GUIEventHandler>

namespace ehb
{
    // TODO: do not inherit from osgGA::GUIEventHandler but instead create a separate class just for this
    class IConfig;
    class Game final : public osgGA::GUIEventHandler
    {
    public:

        Game(IConfig & config);

        int exec();

    private:

        virtual ~Game() = default;

    private:

        void setupScene();

        IConfig& config;
    };
}
