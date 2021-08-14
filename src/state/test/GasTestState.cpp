
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
                [testValueAsBool]
                {
                    test = true;
                }
                [testValueAsInt]
                {
                    test = -543;
                }
                [testValueAsInt4]
                {
                    test = 1, 9, 6, 2;
                }
                [testValueAsUInt]
                {
                    test = 534;
                }
                [testValueAsFloat]
                {
                    test = 3.141592654;
                }
                [testValueAsString]
                {
                    test = "this is a test string, with a , and a '";
                }
                [testValueAsFloat3]
                {
                    test = 1.25, 3.67, 9.99;
                }
                [testValueAsFloat4]
                {
                    test = 1.2, 3.6343, 9.1034, -99.324;
                }
            }
        )";

        if (Fuel doc; doc.load(*stream))
        {
            auto readValuesBlock = doc.child("readValues");
            REQUIRE(readValuesBlock != nullptr);

            // bool
            REQUIRE(readValuesBlock->child("testValueAsBool") != nullptr);
            CHECK_EQ(readValuesBlock->child("testValueAsBool")->valueAsBool("test"), true);

            // int
            REQUIRE(readValuesBlock->child("testValueAsInt") != nullptr);
            CHECK_EQ(readValuesBlock->child("testValueAsInt")->valueAsInt("test"), -543);

            // valueAsInt4
            REQUIRE(readValuesBlock->child("testValueAsInt4") != nullptr);
            auto testValues = readValuesBlock->child("testValueAsInt4")->valueAsInt4("test");
            CHECK(testValues[0] == 1); CHECK(testValues[1] == 9); CHECK(testValues[2] == 6); CHECK(testValues[3] == 2);

            // uint
            REQUIRE(readValuesBlock->child("testValueAsUInt") != nullptr);
            CHECK_EQ(readValuesBlock->child("testValueAsUInt")->valueAsInt("test"), 534);

            // float
            REQUIRE(readValuesBlock->child("testValueAsFloat") != nullptr);
            CHECK_EQ(readValuesBlock->child("testValueAsFloat")->valueAsFloat("test"), doctest::Approx(3.14159f));

            // string
            REQUIRE(readValuesBlock->child("testValueAsString") != nullptr);
            CHECK_EQ(readValuesBlock->child("testValueAsString")->valueAsString("test"), "this is a test string, with a , and a '");

            // valueAsFloat3
            REQUIRE(readValuesBlock->child("testValueAsFloat3") != nullptr);
            auto testValues3 = readValuesBlock->child("testValueAsFloat3")->valueAsFloat3("test");
            CHECK(testValues3[0] == doctest::Approx(1.25)); CHECK(testValues3[1] == doctest::Approx(3.67)); CHECK(testValues3[2] == doctest::Approx(9.99));

            // valueAsFloat4
            REQUIRE(readValuesBlock->child("testValueAsFloat4") != nullptr);
            auto testValues4 = readValuesBlock->child("testValueAsFloat4")->valueAsFloat4("test");
            CHECK(testValues4[0] == doctest::Approx(1.2)); CHECK(testValues4[1] == doctest::Approx(3.6343)); CHECK(testValues4[2] == doctest::Approx(9.1034)); CHECK(testValues4[3] == doctest::Approx(-99.324));
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
