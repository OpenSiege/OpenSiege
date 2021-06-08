
#pragma once

#include <cstdint>
#include <vector>
#include <osg/Vec3>
#include <osg/Quat>
#include "osgPlugins/BinaryReader.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    // I'd like to get rid of the below to cut out the intermediate but this is good in case we need to query the animation
    struct PRS final
    {
        PRS(ByteArray fileContents, std::string filename = "");
        ~PRS() = default;

        // "Raw" version numbers of the ASP sections.
        // There are also alternate versions that can be fetched by 'versionOf()'.
        struct Version
        {
            enum Enum
            {
                null = 0,
                v1_2 = 513,
                v1_3 = 769,
                v2_0 = 2,
                v2_1 = 258,
                v2_2 = 514,
                v2_3 = 770,
                v2_4 = 1026,
                v2_5 = 1282,
                v3_0 = 3, // prs
                v4_0 = 4,
                v4_1 = 260,
                v5_0 = 5 // DS Legends of Aranna expansion.
            };
        };

        static uint32_t versionOf(const uint32_t v)
        {
            // Adapted from "PRSImport.ms".
            // The question to ask here is, why didn't
            // they save these numbers in the file, instead
            // of the wacky version constants?
            switch (v)
            {
            case Version::v1_2: return 12;
            case Version::v1_3: return 13;
            case Version::v2_0: return 20;
            case Version::v2_1: return 21;
            case Version::v2_2: return 22;
            case Version::v2_3: return 23;
            case Version::v2_4: return 24;
            case Version::v2_5: return 25;
            case Version::v3_0:  return 30;
            case Version::v4_0: return 40;
            case Version::v4_1: return 41;
            case Version::v5_0: return 50;
            default: return Version::null;
            } // switch (v)
        }

        // I'd like to get rid of the below to cut out the intermediate but this is good in case we need to query the animation
        struct AnimSeq final
        {
            struct RotKey
            {
                float time = 0.0f;
                osg::Quat rotation { 0, 0, 0, 1 };
            };

            struct PosKey
            {
                float time = 0.0f;
                osg::Vec3 position { 0, 0, 0 };
            };

            std::vector<RotKey> rotKeys;
            std::vector<PosKey> posKeys;
        };

        struct BoneInfo { std::string boneName; };
        struct NoteInfo { float time = 0.0f; uint32_t mark = 0; };
        struct TrcrInfo { float unk[12]; };

        float duration = 0.0f; // this should match the osgAnimation value

        std::vector<BoneInfo> boneInfos;

        // TODO: notes and tracers

        //! t-pos?
        AnimSeq rKeyListSeq;

        //! each index should correspond to a bone index
        std::vector<AnimSeq> kListSeq;
            
        //! debug data
        std::string filename;
    };
}