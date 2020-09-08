
#include "ReaderWriterSNO.hpp"

#include <spdlog/spdlog.h>
#include <osg/Texture2D>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include "IFileSys.hpp"
#include "gas/Fuel.hpp"

#include <osg/MatrixTransform>
#include <osg/Notify>
#include <osg/Matrix>

namespace ehb
{
    ReaderWriterSNO::ReaderWriterSNO(IFileSys & fileSys) : fileSys(fileSys)
    {
        supportsExtension("sno", "Dungeon Siege SNO Mesh");
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
        const bool bSwap = (osg::getCpuByteOrder() == osg::BigEndian);

        uint32_t magic, version, unk1;

        stream.read((char *)&magic, sizeof(uint32_t));
        stream.read((char *)&version, sizeof(uint32_t));
        stream.read((char *)&unk1, sizeof(uint32_t));

        if (bSwap)
        {
            osg::swapBytes4((char *)&magic);
            osg::swapBytes4((char *)&version);
            osg::swapBytes4((char *)&unk1);
        }

        if (magic != 0x444F4E53) return ReadResult::FILE_NOT_HANDLED;

        // construct the actual mesh node
        osg::ref_ptr<SiegeNodeMesh> node = new SiegeNodeMesh;

        uint32_t doorCount, spotCount, cornerCount, faceCount, textureCount;
        float minX, minY, minZ, maxX, maxY, maxZ;
        float unk2, unk3, unk4;
        uint32_t unk5, unk6, unk7, unk8;
        float checksum;

        stream.read((char *)&doorCount, sizeof(uint32_t));
        stream.read((char *)&spotCount, sizeof(uint32_t));
        stream.read((char *)&cornerCount, sizeof(uint32_t));
        stream.read((char *)&faceCount, sizeof(uint32_t));
        stream.read((char *)&textureCount, sizeof(uint32_t));
        stream.read((char *)&minX, sizeof(float));
        stream.read((char *)&minY, sizeof(float));
        stream.read((char *)&minZ, sizeof(float));
        stream.read((char *)&maxX, sizeof(float));
        stream.read((char *)&maxY, sizeof(float));
        stream.read((char *)&maxZ, sizeof(float));
        stream.read((char *)&unk2, sizeof(float));
        stream.read((char *)&unk3, sizeof(float));
        stream.read((char *)&unk4, sizeof(float));
        stream.read((char *)&unk5, sizeof(uint32_t));
        stream.read((char *)&unk6, sizeof(uint32_t));
        stream.read((char *)&unk7, sizeof(uint32_t));
        stream.read((char *)&unk8, sizeof(uint32_t));
        stream.read((char *)&checksum, sizeof(float));

        if (bSwap)
        {
            osg::swapBytes4((char *)&doorCount);
            osg::swapBytes4((char *)&spotCount);
            osg::swapBytes4((char *)&cornerCount);
            osg::swapBytes4((char *)&faceCount);
            osg::swapBytes4((char *)&textureCount);
            osg::swapBytes4((char *)&minX);
            osg::swapBytes4((char *)&minY);
            osg::swapBytes4((char *)&minZ);
            osg::swapBytes4((char *)&maxX);
            osg::swapBytes4((char *)&maxY);
            osg::swapBytes4((char *)&maxZ);
            osg::swapBytes4((char *)&unk2);
            osg::swapBytes4((char *)&unk3);
            osg::swapBytes4((char *)&unk4);
            osg::swapBytes4((char *)&unk5);
            osg::swapBytes4((char *)&unk6);
            osg::swapBytes4((char *)&unk7);
            osg::swapBytes4((char *)&unk8);
            osg::swapBytes4((char *)&checksum);
        }

        for (uint32_t index = 0; index < doorCount; index++)
        {
            int32_t id, count;
            float a00, a01, a02, a10, a11, a12, a20, a21, a22, x, y, z;

            stream.read((char *)&id, sizeof(uint32_t));
            stream.read((char *)&x, sizeof(float));
            stream.read((char *)&y, sizeof(float));
            stream.read((char *)&z, sizeof(float));
            stream.read((char *)&a00, sizeof(float));
            stream.read((char *)&a01, sizeof(float));
            stream.read((char *)&a02, sizeof(float));
            stream.read((char *)&a10, sizeof(float));
            stream.read((char *)&a11, sizeof(float));
            stream.read((char *)&a12, sizeof(float));
            stream.read((char *)&a20, sizeof(float));
            stream.read((char *)&a21, sizeof(float));
            stream.read((char *)&a22, sizeof(float));
            stream.read((char *)&count, sizeof(uint32_t));

            if (bSwap)
            {
                osg::swapBytes4((char *)&id);
                osg::swapBytes4((char *)&x);
                osg::swapBytes4((char *)&y);
                osg::swapBytes4((char *)&z);
                osg::swapBytes4((char *)&a00);
                osg::swapBytes4((char *)&a01);
                osg::swapBytes4((char *)&a02);
                osg::swapBytes4((char *)&a10);
                osg::swapBytes4((char *)&a11);
                osg::swapBytes4((char *)&a12);
                osg::swapBytes4((char *)&a20);
                osg::swapBytes4((char *)&a21);
                osg::swapBytes4((char *)&a22);
                osg::swapBytes4((char *)&count);
            }

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

            stream.seekg(count * 4, std::ios::cur);
        }

        for (uint32_t index = 0; index < spotCount; index++)
        {
            std::string tmp;

            // rot, pos, string?
            stream.seekg(44, std::ios_base::cur);
            std::getline(stream, tmp, '\0');
        }

        // create vertex data per entire mesh
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(cornerCount);
        osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(cornerCount);
        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(cornerCount);
        osg::ref_ptr<osg::Vec2Array> tcoords = new osg::Vec2Array(cornerCount);

        for (uint32_t index = 0; index < cornerCount; index++)
        {
            float x, y, z, nX, nY, nZ, tX, tY;
            uint8_t r, g, b, a;

            stream.read((char *)&x, sizeof(float));
            stream.read((char *)&y, sizeof(float));
            stream.read((char *)&z, sizeof(float));

            stream.read((char *)&nX, sizeof(float));
            stream.read((char *)&nY, sizeof(float));
            stream.read((char *)&nZ, sizeof(float));

            // this is swizzled
            stream.read((char *)&r, sizeof(uint8_t));
            stream.read((char *)&b, sizeof(uint8_t));
            stream.read((char *)&g, sizeof(uint8_t));
            stream.read((char *)&a, sizeof(uint8_t));

            stream.read((char *)&tX, sizeof(float));
            stream.read((char *)&tY, sizeof(float));

            if (bSwap)
            {
                osg::swapBytes4((char *)&x);
                osg::swapBytes4((char *)&y);
                osg::swapBytes4((char *)&z);

                osg::swapBytes4((char *)&nX);
                osg::swapBytes4((char *)&nY);
                osg::swapBytes4((char *)&nZ);

                osg::swapBytes4((char *)&tX);
                osg::swapBytes4((char *)&tY);
            }

            (*vertices)[index].set(x, y, z);
            (*normals)[index].set(nX, nY, nZ);
            (*colors)[index].set(r, g, b, a);
            (*tcoords)[index].set(tX, 1 - tY); // adjust the y coord for osg
        }

        for (uint32_t index = 0; index < textureCount; index++)
        {
            std::string textureFileName;
            uint32_t start, span, count;

            std::getline(stream, textureFileName, '\0');
            stream.read((char *)&start, sizeof(uint32_t));
            stream.read((char *)&span, sizeof(uint32_t));
            stream.read((char *)&count, sizeof(uint32_t));

            if (bSwap)
            {
                osg::swapBytes4((char *)&start);
                osg::swapBytes4((char *)&span);
                osg::swapBytes4((char *)&count);
            }

            // create our unsigned short index data, as per the mesh format
            osg::ref_ptr<osg::DrawElementsUShort> elements = new osg::DrawElementsUShort(GL_TRIANGLES, static_cast<uint32_t>(count));

            // read in each index value and adjust for the global vertex list
            for (uint32_t j = 0; j < count; ++j)
            {
                uint16_t value;

                stream.read((char *)&value, sizeof(uint16_t));

                if (bSwap)
                {
                    osg::swapBytes2((char *)&value);
                }

                (*elements)[j] = start + value;
            }

            osg::Geometry * geometry = new osg::Geometry;

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

            auto log = spdlog::get("log");

            osg::StateSet* stateSet = nullptr;

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

                if (osg::Image* image = osgDB::readImageFile(textureFileName + ".raw"))
                {
                    auto texture = new osg::Texture2D(image);

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

        return node.release();
    }

    void SiegeNodeMesh::connect(osg::MatrixTransform* targetNode, uint32_t targetDoor, osg::MatrixTransform* connectNode, uint32_t connectDoor)
    {
        auto log = spdlog::get("log");

        // const auto targetMesh = dynamic_cast<SiegeNodeMesh *>(targetNode->getParent(0));
        // const auto connectMesh = dynamic_cast<SiegeNodeMesh *>(connectNode->getParent(0));

        const auto targetMesh = dynamic_cast<SiegeNodeMesh*>(targetNode->getChild(0));
        const auto connectMesh = dynamic_cast<SiegeNodeMesh*>(connectNode->getChild(0));

        if (!targetMesh) { log->error("SiegeNode::connect - targetNode has no SiegeNode parent"); return; }
        if (!connectMesh) { log->error("SiegeNode::connect - connectNode has no SiegeNode parent"); return; }

        const osg::Matrix* m1 = nullptr, * m2 = nullptr;

        for (const auto& entry : targetMesh->doorXform)
        {
            if (entry.first == targetDoor)
            {
                m1 = &entry.second;
                break;
            }
        }

        if (!m1) { log->error("couldn't find targetDoor {}", targetDoor); return; }

        for (const auto& entry : connectMesh->doorXform)
        {
            if (entry.first == connectDoor)
            {
                m2 = &entry.second;
                break;
            }
        }

        if (!m2) { log->error("couldn't find connectDoor {}", connectDoor); return; }

        osg::Matrix xform;

        /*
         * lets start at the location of the destination door
         * we want to use an inverse here to account for the fact that
         * we're currently placing the center of connectNode at the location
         * of its door and will be connecting this to door 1
         */
        xform = osg::Matrix::inverse(*m2);

        // account for flipping from door 1 to door 2
        xform.postMultRotate(osg::Quat(osg::DegreesToRadians(180.0), osg::Vec3(0, 1, 0)));

        // now transform by the first door...
        xform.postMult(*m1);

        // and adjust for the node we're connecting to
        xform.postMult(targetNode->getMatrix());

        // "Hold on to your butts." - Ray Arnold
        connectNode->setMatrix(xform);
    }
}
