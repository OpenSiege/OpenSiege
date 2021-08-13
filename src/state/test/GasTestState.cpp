
#include "GasTestState.hpp"

#include <sstream>

#include <spdlog/spdlog.h>

#include "IFileSys.hpp"
#include "gas/Fuel.hpp"

namespace ehb
{
    void GasTestState::enter()
    {
        auto log = spdlog::get("log");
        log->info("GasTestState::enter()");

        auto stream = std::make_unique<std::stringstream>();
        *stream << "[t:template,n:actor]{doc=\"Generic brained objects\";}";

        if (Fuel doc; doc.load(*stream))
        {
            assert(doc.child("actor")->name() == "actor");
            assert(doc.child("actor")->type() == "template");
        }

        std::stringstream().swap(*stream);
        *stream << R"(
            [t:test,n:readValues]
            {
                [testValueAsInt4]
                {
                    test = 1, 9, 6, 2;
                }
            }
        )";

        if (Fuel doc; doc.load(*stream))
        {
            auto readValuesBlock = doc.child("readValues");
            assert(readValuesBlock != nullptr);
            assert(readValuesBlock->name() == "readValues");

            auto testValueAsInt4Block = readValuesBlock->child("testValueAsInt4");
            assert(testValueAsInt4Block != nullptr);

            auto testValuesAsInt4 = testValueAsInt4Block->valueAsInt4("test");
            assert(testValuesAsInt4[0] == 1); assert(testValuesAsInt4[1] == 9); assert(testValuesAsInt4[2] == 6); assert(testValuesAsInt4[3] == 2);
        }
    }

    void GasTestState::leave()
    {
    }

    void GasTestState::update(double deltaTime)
    {
    }

    bool GasTestState::handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action)
    {
        return false;
    }
}
