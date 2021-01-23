
#pragma once

#include "state/IGameState.hpp"
#include "filesystem/tank_file.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    class IConfig;
    class IFileSys;
    class TankTestState : public IGameState
    {
    public:

        TankTestState(IGameStateMgr & gameStateMgr, IConfig & config, IFileSys& fileSys);

        virtual ~TankTestState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        bool validateFileSize(const TankFile& tank, std::uintmax_t expected);

    private:

        IGameStateMgr & gameStateMgr;
        IConfig & config;
        IFileSys& fileSys;
    };

    inline TankTestState::TankTestState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys) : gameStateMgr(gameStateMgr), config(config), fileSys(fileSys)
    {
    }

    inline bool TankTestState::validateFileSize(const TankFile& tank, std::uintmax_t expected)
    {
        if (tank.getFileSizeBytes() != expected)
        {
            spdlog::get("log")->error("Tank: {} was loaded with {} bytes but expected was {}", tank.getFileName(), tank.getFileSizeBytes(), expected);
            return false;
        }
    }
}
