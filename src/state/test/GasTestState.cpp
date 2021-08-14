
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

        auto stream = std::make_unique<std::stringstream>();
        *stream << "[t:template,n:actor]{doc=\"Generic brained objects\";}";

        if (Fuel doc; doc.load(*stream))
        {
            REQUIRE(doc.child("actor")->name() == "actor");
            REQUIRE(doc.child("actor")->type() == "template");
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
            REQUIRE(readValuesBlock != nullptr);

            auto testValueAsInt4Block = readValuesBlock->child("testValueAsInt4");
            CHECK(testValueAsInt4Block != nullptr);

            auto testValuesAsInt4 = testValueAsInt4Block->valueAsInt4("test");
            CHECK(testValuesAsInt4[0] == 1); CHECK(testValuesAsInt4[1] == 9); CHECK(testValuesAsInt4[2] == 6); CHECK(testValuesAsInt4[3] == 2);
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
