
#include "GasTestState.hpp"

#include <sstream>

#include <spdlog/spdlog.h>

#include "IFileSys.hpp"
#include "gas/Fuel.hpp"

#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
#include "doctest.h"

namespace ehb
{

    static void handler(const doctest::AssertData& ad)
    {
        using namespace doctest;

        auto log = spdlog::get("testing");

        std::stringstream ss;

        ss << skipPathFromFilename(ad.m_file) << "(" << ad.m_line << "): " << failureString(ad.m_at) << ": ";

        // handling only normal (comparison and unary) asserts - exceptions-related asserts have been skipped
        if (ad.m_at & assertType::is_normal) {
            ss << assertString(ad.m_at) << "( " << ad.m_expr << " ) ";
            ss << (ad.m_threw ? "THREW exception: " : "is NOT correct!\n");
            if (ad.m_threw)
                ss << ad.m_exception;
            else
                ss << "  values: " << assertString(ad.m_at) << "( " << ad.m_decomp << " )";
        }
        else {
            ss << "an assert dealing with exceptions has failed!";
        }

        log->error("{}", ss.str());
    }

    void GasTestState::enter()
    {
        auto log = spdlog::get("log");
        log->info("GasTestState::enter()");

        doctest::Context context;
        context.setAsDefaultForAssertsOutOfTestCases();
        context.setAssertHandler(handler);

        CHECK_EQ(true, false);

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
