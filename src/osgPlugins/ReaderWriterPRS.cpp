
#include "ReaderWriterPRS.hpp"

#include <osgAnimation/Animation>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include "IFileSys.hpp"
#include "BinaryReader.hpp"
#include "osg/PRS.hpp"

namespace ehb
{
    ReaderWriterPRS::ReaderWriterPRS(IFileSys & fileSys) : fileSys(fileSys)
    {
        supportsExtension("prs", "Dungeon Siege PRS Animation");
    }

    const char * ReaderWriterPRS::className() const {

        return "Dungeon Siege Animation Reader";
    }

    bool ReaderWriterPRS::acceptsExtension(const std::string & extension) const
    {
        return osgDB::equalCaseInsensitive(extension, "prs");
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterPRS::readObject(const std::string & filename, const osgDB::Options * options) const
    {
        // do an extension check just in case a file reaches a loader it's not supposed to
        const std::string ext = osgDB::getLowerCaseFileExtension(filename);
        if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

        // try to find the file being requested
        const std::string fileName = osgDB::findDataFile(filename);
        if (fileName.empty()) return ReadResult::FILE_NOT_HANDLED;

        InputStream stream = fileSys.createInputStream(fileName);
        if (!stream) return ReadResult::FILE_NOT_HANDLED;

        // don't return ReadResult::FILE_NOT_FOUND because it will spam output
        if (stream->rdbuf()->in_avail() == 0) ReadResult::FILE_NOT_FOUND;

        spdlog::get("log")->info("attempting to read prs: {}", filename);

        return readObject(*stream, options);
    }

    osgDB::ReaderWriter::ReadResult ReaderWriterPRS::readObject(std::istream & stream, const osgDB::Options * options) const
    {
        auto log = spdlog::get("log");

        ByteArray data((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        if (data.empty()) return ReadResult::FILE_NOT_HANDLED;

        PRS anim(data);

        osg::ref_ptr<osgAnimation::Animation> animation = new osgAnimation::Animation;

        // it's important to set the duration of the animation here as osgAnimation can't seem to calculate it right
        animation->setDuration(anim.duration);

        // for every bone in the animation create a position and rotation channel
        for (uint32_t i = 0; i < anim.boneInfos.size(); ++i) {

            // get the sequence for our boneindex and bone name for our target
            const auto keyList = &anim.kListSeq[i];
            const auto target = anim.boneInfos[i].boneName;

            // create our position key frame container and how we sample those frames
            osg::ref_ptr<osgAnimation::Vec3KeyframeContainer> posKey = new osgAnimation::Vec3KeyframeContainer;
            osg::ref_ptr<osgAnimation::Vec3LinearSampler> posSampler = new osgAnimation::Vec3LinearSampler;
            osg::ref_ptr<osgAnimation::Vec3LinearChannel> posChannel = new osgAnimation::Vec3LinearChannel(posSampler.get());
            posSampler->setKeyframeContainer(posKey.get());
            posChannel->setName("position");
            posChannel->setTargetName(target);

            // create our rotation key frame container and how we sample those frames
            osg::ref_ptr<osgAnimation::QuatKeyframeContainer> rotKey = new osgAnimation::QuatKeyframeContainer;
            osg::ref_ptr<osgAnimation::QuatSphericalLinearSampler> rotSampler = new osgAnimation::QuatSphericalLinearSampler;
            osg::ref_ptr<osgAnimation::QuatSphericalLinearChannel> rotChannel = new osgAnimation::QuatSphericalLinearChannel(rotSampler.get());
            rotSampler->setKeyframeContainer(rotKey.get());
            rotChannel->setName("rotation");
            rotChannel->setTargetName(target);

            // for every position key, create a corresponding keyframe
            for (uint32_t j = 0; j < keyList->posKeys.size(); ++j) {

                const auto& key = keyList->posKeys[j];
                posKey->push_back(osgAnimation::Vec3Keyframe(key.time, key.position));
            }

            // for every rotation key, create a corresponding keyframe
            for (uint32_t j = 0; j < keyList->rotKeys.size(); ++j) {

                const auto& key = keyList->rotKeys[j];
                rotKey->push_back(osgAnimation::QuatKeyframe(key.time, key.rotation));
            }

            // TODO: confirm if the below check is required
            // add our position channel to the animation
            if (posChannel->getOrCreateSampler()->getOrCreateKeyframeContainer()->size() > 0)
                animation->addChannel(posChannel.get());

            // TODO: confirm if the below check is required
            // add our rotation channel to the animation
            if (rotChannel->getOrCreateSampler()->getOrCreateKeyframeContainer()->size() > 0)
                animation->addChannel(rotChannel.get());
        }

        return animation;
    }
}
