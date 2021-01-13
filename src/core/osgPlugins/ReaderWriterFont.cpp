
#include "ReaderWriterFont.hpp"

#include <spdlog/spdlog.h>
#include <osg/Texture2D>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include "IFileSys.hpp"
#include "ui/ImageFont.hpp"

namespace ehb
{
    ReaderWriterFont::ReaderWriterFont(IFileSys & fileSys) : fileSys(fileSys)
    {
        supportsExtension("gas", "Dungeon Siege Gas");
    }

    const char * ReaderWriterFont::className() const
    {
        return "Dungeon Siege Font Reader";
    }

    bool ReaderWriterFont::acceptsExtension(const std::string & extension) const
    {
        return osgDB::equalCaseInsensitive(extension, "gas");
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterFont::readObject(const std::string & filename, const osgDB::Options * options) const
    {
        // do an extension check just in case a file reaches a loader it's not supposed to
        const std::string ext = osgDB::getLowerCaseFileExtension(filename);
        if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

        // try to find the file being requested
        const std::string fileName = osgDB::findDataFile(filename);
        if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

        InputStream stream = fileSys.createInputStream(fileName);
        if (!stream) return ReadResult::FILE_NOT_HANDLED;

        return readObject(*stream, options);
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterFont::readObject(std::istream & stream, const osgDB::Options * options) const
    {
        const osg::Vec4 PINK(1.f, 0.f, 1.f, 0.f);

        auto log = spdlog::get("log");

        // log->info("ReaderWriterFont::readObject(std::istream &)");

        if (options != nullptr)
        {
            const std::string font = options->getPluginStringData("font");

            if (Fuel doc; doc.load(stream))
            {
                if (auto node = doc.child(font))
                {
                    const int startRange = node->valueAsInt("startrange");
                    const int endRange = node->valueAsInt("endrange");
                    const int height = node->valueAsInt("height");
                    const std::string textureFileName = node->valueOf("texture");

                    if (auto image = osgDB::readRefImageFile(textureFileName + ".raw"))
                    {
                        const float imageWidth = image->s();
                        const float imageHeight = image->t();

                        osg::ref_ptr<ImageFont> font = new ImageFont(new osg::Texture2D(image), height);

                        font->setName(textureFileName);

                        int lastX = -1;
                        unsigned int charcode = startRange;

                        for (int y = 0; y < image->t(); y += height)
                        {
                            for (int x = 0; x < image->s(); x++)
                            {
                                if (image->getColor(x, y) == PINK)
                                {
                                    float xCoord = lastX + 1;
                                    float yCoord = y;
                                    float width = x - (lastX + 1);

                                    const float l = (xCoord + 0) / imageWidth;
                                    const float b = (yCoord + 0) / imageHeight;
                                    const float r = (xCoord + width) / imageWidth;
                                    const float t = (yCoord + height) / imageHeight;

                                    font->addGlyph(charcode, { static_cast<unsigned int>(width), l, b, r, t });

                                    charcode++;
                                    lastX = x;

                                    if (charcode == endRange) break;
                                }
                            }

                            lastX = -1;

                            if (charcode == endRange) break;
                        }

                        return font.release();
                    }
                    else
                    {
                        return ReadResult::ERROR_IN_READING_FILE;
                    }
                }
                else
                {
                    return ReadResult::FILE_NOT_FOUND;
                }
            }
            else
            {
                return ReadResult::ERROR_IN_READING_FILE;
            }
        }

        return ReadResult::FILE_NOT_HANDLED;
    }
}
