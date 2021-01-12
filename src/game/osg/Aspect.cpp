
#include "Aspect.hpp"
#include "AspectImpl.hpp"

#include <algorithm>
#include <functional>

#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osg/ComputeBoundsVisitor>
#include <osgAnimation/UpdateBone>
#include <osgAnimation/StackedTranslateElement>
#include <osgAnimation/StackedQuaternionElement>
#include <osg/PolygonMode>

#include "spdlog/spdlog.h"

namespace ehb
{
    static osgAnimation::VertexInfluence& getVertexInfluence(osgAnimation::VertexInfluenceMap& vim, const std::string& name) {

        osgAnimation::VertexInfluenceMap::iterator it = vim.lower_bound(name);
        if (it == vim.end() || name != it->first) {

            it = vim.insert(it, osgAnimation::VertexInfluenceMap::value_type(name, osgAnimation::VertexInfluence()));
            it->second.setName(name);
        }
        return it->second;
    }

    static osg::ref_ptr<osg::Group> createRefGeometry(osg::Vec3 p, double len)
    {
        osg::ref_ptr<osg::Group> group = new osg::Group;
        {
            osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
            osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

            // Joint
            vertices->push_back(osg::Vec3(-len, 0.0, 0.0));
            vertices->push_back(osg::Vec3(len, 0.0, 0.0));
            vertices->push_back(osg::Vec3(0.0, -len, 0.0));
            vertices->push_back(osg::Vec3(0.0, len, 0.0));
            vertices->push_back(osg::Vec3(0.0, 0.0, -len));
            vertices->push_back(osg::Vec3(0.0, 0.0, len));

            // Bone
            vertices->push_back(osg::Vec3(0.0, 0.0, 0.0));
            vertices->push_back(p);

            geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 8));
            geometry->setVertexArray(vertices.get());

