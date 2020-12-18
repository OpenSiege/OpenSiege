
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
                [ValueAsBool]
                {
                    test = true;
                }
                [ValueAsInt]
                {
                    test = -543;
                }
                [ValueAsInt4]
                {
                    test = 1, 9, 6, 2;
                }
                [ValueAsUInt]
                {
                    test = 534;
                }
                [ValueAsFloat]
                {
                    test = 3.141592654;
                }
                [ValueAsString]
                {
                    test = "this is a test string, with a , and a '";
                }
                [ValueAsFloat3]
                {
                    test = 1.25, 3.67, 9.99;
                }
                [ValueAsFloat4]
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
            REQUIRE(readValuesBlock->child("ValueAsBool") != nullptr);
            CHECK_EQ(readValuesBlock->child("ValueAsBool")->valueAsBool("test"), true);

            // int
            REQUIRE(readValuesBlock->child("ValueAsInt") != nullptr);
            CHECK_EQ(readValuesBlock->child("ValueAsInt")->valueAsInt("test"), -543);

            // valueAsInt4
            REQUIRE(readValuesBlock->child("ValueAsInt4") != nullptr);
            auto testValues = readValuesBlock->child("ValueAsInt4")->valueAsInt4("test");
            CHECK(testValues[0] == 1); CHECK(testValues[1] == 9); CHECK(testValues[2] == 6); CHECK(testValues[3] == 2);

            // uint
            REQUIRE(readValuesBlock->child("ValueAsUInt") != nullptr);
            CHECK_EQ(readValuesBlock->child("ValueAsUInt")->valueAsInt("test"), 534);

            // float
            REQUIRE(readValuesBlock->child("ValueAsFloat") != nullptr);
            CHECK_EQ(readValuesBlock->child("ValueAsFloat")->valueAsFloat("test"), doctest::Approx(3.14159f));

            // string
            REQUIRE(readValuesBlock->child("ValueAsString") != nullptr);
            CHECK_EQ(readValuesBlock->child("ValueAsString")->valueAsString("test"), "this is a test string, with a , and a '");

            // valueAsFloat3
            REQUIRE(readValuesBlock->child("ValueAsFloat3") != nullptr);
            auto testValues3 = readValuesBlock->child("ValueAsFloat3")->valueAsFloat3("test");
            CHECK(testValues3[0] == doctest::Approx(1.25)); CHECK(testValues3[1] == doctest::Approx(3.67)); CHECK(testValues3[2] == doctest::Approx(9.99));

            // valueAsFloat4
            REQUIRE(readValuesBlock->child("ValueAsFloat4") != nullptr);
            auto testValues4 = readValuesBlock->child("ValueAsFloat4")->valueAsFloat4("test");
            CHECK(testValues4[0] == doctest::Approx(1.2)); CHECK(testValues4[1] == doctest::Approx(3.6343)); CHECK(testValues4[2] == doctest::Approx(9.1034)); CHECK(testValues4[3] == doctest::Approx(-99.324));
        }

        std::stringstream().swap(*stream);
        *stream << R"(
            [t:template,n:bd_ch_f_g_c_avg]
            {
	            doc = "Thick Chain";
	            specializes = base_body_armor_chain;
	            [common]
	            {
	              b allow_modifiers = false;
		            screen_name = "Thick Chain";
	            }
	            [defend]
	            {
		            armor_style = 004;
		            armor_type = a4;
	              f defense = 135.000000;
	            }
	            [gui]
	            {
		            equip_requirements = strength:24;
		            inventory_height = 3;
		            inventory_icon = b_gui_ig_i_a_pos_a4_004;
		            inventory_width = 2;
	            }
	            [pcontent]
	            {
		            [base]
		            {
		              f modifier_max = 0.000000;
		              f modifier_min = 0.000000;
		            }
		            [c_fin]
		            {
			            armor_style = 027;
			            armor_type = a3;
		              f defense = 147.000000;
			            equip_requirements = strength:25;
			            inventory_height = 2;
			            inventory_icon = b_gui_ig_i_a_pos_a3_027;
			            inventory_width = 2;
		              f modifier_max = 6.000000;
		              f modifier_min = 1.000000;
		            }
		            [c_str]
		            {
			            armor_style = 007;
			            armor_type = a3;
		              f defense = 241.000000;
			            equip_requirements = strength:34;
			            inventory_icon = b_gui_ig_i_a_pos_a3_007;
		              f modifier_max = 17.000000;
		              f modifier_min = 9.000000;
		            }
		            [o_fin]
		            {
			            armor_style = 028;
			            armor_type = a3;
		              f defense = 204.000000;
			            equip_requirements = strength:31;
			            inventory_height = 2;
			            inventory_icon = b_gui_ig_i_a_pos_a3_028;
			            inventory_width = 2;
		              f modifier_max = 14.000000;
		              f modifier_min = 7.000000;
			            pcontent_special_type = rare, unique, normal;
		            }
	            }
            }
        )";

        if (Fuel doc; doc.load(*stream))
        {
            auto bd_ch_f_g_c_avg = doc.child("bd_ch_f_g_c_avg");
            REQUIRE(bd_ch_f_g_c_avg != nullptr);

            Fuel newFuelDoc;
            auto new_test_doc = newFuelDoc.appendChild("new_test_doc");

            // merge will overwrite properties that already exist
            bd_ch_f_g_c_avg->merge(new_test_doc);

            CHECK_EQ(new_test_doc->name(), "bd_ch_f_g_c_avg");
            CHECK_EQ(new_test_doc->type(), "template");

            auto common = new_test_doc->child("common");
            REQUIRE(common != nullptr);
            {
                CHECK_EQ(common->valueAsBool("allow_modifiers"), false);
                CHECK_EQ(common->valueAsString("screen_name"), "Thick Chain");
            }

            auto defend = new_test_doc->child("defend");
            REQUIRE(defend != nullptr);
            {
                CHECK_EQ(defend->valueOf("armor_style"), "004");
                CHECK_EQ(defend->valueOf("armor_type"), "a4");
                CHECK(defend->valueAsFloat("defense") == doctest::Approx(135.0f));
            }

            auto gui = new_test_doc->child("gui");
            REQUIRE(gui != nullptr);
            {
                CHECK_EQ(gui->valueOf("equip_requirements"), "strength:24");
                CHECK_EQ(gui->valueAsUInt("inventory_height"), 3);
                CHECK_EQ(gui->valueOf("inventory_icon"), "b_gui_ig_i_a_pos_a4_004");
                CHECK_EQ(gui->valueAsUInt("inventory_width"), 2);
            }

            auto pcontent = new_test_doc->child("pcontent");
            REQUIRE(pcontent != nullptr);
            {
                auto base = pcontent->child("base");
                REQUIRE(base != nullptr);
                {
                    CHECK(base->valueAsFloat("modifier_max") == doctest::Approx(0.0f));
                    CHECK(base->valueAsFloat("modifier_min") == doctest::Approx(0.0f));
                }

                auto c_fin = pcontent->child("c_fin");
                REQUIRE(c_fin != nullptr);
                {
                    CHECK_EQ(c_fin->valueOf("armor_style"), "027");
                    CHECK_EQ(c_fin->valueOf("armor_type"), "a3");
                    CHECK(c_fin->valueAsFloat("defense") == doctest::Approx(147.0f));
                    CHECK_EQ(c_fin->valueOf("equip_requirements"), "strength:25");
                    CHECK_EQ(c_fin->valueAsUInt("inventory_height"), 2);
                    CHECK_EQ(c_fin->valueOf("inventory_icon"), "b_gui_ig_i_a_pos_a3_027");
                    CHECK_EQ(c_fin->valueAsUInt("inventory_width"), 2);
                    CHECK(c_fin->valueAsFloat("modifier_max") == doctest::Approx(6.0f));
                    CHECK(c_fin->valueAsFloat("modifier_min") == doctest::Approx(1.0f));
                }

                auto c_str = pcontent->child("c_str");
                REQUIRE(c_str != nullptr);
                {
                    CHECK_EQ(c_str->valueOf("armor_style"), "007");
                    CHECK_EQ(c_str->valueOf("armor_type"), "a3");
                    CHECK(c_str->valueAsFloat("defense") == doctest::Approx(241.0f));
                    CHECK_EQ(c_str->valueOf("equip_requirements"), "strength:34");
                    CHECK_EQ(c_str->valueOf("inventory_icon"), "b_gui_ig_i_a_pos_a3_007");
                    CHECK(c_str->valueAsFloat("modifier_max") == doctest::Approx(17.0f));
                    CHECK(c_str->valueAsFloat("modifier_min") == doctest::Approx(9.0f));
                }

                auto o_fin = pcontent->child("o_fin");
                REQUIRE(o_fin != nullptr);
                {
                    CHECK_EQ(o_fin->valueOf("armor_style"), "028");
                    CHECK_EQ(o_fin->valueOf("armor_type"), "a3");
                    CHECK(o_fin->valueAsFloat("defense") == doctest::Approx(204.0f));
                    CHECK_EQ(o_fin->valueOf("equip_requirements"), "strength:31");
                    CHECK_EQ(o_fin->valueAsUInt("inventory_height"), 2);
                    CHECK_EQ(o_fin->valueOf("inventory_icon"), "b_gui_ig_i_a_pos_a3_028");
                    CHECK_EQ(o_fin->valueAsUInt("inventory_width"), 2);
                    CHECK(o_fin->valueAsFloat("modifier_max") == doctest::Approx(14.0f));
                    CHECK(o_fin->valueAsFloat("modifier_min") == doctest::Approx(7.0f));
                    CHECK_EQ(o_fin->valueOf("pcontent_special_type"), "rare, unique, normal");
                }
            }
        }

        // multiple wild card block test
        std::stringstream().swap(*stream);
        *stream << R"(
            [t:unit_test,n:multiple_wildcard_merge]
            {
                [magic]
                {
                    [enchantments]
                    {
                        [*]
                        {
                            alteration = alter_life_recovery_unit;
                            description = "+1 to Health Regeneration";
                            duration = #infinite;
                            is_permanent = true;
                            is_single_instance = false;
                            value = 0.65;
                        }
                        [*]
                        {
                            alteration = alter_mana_recovery_unit;
                            description = "+1 to Mana Regeneration";
                            duration = #infinite;
                            is_permanent = true;
                            is_single_instance = false;
                            value = 0.4;
                        }
                        [*]
                        {
                            alteration = alter_intelligence;
                            description = "Adds 2 to Intelligence";
                            duration = #infinite;
                            is_permanent = true;
                            is_single_instance = false;
                            value = 2;
                        }
                        [*]
                        {
                            alteration = alter_armor;
                            description = "Adds 20 to Armor";
                            duration = #infinite;
                            is_permanent = true;
                            is_single_instance = false;
                            value = 20;
                        }
                    }
                }
            }
        )";

        if (Fuel doc; doc.load(*stream))
        {
            auto multiple_wildcard_merge = doc.child("multiple_wildcard_merge");
            REQUIRE(multiple_wildcard_merge != nullptr);

            // quick asserts to make sure the appropriate blocks got loaded
            auto wildcards = multiple_wildcard_merge->child("magic:enchantments");
            REQUIRE(wildcards != nullptr);
            {
                REQUIRE_EQ(wildcards->eachChild().size(), 4);
            }

            Fuel newFuelDoc;
            auto new_test_doc = newFuelDoc.appendChild("multiple_wildcard_merge_clone");

            // since we are using an empty block merge actually becomes a clone so you will get the correct result
            multiple_wildcard_merge->merge(new_test_doc);
            {
                auto new_wildcards = new_test_doc->child("magic:enchantments");
                REQUIRE(new_wildcards != nullptr);

                REQUIRE_EQ(new_wildcards->eachChild().size(), 4);
            }

            new_test_doc = newFuelDoc.appendChild("multiple_wildcard_merge_append");
            auto magic = new_test_doc->appendChild("magic"); // add magic to parent
            auto enchantments = magic->appendChild("enchantments"); // enchantments to parent
            auto test_block = new_test_doc->child("magic:enchantments"); // re-grab just to ensure everything is setup properly
            REQUIRE(test_block != nullptr);
            {
                // loop each enchantment and attempt to merge it
                // this will make sure when multiple wildcard characters are encountered we get unique blocks
                // we are reading from the original fuel string here
                if (!multiple_wildcard_merge->eachChildOf("magic").empty())
                {
                    multiple_wildcard_merge->child("magic")->merge(magic);
                }

                REQUIRE_EQ(test_block->eachChild().size(), 4);
            }

            new_test_doc = newFuelDoc.appendChild("multiple_wildcard_integrate");
            new_test_doc->integrate(multiple_wildcard_merge);

            // re-run the above test
            {
                auto new_wildcards = new_test_doc->child("magic:enchantments");
                REQUIRE(new_wildcards != nullptr);

                REQUIRE_EQ(new_wildcards->eachChild().size(), 4);
            }
        }
    }

        {
            std::string tmpl = R"(
	[t:template,n:3W_dsx_skeleton_04]{

	[inventory]
		{
		[pcontent]
	{
		[oneof*]
		{
			[gold*]
			{
				chance = 0.2;
				min = 143018;
				max = 286036;
			}
			[oneof*]
			{
				chance = 0.088888888888889;
				il_main = potion_mana_super;
				il_main = potion_health_super;
			}
			[oneof*]
			{
				chance = 0.15;
				il_main = #weapon/186;
				il_main = #armor/45-510;
				il_main = #*/186;
			}
			[oneof*]
			{
				chance = 0.0075;
				il_main = #weapon/-rare(1)/225-265;
				il_main = #armor/-rare(1)/0-645;
				il_main = #*/-rare(1)/225-265;
			}
			
		}
	}
	}	
		}
})";
            auto stream = std::make_unique<std::stringstream>();
            *stream << tmpl;

            if (Fuel doc; doc.load(*stream))
            {
            }
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
