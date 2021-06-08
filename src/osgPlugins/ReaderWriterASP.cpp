
#include "ReaderWriterASP.hpp"

#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include "IFileSys.hpp"
#include "BinaryReader.hpp"
#include "osg/Aspect.hpp"
#include "osg/AspectImpl.hpp"

namespace ehb
{
    ReaderWriterASP::ReaderWriterASP(IFileSys& fileSys) : fileSys(fileSys)
    {
        supportsExtension("asp", "Dungeon Siege ASP Mesh");

        log = spdlog::get("log");
    }

    const char* ReaderWriterASP::className() const {

        return "Dungeon Siege Aspect Mesh Reader";
    }

    bool ReaderWriterASP::acceptsExtension(const std::string& extension) const
    {
        return osgDB::equalCaseInsensitive(extension, "asp");
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterASP::readNode(const std::string& filename, const osgDB::Options* options) const
    {
        // do an extension check just in case a file reaches a loader it's not supposed to
        const std::string ext = osgDB::getLowerCaseFileExtension(filename);
        if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

        // try to find the file being requested
        const std::string fileName = osgDB::findDataFile(filename);
        if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

        InputStream stream = fileSys.createInputStream(fileName);
        if (!stream) return ReadResult::FILE_NOT_HANDLED;

        return readNode(*stream, options);
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterASP::readNode(std::istream& stream, const osgDB::Options* options) const
    {
        ByteArray data((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        BinaryReader reader(data);

        std::shared_ptr<Aspect::Impl> aspectImpl = std::make_shared<Aspect::Impl>();

        uint32_t currentSubMeshIndex;

        FourCC chunkId;
        while (reader.readFourCC(chunkId))
        {
            if (chunkId == "BMSH")
            {
                // version
                reader.skipBytes(4);

                aspectImpl->sizeTextField = reader.readUInt32();
                aspectImpl->boneCount = reader.readUInt32();
                aspectImpl->textureCount = reader.readUInt32();
                aspectImpl->vertexCount = reader.readUInt32();
                aspectImpl->subMeshCount = reader.readUInt32();
                aspectImpl->renderFlags = reader.readUInt32();

                log->debug("asp mesh has {} textures", aspectImpl->textureCount);

                ByteArray rawText(aspectImpl->sizeTextField);
                reader.readBytes(rawText.data(), rawText.size());

                size_t index = 0;
                aspectImpl->textureNames.resize(aspectImpl->textureCount);
                for (uint32_t i = 0; i < aspectImpl->textureCount; ++i)
                {
                    for (; index < rawText.size(); ++index)
                    {
                        const char c = static_cast<char>(rawText[index]);
                        if (c == '\0')
                        {
                            // Skip null padding for the next name:
                            while (index < rawText.size() && rawText[index] == 0)
                            {
                                ++index;
                            }
                            break;
                        }
                        aspectImpl->textureNames[i].push_back(c);
                    }

                    log->debug("textureNames[{}] = {}", i, aspectImpl->textureNames[i]);
                }

                aspectImpl->boneInfos.resize(aspectImpl->boneCount);
                for (uint32_t b = 0; b < aspectImpl->boneCount; ++b)
                {
                    for (; index < rawText.size(); ++index)
                    {
                        const char c = static_cast<char>(rawText[index]);
                        if (c == '\0')
                        {
                            // Skip null padding for the next name:
                            while (index < rawText.size() && rawText[index] == 0)
                            {
                                ++index;
                            }
                            break;
                        }
                        aspectImpl->boneInfos[b].name.push_back(c);
                    }
                }

                aspectImpl->subMeshes.resize(aspectImpl->subMeshCount);
            }
            else if (chunkId == "BONH")
            {
                // version
                reader.skipBytes(4);

                for (size_t b = 0; b < aspectImpl->boneInfos.size(); ++b)
                {
                    const auto boneIndex = reader.readUInt32();
                    const auto parentIndex = reader.readUInt32();
                    const auto boneFlags = reader.readUInt32();

                    aspectImpl->boneInfos[boneIndex].parentIndex = parentIndex;
                    aspectImpl->boneInfos[boneIndex].flags = boneFlags;
                }
            }
            else if (chunkId == "BSUB")
            {
                const auto version = reader.readUInt32();

                currentSubMeshIndex = reader.readUInt32();

                if (Aspect::Impl::versionOf(version) <= 40)
                {
                    currentSubMeshIndex += 1;
                }

                Aspect::Impl::SubMesh& mesh = aspectImpl->subMeshes[currentSubMeshIndex];

                mesh.textureCount = reader.readUInt32();
                mesh.vertexCount = reader.readUInt32();
                mesh.cornerCount = reader.readUInt32();
                mesh.faceCount = reader.readUInt32();
            }
            else if (chunkId == "BSMM")
            {
                reader.skipBytes(4);

                auto& mesh = aspectImpl->subMeshes[currentSubMeshIndex];
                mesh.textureCount = reader.readUInt32();

                mesh.matInfo.resize(mesh.textureCount);
                for (uint32_t t = 0; t < mesh.textureCount; ++t)
                {
                    mesh.matInfo[t].textureIndex = reader.readUInt32();
                    mesh.matInfo[t].faceSpan = reader.readUInt32();
                }
            }
            else if (chunkId == "BVTX")
            {
                // version
                reader.skipBytes(4);

                // another instance of vertexCount
                reader.skipBytes(4);

                auto& mesh = aspectImpl->subMeshes[currentSubMeshIndex];
                mesh.positions.resize(mesh.vertexCount);
                for (uint32_t v = 0; v < mesh.vertexCount; ++v)
                {
                    auto vert = reader.readVec3();

                    mesh.positions[v] = vert;
                }
            }
            else if (chunkId == "BCRN")
            {
                // version
                reader.skipBytes(4);

                reader.skipBytes(4);

                auto& mesh = aspectImpl->subMeshes[currentSubMeshIndex];

                mesh.corners.resize(mesh.cornerCount);
                for (uint32_t c = 0; c < mesh.cornerCount; ++c)
                {
                    auto& corner = mesh.corners[c];

                    // Vertex position:
                    corner.vtxIndex = reader.readUInt32();
                    if (corner.vtxIndex > mesh.positions.size())
                    {
                        corner.vtxIndex = static_cast<uint32_t>(mesh.positions.size() - 1);
                    }

                    corner.position = mesh.positions[corner.vtxIndex];

                    // Vertex normal, color:
                    corner.normal = reader.readVec3();
                    corner.color[0] = reader.readUInt8();
                    corner.color[1] = reader.readUInt8();
                    corner.color[2] = reader.readUInt8();
                    corner.color[3] = reader.readUInt8();

                    // Why did they leave this unused field here in the middle?
                    /* auto unused = */ reader.skipBytes(4);

                    // Float UVs:
                    corner.texCoord = reader.readVec2();
                    corner.texCoord.y() = 1 - corner.texCoord.y();
                }
            }
            else if (chunkId == "WCRN")
            {
                reader.skipBytes(4);

                reader.skipBytes(4);
                auto& mesh = aspectImpl->subMeshes[currentSubMeshIndex];

                mesh.wCorners.resize(mesh.cornerCount);
                for (uint32_t c = 0; c < mesh.cornerCount; ++c)
                {
                    auto& wCorner = mesh.wCorners[c];

                    wCorner.pos = reader.readVec3();
                    wCorner.weight = reader.readQuat();

                    wCorner.bone[0] = reader.readUInt8();
                    wCorner.bone[1] = reader.readUInt8();
                    wCorner.bone[2] = reader.readUInt8();
                    wCorner.bone[3] = reader.readUInt8();

                    // TODO: handle potential differnces for version < 40

                    wCorner.normal = reader.readVec3();

                    wCorner.color[0] = reader.readUInt8();
                    wCorner.color[1] = reader.readUInt8();
                    wCorner.color[2] = reader.readUInt8();
                    wCorner.color[3] = reader.readUInt8();

                    wCorner.texCoord = reader.readVec2();
                    wCorner.texCoord.y() = 1 - wCorner.texCoord.y();

                    /* TODO
                    // remove null bone/weights
                    // This is a reverse iteration, I guess, from 4 to 1 (or 0)
                    for i = 4 to 1 by -1 do
                    if (w[i] == 0) do
                    (
                        deleteItem w i
                        deleteItem b i
                    )
                    */
                }
            }
            else if (chunkId == "BVMP")
            {
            }
            else if (chunkId == "BTRI")
            {
                // version
                const auto version = reader.readUInt32();

                // skip faceCount
                reader.skipBytes(4);

                auto& mesh = aspectImpl->subMeshes[currentSubMeshIndex];

                if (Aspect::Impl::versionOf(version) == 22)
                {
                    mesh.faceInfo.cornerSpan.resize(mesh.textureCount);
                    for (uint32_t i = 0; i < mesh.textureCount; ++i)
                    {
                        mesh.faceInfo.cornerSpan[i] = reader.readUInt32();
                    }

                    mesh.faceInfo.cornerStart.resize(mesh.textureCount);
                    mesh.faceInfo.cornerStart[0] = 0;
                    for (uint32_t i = 0; i < mesh.textureCount - 1; ++i)
                    {
                        mesh.faceInfo.cornerStart[i] =
                            mesh.faceInfo.cornerStart[i] + mesh.faceInfo.cornerSpan[i];
                    }
                }
                else if (Aspect::Impl::versionOf(version) > 22)
                {
                    mesh.faceInfo.cornerStart.resize(mesh.textureCount);
                    mesh.faceInfo.cornerSpan.resize(mesh.textureCount);
                    for (uint32_t i = 0; i < mesh.textureCount; ++i)
                    {
                        mesh.faceInfo.cornerStart[i] = reader.readUInt32();
                        mesh.faceInfo.cornerSpan[i] = reader.readUInt32();
                    }
                }
                else
                {
                    mesh.faceInfo.cornerStart.resize(mesh.textureCount);
                    mesh.faceInfo.cornerSpan.resize(mesh.textureCount);
                    for (uint32_t i = 0; i < mesh.textureCount; ++i)
                    {
                        mesh.faceInfo.cornerStart[i] = 0;
                        mesh.faceInfo.cornerSpan[i] = mesh.cornerCount;
                    }
                }

                mesh.faceInfo.cornerIndex.resize(mesh.faceCount);
                for (uint32_t f = 0; f < mesh.faceCount; ++f)
                {
                    auto a = reader.readUInt32();
                    auto b = reader.readUInt32();
                    auto c = reader.readUInt32();

                    mesh.faceInfo.cornerIndex[f].index[0] = a;
                    mesh.faceInfo.cornerIndex[f].index[1] = b;
                    mesh.faceInfo.cornerIndex[f].index[2] = c;
                }
            }
            else if (chunkId == "RPOS")
            {
                // version
                reader.skipBytes(4);

                // numBones
                reader.skipBytes(4);

                aspectImpl->rposInfoAbI.resize(aspectImpl->boneInfos.size());
                aspectImpl->rposInfoRel.resize(aspectImpl->boneInfos.size());

                for (uint32_t i = 0; i < aspectImpl->boneInfos.size(); i++)
                {
                    Aspect::Impl::RPosInfo& abI = aspectImpl->rposInfoAbI[i];
                    abI.rotation = reader.readQuat();
                    abI.position = reader.readVec3();

                    Aspect::Impl::RPosInfo& rel = aspectImpl->rposInfoRel[i];
                    rel.rotation = reader.readQuat();
                    rel.position = reader.readVec3();
                }
            }
        }

        return new Aspect(std::move(aspectImpl));
    }
}