            group->addChild(geometry.get());
        }

        return group;
    }

    // https://github.com/xarray/osgRecipes/blob/master/cookbook/chapter8/ch08_07/OctreeBuilder.cpp
    // flipped the parameters for ease of use with ds bboxes
    static osg::Group* createBoxForDebug(const osg::Vec3& min, const osg::Vec3& max)
    {
        osg::Vec3 dir = max - min;
        osg::ref_ptr<osg::Vec3Array> va = new osg::Vec3Array(10);
        (*va)[0] = min + osg::Vec3(0.0f, 0.0f, 0.0f);
        (*va)[1] = min + osg::Vec3(0.0f, 0.0f, dir[2]);
        (*va)[2] = min + osg::Vec3(dir[0], 0.0f, 0.0f);
        (*va)[3] = min + osg::Vec3(dir[0], 0.0f, dir[2]);
        (*va)[4] = min + osg::Vec3(dir[0], dir[1], 0.0f);
        (*va)[5] = min + osg::Vec3(dir[0], dir[1], dir[2]);
        (*va)[6] = min + osg::Vec3(0.0f, dir[1], 0.0f);
        (*va)[7] = min + osg::Vec3(0.0f, dir[1], dir[2]);
        (*va)[8] = min + osg::Vec3(0.0f, 0.0f, 0.0f);
        (*va)[9] = min + osg::Vec3(0.0f, 0.0f, dir[2]);

        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
        geom->setVertexArray(va.get());
        geom->addPrimitiveSet(new osg::DrawArrays(GL_QUAD_STRIP, 0, 10));

        osg::ref_ptr<osg::Group> group = new osg::Group;
        group->addChild(geom.get());
        group->getOrCreateStateSet()->setAttribute(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
        group->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        return group.release();
    }

    Aspect::Aspect(std::shared_ptr<Impl> impl) : d(std::move(impl))
    {
        auto log = spdlog::get("log");

        debugDrawingGroups.resize(3);

        skeleton = new osgAnimation::Skeleton;

        // be default meshes aren't rendered using their skeleton so lets mimic a root bone
        pseudoRoot = new osg::MatrixTransform;
        addChild(pseudoRoot);

        // put the root bone as the first child of skeleton for easy acces
        skeleton->addChild(new osgAnimation::Bone(d->boneInfos[0].name));

        // it looks like the skeleton callback just validates the skeleton and then stays in the callbacks
        // TODO: osgAnimation should expose the ValidateSkeletonVisitor to the API and let you call it once when you need it
        skeleton->setUpdateCallback(nullptr);

        log->debug("asp has {} sub meshes", d->subMeshes.size());

        for (const auto& mesh : d->subMeshes)
        {
            log->debug("asp subMesh has {} textures", mesh.textureCount);

            uint32_t f = 0; // track which face the loader is loading across the sub mesh
            for (uint32_t i = 0; i < mesh.textureCount; ++i)
            {
                // create the geometry which will be drawn
                osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;

                // create the rigged geometry that is used to do our weighting
                osg::ref_ptr<osgAnimation::RigGeometry> rigGeometry = new osgAnimation::RigGeometry;
                rigGeometry->setInfluenceMap(new osgAnimation::VertexInfluenceMap);
                rigGeometry->setSourceGeometry(geometry);

                // store our rigGeometry for later use
                rigGeometryVec.push_back(rigGeometry);

                // add the geometry to our aspect for quick lookups
                geometryVec.push_back(geometry);

                // textures are stored directly against the mesh
                // TODO: should we store the osg::Image against AspectImpl?
                const std::string imageFilename = d->textureNames[i] + ".raw";
                osg::ref_ptr<osg::Image> image = osgDB::readImageFile(imageFilename);

                geometry->setName(d->textureNames[i]);

                if (image != nullptr)
                {
                    osg::Texture2D* texture = new osg::Texture2D(image);
                    geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
                    geometry->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

                    texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
                    texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);

                    log->debug("loaded image {} for asp sub mesh: {}", i, imageFilename);
                }

                osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
                osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
                osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array;
                osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

                geometry->setVertexArray(vertices);
                geometry->setNormalArray(normals, osg::Array::BIND_PER_VERTEX);
                geometry->setTexCoordArray(0, texCoords);
                geometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX);

                for (uint32_t cornerCounter = 0; cornerCounter < mesh.cornerCount; ++cornerCounter)
                {
                    const auto& c = mesh.corners[cornerCounter];
                    const auto& w = mesh.wCorners[cornerCounter];

                    vertices->push_back(c.position);
                    texCoords->push_back(c.texCoord);
                    normals->push_back(c.normal);
                    colors->push_back(osg::Vec4(c.color[0], c.color[1], c.color[2], c.color[3]));

                    osgAnimation::VertexInfluenceMap& influenceMap = *rigGeometry->getInfluenceMap();

                    // There are 4 weights per bone
                    for (uint32_t weight = 0; weight < 4; ++weight)
                    {
                        int32_t boneId = static_cast<int32_t> (w.bone[weight]);
                        float value = static_cast<float> (w.weight[weight]);

                        if (boneId != 0) {

                            osgAnimation::VertexInfluence& vi = getVertexInfluence(influenceMap, d->boneInfos[boneId - 1].name);
                            vi.push_back(osgAnimation::VertexIndexWeight(cornerCounter, value));
                        }
                    }

                }

                osg::ref_ptr<osg::DrawElementsUInt> elements = new osg::DrawElementsUInt(GL_TRIANGLES);
                geometry->addPrimitiveSet(elements);

                for (uint32_t fpt = 0; fpt < mesh.matInfo[i].faceSpan; ++fpt)
                {
                    elements->push_back(mesh.faceInfo.cornerIndex.at(f).index[0] + mesh.faceInfo.cornerStart[i]);
                    elements->push_back(mesh.faceInfo.cornerIndex.at(f).index[1] + mesh.faceInfo.cornerStart[i]);
                    elements->push_back(mesh.faceInfo.cornerIndex.at(f).index[2] + mesh.faceInfo.cornerStart[i]);

                    ++f;
                }

                // make sure to calculate our bounding boxes
                geometry->setInitialBound(geometry->computeBoundingBox());

                // by default we draw static geometry
                pseudoRoot->addChild(geometry);

                // make sure the skeleton is ready to go if we need it
                skeleton->addChild(rigGeometry);
            }
        }

        osg::ComputeBoundsVisitor cb;
        cb.apply(*pseudoRoot);

        // both pseudoRoot and skeleton should contain the same geometry information but pseudo root should take less time to traverse
        setInitialBound(cb.getBoundingBox());

        // setup our root bone
        osg::ref_ptr<osgAnimation::Bone> root = static_cast<osgAnimation::Bone*>(skeleton->getChild(0));
        bones.push_back(root);

        // root->addChild(createRefGeometry(pos, 0.25).get());

        //  hack work around for some meshes having duplicate names
        std::set<std::string> boneNames;

        boneNames.emplace(root->getName());

        // generate our bone heirarchy - start from 1 since we already handled the root bone
        for (uint32_t i = 1; i < d->boneInfos.size(); ++i)
        {
            osg::ref_ptr<osgAnimation::Bone> bone = new osgAnimation::Bone(d->boneInfos[i].name);
            osg::ref_ptr<osgAnimation::Bone> parent = bones.at(d->boneInfos[i].parentIndex);

            if (boneNames.count(bone->getName()) != 0)
            {
                const std::string& newName = bone->getName() + "_DUP";
                // log->error("DUPLICATE BONE NAME: {} WE ARE REMAPPING TO {}", bone->getName(), bone->getName() + "_DUP");
                boneNames.emplace(bone->getName() + "_DUP");

                bone->setName(newName);
            }
            else
            {
                boneNames.emplace(bone->getName());
            }

            parent->addChild(bone);
            bones.push_back(bone);
        }

        osg::Matrix bind = osg::Matrix::rotate(d->rposInfoRel[0].rotation) * osg::Matrix::translate(d->rposInfoRel[0].position);
        osg::Matrix inverseBind = osg::Matrix::rotate(d->rposInfoAbI[0].rotation) * osg::Matrix::translate(d->rposInfoAbI[0].position);

        // this is totally wrong, i think
        if (d->boneInfos.size() > 1)
        {
            root->setInvBindMatrixInSkeletonSpace(inverseBind);
        }
        else
        {
            root->setInvBindMatrixInSkeletonSpace(inverseBind * bind);
        }

        // updaters aren't applied unless the GameObject has a [body] component
        osg::ref_ptr<osgAnimation::UpdateBone> updater = new osgAnimation::UpdateBone(d->boneInfos[0].name);
        updater->getStackedTransforms().push_back(new osgAnimation::StackedTranslateElement("position", d->rposInfoRel[0].position));
        updater->getStackedTransforms().push_back(new osgAnimation::StackedQuaternionElement("rotation", d->rposInfoRel[0].rotation));

        root->setUpdateCallback(updater);

        // lets force the intial position of the bone to be our stacked transform
        // with this we don't require the UpdateBone updaters to run at all
        pseudoRoot->setMatrix(bind);
        root->setMatrix(bind);
        root->setMatrixInSkeletonSpace(bind);

        // iterate again and setup our initial bone positions
        for (uint32_t i = 1; i < d->boneInfos.size(); ++i)
        {
            osg::ref_ptr<osgAnimation::Bone> bone = bones.at(i);
            osg::ref_ptr<osgAnimation::Bone> parent = bones.at(d->boneInfos[i].parentIndex);

            bind = osg::Matrix::rotate(d->rposInfoRel[i].rotation) * osg::Matrix::translate(d->rposInfoRel[i].position);
            inverseBind = osg::Matrix::rotate(d->rposInfoAbI[i].rotation) * osg::Matrix::translate(d->rposInfoAbI[i].position);

            bone->setInvBindMatrixInSkeletonSpace(inverseBind);

            // updaters aren't applied unless the GameObject has a [body] component
            osg::ref_ptr<osgAnimation::UpdateBone> updater = new osgAnimation::UpdateBone(d->boneInfos[i].name);
            updater->getStackedTransforms().push_back(new osgAnimation::StackedTranslateElement("position", d->rposInfoRel[i].position));
            updater->getStackedTransforms().push_back(new osgAnimation::StackedQuaternionElement("rotation", d->rposInfoRel[i].rotation));
            bone->setUpdateCallback(updater);

            // lets force the intial position of the bone to be our stacked transform
            // with this we don't require the UpdateBone updaters to run at all
            bone->setMatrix(osg::Matrix(d->rposInfoRel[i].rotation) * osg::Matrix::translate(d->rposInfoRel[i].position));
            bone->setMatrixInSkeletonSpace(osg::Matrix(d->rposInfoRel[i].rotation) * osg::Matrix::translate(d->rposInfoRel[i].position) * parent->getMatrixInSkeletonSpace());
        }
    }

    Aspect::Aspect(const Aspect& aspect, const osg::CopyOp& copyop) : osg::Group(aspect, copyop)
    {
        // share our implementation details
        d = aspect.d;

        // if this is a shallow copy we are pretty safe to copy over the pointers
        if (copyop.getCopyFlags() & osg::CopyOp::SHALLOW_COPY)
        {
            bones = aspect.bones;
            rigGeometryVec = aspect.rigGeometryVec;
            geometryVec = aspect.geometryVec;
            skeleton = aspect.skeleton;
            pseudoRoot = aspect.pseudoRoot;
        }
        else if (copyop.getCopyFlags() & osg::CopyOp::DEEP_COPY_NODES)
        {
            std::function<void(osg::ref_ptr<osgAnimation::Bone>)> recurse = [&, this](osgAnimation::Bone* bone)
            {
                bones.push_back(bone);

                for (uint32_t i = 0; i < bone->getNumChildren(); ++i)
                {
                    recurse(static_cast<osgAnimation::Bone*>(bone->getChild(i)));
                }
            };

            d = aspect.d;

            // this is normally called from AspectComponent when things are added to the graph so... make some assumptions...

            // deep copy our skeleton which isn't part of the group yet
            skeleton = static_cast<osgAnimation::Skeleton*>(aspect.skeleton->clone(copyop));

            // make sure to update our bone reference vec
            recurse(static_cast<osgAnimation::Bone*>(skeleton->getChild(0)));

            // first child is our pseudo bone which we can grab from this group since it's been cloned
            pseudoRoot = static_cast<osg::MatrixTransform*>(getChild(0));

            // rebuild our geometry references
            for (uint32_t i = 0; i < pseudoRoot->getNumChildren(); ++i)
            {
                geometryVec.push_back(static_cast<osg::Geometry*>(pseudoRoot->getChild(i)));
            }
        }
    }

    osg::Object* Aspect::clone(const osg::CopyOp& copyop) const
    {
        return new Aspect(*this, copyop);
    }

    osg::Geometry* Aspect::geometry(unsigned int index)
    {
        return geometryVec[index];
    }

    // Should this be an update callback?
    void Aspect::applySkeleton()
    {
        // should I shallow clone the mesh to prevent anything going out of scope and being destructed?

        // first step is to remove all children from the group
        removeChildren(0, getNumChildren());

        // apply the skeleton as the first child
        addChild(skeleton);
    }

    // Should this be an update callback?
    void Aspect::removeSkeleton()
    {
        // remove our skeleton
        removeChildren(0, getNumChildren());

        // apply our static geometry with root bone transform
        addChild(pseudoRoot);
    }

    void Aspect::toggleBoundingBox()
    {
        if (!drawingBoundingBox)
        {
            if (debugDrawingGroups[1] == nullptr)
            {
                debugDrawingGroups[1] = new osg::Group;

                osg::ComputeBoundsVisitor cbv;
                accept(cbv);

                debugDrawingGroups[1]->addChild(createBoxForDebug(cbv.getBoundingBox()._min, cbv.getBoundingBox()._max));
            }

            addChild(debugDrawingGroups[1]);

            drawingBoundingBox = true;

            return;
        }

        if (drawingBoundingBox)
        {
            if (debugDrawingGroups[1] != nullptr)
                removeChild(debugDrawingGroups[1]);

            drawingBoundingBox = false;

            return;
        }
    }
}