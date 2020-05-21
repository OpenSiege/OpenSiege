
#pragma once

#include <string>

namespace ehb
{
    class IGameStateMgr
    {
    public:

        virtual ~IGameStateMgr() = default;

        virtual void request(const std::string & gameStateType) = 0;

        virtual std::string currentStateName() const = 0;
        virtual std::string previousStateName() const = 0;
        virtual std::string pendingStateName() const = 0;
    };
}
