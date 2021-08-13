
/*
 * Copyright (c) 2016-20202 OpenSiege Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#include <string>
#include <vector>
#include <array>
//include <osg/Vec3>
//#include <osg/Vec4>
//#include "SiegeRot.hpp"
//#include "SiegePos.hpp"

#include <iostream>

namespace ehb
{
    // private
    struct Attribute
    {
        std::string name;
        std::string type;
        std::string value;
    };

    // main element to make use of in this api
    class FuelBlock
    {
        public:

           ~FuelBlock();

            FuelBlock * parent() const;

            const std::string & name () const;
            const std::string & type () const;

            //! @return whether this node has no child nodes and attributes or not
            bool isEmpty() const;

            /**
             * @param name the full name of the new node to create
             * @param type the type of the new node to create
             * @return the newly created child node
             */
            FuelBlock * appendChild(const std::string & name);
            FuelBlock * appendChild(const std::string & name, const std::string & type);

            FuelBlock * child(const std::string & name) const;

            const std::vector<FuelBlock *> & eachChild() const;
            const std::vector<FuelBlock *> & eachChildOf(const std::string & name) const;

            bool hasAttr(const std::string & name) const;

            // TODO: rename eachAttribute to eachAttr
            const std::vector<Attribute> & eachAttribute() const;
            const std::vector<Attribute> & eachAttrOf(const std::string & name) const;

            void appendValue(const std::string & name, const std::string & value);
            void appendValue(const std::string & name, const std::string & type, const std::string & value);

            //! @return the number of attributes in this node
            unsigned int valueCount() const;

            //! @return the number of children in this node
            unsigned int childCount() const;

            /**
             * @param name which attribute type or value to return
             * @param defaultValue the value to return if the attribute does not exist
             * @return the attribute type or value
             */
            const std::string & valueOf(const std::string & name, const std::string & defaultValue = "") const;
            const std::string & typeOf(const std::string & name, const std::string & defaultValue = "") const;

            /**
             * @param index which attribute name, type, or value to return ranging from 0 to valueCount()
             * @param defaultValue the value to return if the attribute does not exist
             * @return the attribute name, type, or value
             */
            const std::string & nameOf(unsigned int index, const std::string & defaultValue = "") const;
            const std::string & typeOf(unsigned int index, const std::string & defaultValue = "") const;
            const std::string & valueOf(unsigned int index, const std::string & defaultValue = "") const;

            /**
             * @param name which attribute value to return
             * @param defaultValue the value to return if the attribute does not exist or cannot be coerced to the desired type
             * @return the attribute value interpreted as the desired type
             */
            bool valueAsBool(const std::string & name, bool defaultValue = false) const;
            int valueAsInt(const std::string & name, int defaultValue = 0) const;
            std::array<int, 4> valueAsInt4(const std::string& name, const std::array<int, 4> defaultValue = { 0, 0, 0, 0 }) const;
            unsigned int valueAsUInt(const std::string & name, unsigned int defaultValue = 0) const;
            float valueAsFloat(const std::string & name, float defaultValue = 0.f) const;
            std::string valueAsString(const std::string & name, const std::string & defaultValue = "") const;

            // extra types...
            std::array<float, 3> valueAsFloat3(const std::string & name, const std::array<float, 3> defaultValue = { 1.0, 1.0, 1.0 }) const;
            std::array<float, 4> valueAsFloat4(const std::string& name, const std::array<float, 4> defaultValue = { 1.0, 1.0, 1.0, 1.0 }) const;
            //osg::Vec3 valueAsVec3(const std::string & name, const osg::Vec3 & defaultValue = { 1.0, 1.0, 1.0 }) const; // don't use 1.f as osg::Vec3 could be doubles
            //osg::Vec4 valueAsColor(const std::string & name, const osg::Vec4 & defaultValue = { 1.f, 1.f, 1.f, 1.f }) const;
            //SiegeRot valueAsSiegeRot(const std::string & name, const SiegeRot & defaultValue = { 0.0, 0.0, 0.0, 0.0, 0}) const;
            //SiegePos valueAsSiegePos(const std::string & name, const SiegePos & defaultValue = { 0.0, 0.0, 0.0, 0 }) const;

            /**
             * merge the contents of this node into the result node
             * NOTE: empty children from this node will entirely overwrite
             * (including all attributes and grand children nodes) children of
             * the same name in the result node
             */
            void merge(FuelBlock * result) const;

            /**
             * integrate a fuelblock into an existing fuelblock
             * this is essentially the same as merge except it will handle the creation of the proper fuel blocks for you
             */
            void integrate(FuelBlock * source);

            void write(std::ostream & stream) const;

        protected:

            FuelBlock(FuelBlock * parent = nullptr);

        private:

            //! create a deep copy of the node that must be deleted by the user
            FuelBlock* clone(FuelBlock* parent = nullptr) const;

            const Attribute * attribute(const std::string & name) const;

        private:

            std::string mName;
            std::string mType;
            FuelBlock * mParent;
            std::vector<FuelBlock *> mChildren;
            std::vector<Attribute> mAttributes;
    };

    inline FuelBlock::FuelBlock(FuelBlock * parent) : mParent(parent)
    {
    }

    inline FuelBlock * FuelBlock::parent() const
    {
        return mParent;
    }

    inline const std::string & FuelBlock::name () const
    {
        return mName;
    }

    inline const std::string & FuelBlock::type () const
    {
        return mType;
    }

    inline bool FuelBlock::isEmpty() const
    {
        return mChildren.empty() && mAttributes.empty();
    }

    inline const std::vector<FuelBlock *> & FuelBlock::eachChild() const
    {
        return mChildren;
    }

    inline bool FuelBlock::hasAttr(const std::string & name) const
    {
        for (const Attribute & attr : mAttributes)
        {
            if (attr.name == name)
            {
                return true;
            }
        }

        return false;
    }

    inline const std::vector<Attribute> & FuelBlock::eachAttribute() const
    {
        return mAttributes;
    }

    inline void FuelBlock::appendValue(const std::string & name, const std::string & value)
    {
        appendValue(name, "", value);
    }

    inline unsigned int FuelBlock::valueCount() const
    {
        return mAttributes.size();
    }

    inline unsigned int FuelBlock::childCount() const
    {
        return mChildren.size();
    }

    inline const std::string & FuelBlock::valueOf(const std::string & name, const std::string & defaultValue) const
    {
        if (const Attribute * attr = attribute(name))
        {
            return attr->value;
        }

        return defaultValue;
    }

    inline const std::string & FuelBlock::typeOf(const std::string & name, const std::string & defaultValue) const
    {
        if (const Attribute * attr = attribute(name))
        {
            return attr->type;
        }

        return defaultValue;
    }

    inline const std::string & FuelBlock::nameOf(unsigned int index, const std::string & defaultValue) const
    {
        if (index < mAttributes.size())
        {
            return mAttributes[index].name;
        }

        return defaultValue;
    }

    inline const std::string & FuelBlock::typeOf(unsigned int index, const std::string & defaultValue) const
    {
        if (index < mAttributes.size())
        {
            return mAttributes[index].type;
        }

        return defaultValue;
    }

    inline const std::string & FuelBlock::valueOf(unsigned int index, const std::string & defaultValue) const
    {
        if (index < mAttributes.size())
        {
            return mAttributes[index].value;
        }

        return defaultValue;
    }

    class Fuel : public FuelBlock
    {
        public:

            explicit Fuel() = default;

            bool load(std::istream & stream);
            bool load(const std::string & filename);

            bool save(std::ostream & stream) const;
            bool save(const std::string & filename) const;

            void print() const;

    };
}
