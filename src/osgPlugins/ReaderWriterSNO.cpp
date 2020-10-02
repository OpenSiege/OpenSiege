
#include "ReaderWriterSNO.hpp"

#include "osg/SiegeNodeMesh.hpp"

#include <osg/Texture2D>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include "IFileSys.hpp"
#include "gas/Fuel.hpp"
#include "BinaryReader.hpp"

#include <osg/MatrixTransform>
#include <osg/Notify>
#include <osg/Matrix>

namespace ehb
{
    ReaderWriterSNO::ReaderWriterSNO(IFileSys & fileSys) : fileSys(fileSys)
    {
        supportsExtension("sno", "Dungeon Siege SNO Mesh");

        log = spdlog::get("log");
    }

    const char * ReaderWriterSNO::className() const {

        return "Dungeon Siege Siege Node Reader";
    }

    bool ReaderWriterSNO::acceptsExtension(const std::string & extension) const
    {
        return osgDB::equalCaseInsensitive(extension, "sno");
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterSNO::readNode(const std::string & filename, const osgDB::Options * options) const
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

    osgDB::ReaderWriter::ReadResult ReaderWriterSNO::readNode(std::istream & stream, const osgDB::Options * options) const
    {
        ByteArray data((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        BinaryReader reader(data);

        uint32_t magic = reader.readUInt32();
        uint32_t version = reader.readUInt32();
        uint32_t unk1 = reader.readUInt32();

        if (magic != 0x444F4E53) return ReadResult::FILE_NOT_HANDLED;

        // construct the actual mesh node
        osg::ref_ptr<SiegeNodeMesh> node = new SiegeNodeMesh;

        uint32_t doorCount = reader.readUInt32(), spotCount = reader.readUInt32(), cornerCount = reader.readUInt32(), faceCount = reader.readUInt32(), textureCount = reader.readUInt32();
        float minX = reader.readFloat32(), minY = reader.readFloat32(), minZ = reader.readFloat32(), maxX = reader.readFloat32(), maxY = reader.readFloat32(), maxZ = reader.readFloat32();
        float unk2 = reader.readFloat32(), unk3 = reader.readFloat32(), unk4 = reader.readFloat32();
        uint32_t unk5 = reader.readUInt32(), unk6 = reader.readUInt32(), unk7 = reader.readUInt32(), unk8 = reader.readUInt32();
        float checksum = reader.readFloat32();

        for (uint32_t index = 0; index < doorCount; index++)
        {
            uint32_t id = reader.readUInt32();
            float x = reader.readFloat32(), y = reader.readFloat32(), z = reader.readFloat32();
            float a00 = reader.readFloat32(), a01 = reader.readFloat32(), a02 = reader.readFloat32();
            float a10 = reader.readFloat32(), a11 = reader.readFloat32(), a12 = reader.readFloat32();
            float a20 = reader.readFloat32(), a21 = reader.readFloat32(), a22 = reader.readFloat32(); 
            uint32_t count = reader.readUInt32();
            /*
             * this is pretty straight forward but just documenting that osg and
             * dungeon siege node transformation matrices ARE compatible so no funky
             * modifications are required here
             */
            osg::Matrix xform;

            xform(0, 0) = a00;
            xform(0, 1) = a01;
            xform(0, 2) = a02;
            xform(1, 0) = a10;
            xform(1, 1) = a11;
            xform(1, 2) = a12;
            xform(2, 0) = a20;
            xform(2, 1) = a21;
            xform(2, 2) = a22;
            xform(3, 0) = x;
            xform(3, 1) = y;
            xform(3, 2) = z;

            node->doorXform.emplace_back(id, std::move(xform));

            reader.skipBytes(count * 4);
        }

        for (uint32_t index = 0; index < spotCount; index++)
        {
            // rot, pos, string?
            reader.skipBytes(44);
            std::string tmp = reader.readString();
        }

        // create vertex data per entire mesh
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(cornerCount);
        osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(cornerCount);
        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(cornerCount);
        osg::ref_ptr<osg::Vec2Array> tcoords = new osg::Vec2Array(cornerCount);

        for (uint32_t index = 0; index < cornerCount; index++)
        {
            float x = reader.readFloat32(), y = reader.readFloat32(), z = reader.readFloat32();
            float nX = reader.readFloat32(), nY = reader.readFloat32(), nZ = reader.readFloat32();
            uint8_t r = reader.readUInt8(), g = reader.readUInt8(), b = reader.readUInt8(), a = reader.readUInt8();
            float tX = reader.readFloat32(), tY = reader.readFloat32();

            (*vertices)[index].set(x, y, z);
            (*normals)[index].set(nX, nY, nZ);
            (*colors)[index].set(r, g, b, a);
            (*tcoords)[index].set(tX, 1 - tY); // adjust the y coord for osg
        }

        for (uint32_t index = 0; index < textureCount; index++)
        {
            std::string textureFileName = reader.readString();
            uint32_t start = reader.readUInt32(), span = reader.readUInt32(), count = reader.readUInt32();

            // create our unsigned short index data, as per the mesh format
            osg::ref_ptr<osg::DrawElementsUShort> elements = new osg::DrawElementsUShort(GL_TRIANGLES, static_cast<uint32_t>(count));

            // read in each index value and adjust for the global vertex list
            for (uint32_t j = 0; j < count; ++j)
            {
                uint16_t value = reader.readUInt16();

                (*elements)[j] = start + value;
            }

            osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;

            std::string texSetAbbr;

            if (options != nullptr)
            {
                texSetAbbr = options->getPluginStringData("texsetabbr");

                if (!texSetAbbr.empty())
                {
                    if (const auto itr = textureFileName.find("_xxx_"); itr != std::string::npos)
                    {
                        textureFileName.replace(itr + 1, 3, texSetAbbr);
                    }
                }
            }

            textureFileName = osgDB::convertToLowerCase(textureFileName);
            const std::string tsdFileName = osgDB::findDataFile(textureFileName + ".gas");

            // set the name of the geometry to the texture filename for debugging and visitor purposes
            geometry->setName(textureFileName);

            osg::ref_ptr<osg::StateSet> stateSet = nullptr;

            if (auto stream = fileSys.createInputStream(tsdFileName))
            {
                // read the corresponding TSD file
                if (Fuel doc; doc.load(*stream))
                {
                    if (auto tsd = doc.child(textureFileName))
                    {
                        // TODO: create OSG state sets
                    }
                }
            }

            if (stateSet == nullptr)
            {
                log->warn("{}.gas not found falling back to {}.raw", textureFileName, textureFileName);

                if (osg::ref_ptr<osg::Image> image = osgDB::readImageFile(textureFileName + ".raw"))
                {
                    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);

                    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
                    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

                    geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
                }
            }

            // set our geometry pointers
            geometry->setVertexArray(vertices.get());
            geometry->setColorArray(colors.get());
            geometry->setNormalArray(normals.get(), osg::Array::BIND_PER_VERTEX);

            // most nodes have 2 layers so we will map 2 units for now
            geometry->setTexCoordArray(0, tcoords.get());
            geometry->setTexCoordArray(1, tcoords.get());

            // add index data to the geometry
            geometry->addPrimitiveSet(elements.get());

            node->addChild(geometry);
        }

        uint32_t logicalGroupingCount = reader.readUInt32();
        node->logicalNodeGroupings.resize(logicalGroupingCount);

        for (uint32_t i = 0; i < logicalGroupingCount; i++)
        {
            auto& logicalNodeGrouping = node->logicalNodeGroupings.at(i);

            logicalNodeGrouping.id = reader.readUInt8();

            logicalNodeGrouping.bbox._min.x() = reader.readFloat32();
            logicalNodeGrouping.bbox._min.y() = reader.readFloat32();
            logicalNodeGrouping.bbox._min.z() = reader.readFloat32();

            logicalNodeGrouping.bbox._max.x() = reader.readFloat32();
            logicalNodeGrouping.bbox._max.y() = reader.readFloat32();
            logicalNodeGrouping.bbox._max.z() = reader.readFloat32();

            logicalNodeGrouping.flag = (SiegeNodeMesh::FloorFlag)reader.readUInt32();

            // no idea what this is yet
            uint32_t unkCount1 = reader.readUInt32();
            for (uint32_t j = 0; j < unkCount1; ++j)
            {
                uint16_t index = reader.readUInt16();

                float unkRotation[9]; reader.readBytes(&unkRotation, sizeof(float) * 9);

                uint16_t unkShortArrayCount1 = reader.readUInt16();
                reader.skipBytes(sizeof(uint16_t) * unkShortArrayCount1);

                uint32_t unkShortArrayCount2 = reader.readUInt32();
                reader.skipBytes(sizeof(uint16_t) * unkShortArrayCount2);
            }

            uint32_t unkShortPairSectionCount = reader.readUInt32();
            for (uint32_t j = 0; j < unkShortPairSectionCount; ++j)
            {
                uint8_t unkByte = reader.readUInt8();

                uint32_t unkCount2 = reader.readUInt32();

                reader.skipBytes((sizeof(uint16_t) * unkCount2) * 2);
            }

            uint32_t triangleCount = reader.readUInt32();
            logicalNodeGrouping.logicalNodeFaces.resize(triangleCount);
            for (uint32_t j = 0; j < triangleCount; ++j)
            {
                SiegeNodeMesh::Face& face = logicalNodeGrouping.logicalNodeFaces.at(j);
                face.a = reader.readVec3();
                face.b = reader.readVec3();
                face.c = reader.readVec3();

                face.normal = reader.readVec3();
            }

            recurse_unknown_section(reader);
        }

        return node.release();
    }

    void ReaderWriterSNO::recurse_unknown_section(BinaryReader& reader) const
    {
        float unkbBox[6]; reader.readBytes(unkbBox, sizeof(float) * 6);

        uint8_t unkByte1 = reader.readUInt8();

        uint16_t unkCount3 = reader.readUInt16();
        std::vector<uint16_t> unkArrayOfShorts;
        for (uint32_t i = 0; i < unkCount3; i++)
        {
            uint16_t value = reader.readUInt16();
            unkArrayOfShorts.push_back(value);
        }

        uint8_t unkByte2 = reader.readUInt8();
        for (uint8_t i = 0; i < unkByte2; i++)
        {
            recurse_unknown_section(reader);
        }
    }
}
