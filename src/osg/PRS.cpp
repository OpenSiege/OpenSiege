
#include "PRS.hpp"

#include <algorithm>

namespace ehb
{
    PRS::PRS(ByteArray fileContents, std::string filename)
    {
        auto log = spdlog::get("log");

        BinaryReader reader(fileContents);

        FourCC chunkId;
        while (reader.readFourCC(chunkId))
        {
            if (chunkId == "ANIM")
            {
                const auto version = reader.readUInt32();
                const auto sizeTextField = reader.readUInt32();
                const auto boneCount = reader.readUInt32();
                const auto length = reader.readFloat32();

                log->debug("reading animation with sizeTextField = {}, boneCount = {}, length = {}", sizeTextField, boneCount, length);

                // no idea what these bytes are
                reader.skipBytes(48);

                ByteArray rawText(sizeTextField);
                reader.readBytes(rawText.data(), rawText.size());

                size_t index = 0;

                boneInfos.resize(boneCount);
                for (uint32_t i = 0; i < boneCount; i++)
                {
                    std::string targetName;
                    for (; index < rawText.size(); ++index)
                    {
                        const char c = static_cast<char>(rawText[index]);
                        if (c == '\0')
                        {
                            while (index < rawText.size() && rawText[index] == 0)
                            {
                                ++index;
                            }
                            break;
                        }

                        boneInfos[i].boneName.push_back(c);
                    }

                }

                kListSeq.resize(boneCount);

                std::transform(std::begin(rawText), std::end(rawText), std::begin(rawText),
                    [](uint8_t b) { return (b != 0) ? b : '-';  });

                rawText.push_back(0);
                log->debug("raw text: {}", rawText.data());

                for (auto bone : boneInfos)
                {
                    log->debug("bone name: {}", bone.boneName);
                }

            }
            else if (chunkId == "NOTE")
            {
                // don't care about notes for now
            }
            else if (chunkId == "TRCR")
            {
                // don't care about tracers for now
            }
            else if (chunkId == "V3_0")
            {
                assert(true);
            }
            else if (chunkId == "RKEY")
            {
                const auto version = reader.readUInt32();

                const auto rotKeyCount = reader.readUInt32();
                const auto posKeyCount = reader.readUInt32();

                log->debug("rootKey has {} positional keys and {} rotational keys", posKeyCount, rotKeyCount);

                rKeyListSeq.rotKeys.resize(rotKeyCount);
                for (auto& key : rKeyListSeq.rotKeys)
                {
                    key.time = reader.readFloat32();
                    key.rotation = reader.readQuat();
                }

                rKeyListSeq.posKeys.resize(posKeyCount);
                for (auto& key : rKeyListSeq.posKeys)
                {
                    key.time = reader.readFloat32();
                    key.position = reader.readVec3();
                }
            }
            else if (chunkId == "KLST")
            {
                const auto version = reader.readUInt32();

                const auto boneIndex = reader.readUInt32();
                const auto dummy = reader.readUInt32(); // ???

                const auto rotKeyCount = reader.readUInt32();
                const auto posKeyCount = reader.readUInt32();

                log->debug("bone {} has {} positional keys and {} rotation keys", boneIndex, posKeyCount, rotKeyCount);

                AnimSeq& seq = kListSeq[boneIndex];

                seq.rotKeys.resize(rotKeyCount);
                seq.posKeys.resize(posKeyCount);

                for (auto& key : seq.rotKeys)
                {
                    key.time = reader.readFloat32();
                    key.rotation = reader.readQuat();
                }

                for (auto& key: seq.posKeys)
                {
                    key.time = reader.readFloat32();
                    key.position = reader.readVec3();
                }

            }
            else if (chunkId == "AEND")
            {
                // don't care about debug data for now
            }
            else
            {
                assert(true);
            }
        }
    }
}