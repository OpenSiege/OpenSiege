
/*
 * Copyright (c) 2016-2020 OpenSiege Team
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

#include "Fuel.hpp"

#include <fstream>
#include <sstream>
#include <cctype>
#include "FuelParser.hpp"
#include "FuelScanner.hpp"

namespace ehb
{
    static std::vector<std::string> split(const std::string & value, char delim)
    {
        std::vector<std::string> result;

        std::istringstream stream(value);

        while (!stream.eof())
        {
            std::string item;
            std::getline(stream, item, delim);

            result.push_back(item);
        }

        return result;
    }

    static bool stringEqual(const std::string & str1, const std::string & str2)
    {
        return ( (str1.size() == str2.size() ) &&
                std::equal(str1.begin(), str1.end(), str2.begin(), [](const char & c1, const char & c2) {
                    if (c1 == c2) return true;
                    if (std::toupper(c1) == std::toupper(c2)) return true;
                    return false;
                }) );
    }

    FuelBlock::~FuelBlock()
    {
        for (FuelBlock * node : mChildren)
        {
            delete node;
        }
    }

    FuelBlock * FuelBlock::appendChild(const std::string & name)
    {
        const auto index = name.find_last_of(':');

        if (index != std::string::npos)
        {
            const std::vector<std::string> path = split(name.substr(0, index), ':');

            FuelBlock * parent = this;

            for (const std::string & item : path)
            {
                FuelBlock * node = parent->child(item);

                if (!node)
                {
                    node = parent->appendChild(item);
                }

                parent = node;
            }

            return parent->appendChild(name.substr(index + 1));
        }
        else
        {
            // simply add a new child to this node with the given name
            FuelBlock * node = new FuelBlock(this);

            node->mName = name;

            mChildren.push_back(node);

            return node;
        }
    }

    FuelBlock * FuelBlock::appendChild(const std::string & name, const std::string & type)
    {
        FuelBlock * result = appendChild(name);

        result->mType = type;

        return result;
    }

    FuelBlock * FuelBlock::child(const std::string & name) const
    {
        // TODO: cleanup the code here

        std::vector<std::string> path = split(name, ':');

        switch (path.size())
        {
            case 1:
            {
                for (FuelBlock * node : mChildren)
                {
                    if (node->name() == name)
                    {
                        return node;
                    }
                }
            } break;

            default:
            {
                const FuelBlock * cItr = this;
                FuelBlock * finalAnswer = nullptr;

                for (const std::string & item : path)
                {
                    FuelBlock * result = nullptr;

                    for (FuelBlock * node : cItr->mChildren)
                    {
                        if (node->name() == item)
                        {
                            result = node;
                            break;
                        }
                    }

                    // did we find the node we're looking for?
                    if (result)
                    {
                        cItr = result;
                        finalAnswer = result;
                    }
                    else
                    {
                        return nullptr;
                    }
                }

                return finalAnswer;
            } break;
        }

        return nullptr;
    }

    const std::vector<FuelBlock *> & FuelBlock::eachChildOf(const std::string & name) const
    {
        static std::vector<FuelBlock *> emptyVector;

        if (FuelBlock * node = this->child(name))
        {
            return node->mChildren;
        }

        return emptyVector;
    }

    const std::vector<Attribute> & FuelBlock::eachAttrOf(const std::string & name) const
    {
        static std::vector<Attribute> empty;

        if (FuelBlock * node = this->child(name))
        {
            return node->mAttributes;
        }

        return empty;
    }

    void FuelBlock::appendValue(const std::string & name, const std::string & type, const std::string & value)
    {
        const auto index = name.find_last_of(':');

        if (index != std::string::npos)
        {
            const std::vector<std::string> path = split(name.substr(0, index), ':');

            FuelBlock * parent = this;

            for (const std::string & item : path)
            {
                FuelBlock * node = parent->child(item);

                if (!node)
                {
                    node = parent->appendChild(item);
                }

                parent = node;
            }

            return parent->appendValue(name.substr(index + 1), type, value);
        }
        else
        {
            Attribute attr;

            attr.name = name;
            attr.type = type;
            attr.value = value;

            mAttributes.push_back(attr);
        }
    }

    bool FuelBlock::valueAsBool(const std::string & name, bool defaultValue) const
    {
        if (const Attribute * attr = attribute(name))
        {
            return stringEqual(attr->value, "true");
        }

        return defaultValue;
    }

    int FuelBlock::valueAsInt(const std::string & name, int defaultValue) const
    {
        if (const Attribute * attr = attribute(name))
        {
            const int base = attr->type == "x" ? 16 : 10;

            try
            {
                return std::stoi(attr->value, nullptr, base);
            }
            catch (...)
            {
            }
        }

        return defaultValue;
    }

    unsigned int FuelBlock::valueAsUInt(const std::string & name, unsigned int defaultValue) const
    {
        if (const Attribute * attr = attribute(name))
        {
            const int base = attr->type == "x" ? 16 : 10;

            try
            {
                return std::stoul(attr->value, nullptr, base);
            }
            catch (...)
            {
            }
        }

        return defaultValue;
    }

    float FuelBlock::valueAsFloat(const std::string & name, float defaultValue) const
    {
        if (const Attribute * attr = attribute(name))
        {
            try
            {
                return std::stof(attr->value, nullptr);
            }
            catch (...)
            {
            }
        }

        return defaultValue;
    }

    std::string FuelBlock::valueAsString(const std::string & name, const std::string & defaultValue) const
    {
        if (const Attribute * attr = attribute(name))
        {
            if (attr->value.front() == '"' && attr->value.back() == '"')
            {
                return attr->value.substr(1, attr->value.size() - 2);
            }
        }

        return defaultValue;
    }

    std::array<float, 3> FuelBlock::valueAsFloat3(const std::string& name, std::array<float, 3> defaultValue) const
    {
        if (const Attribute * attr = attribute(name))
        {
            const std::string& value = valueOf(name);

            if (value.empty())
            {
                return defaultValue;
            }

            const auto itr1 = value.find(',');
            const auto itr2 = value.find(',', itr1 + 1);
            const auto itr3 = value.find(',', itr2 + 1);

            float x = std::stof(value.substr(0, itr1));
            float y = std::stof(value.substr(itr1 + 1, itr2 - itr1 - 1));
            float z = std::stof(value.substr(itr2 + 1, itr3 - itr2 - 1));

            return std::array<float, 3> {x, y, z};
        }

        return defaultValue;
    }

    osg::Vec3 FuelBlock::valueAsVec3(const std::string& name, const osg::Vec3& defaultValue) const
    {
        if (const Attribute * attr = attribute(name))
        {
            auto value = valueAsFloat3(name);

            return osg::Vec3(value[0], value[1], value[2]);
        }

        return defaultValue;
    }

    osg::Vec4 FuelBlock::valueAsColor(const std::string & name, const osg::Vec4 & defaultValue) const
    {
        if (const Attribute * attr = attribute(name))
        {
            if (attr->value != "-1")
            {
                const int base = attr->type == "i" ? 10 : 16;

                try
                {
                    const unsigned int value = std::stoul(attr->value, nullptr, 16);

                    uint8_t r = static_cast<float>((value >> 16) & 255);
                    uint8_t g = static_cast<float>((value >> 8) & 255);
                    uint8_t b = static_cast<float>(value & 255);

                    return osg::Vec4(r, g, b, 255.f) / 255.f;
                }
                catch (...)
                {
                }
            }
        }

        return defaultValue;
    }

#if 0
    SiegeRot FuelBlock::valueAsSiegeRot(const std::string & name, const SiegeRot & defaultValue) const
    {
        if (const Attribute * attr = attribute(name))
        {
            const std::string & value = valueOf(name);

            if (value.empty())
            {
                return defaultValue;
            }

            const auto itr1 = value.find(',');
            const auto itr2 = value.find(',', itr1 + 1);
            const auto itr3 = value.find(',', itr2 + 1);
            const auto itr4 = value.find(',', itr3 + 1);

            float x = std::stof(value.substr(0, itr1));
            float y = std::stof(value.substr(itr1 + 1, itr2 - itr1 - 1));
            float z = std::stof(value.substr(itr2 + 1, itr3 - itr2 - 1));
            float w = std::stof(value.substr(itr3 + 1, itr4 - itr3 - 1));
            unsigned int node = std::stoul(value.substr(itr4 + 1), nullptr, 16);

            return SiegeRot(x, y, z, w, node);
        }

        return defaultValue;
    }

    SiegePos FuelBlock::valueAsSiegePos(const std::string & name, const SiegePos & defaultValue) const
    {
        if (const Attribute * attr = attribute(name))
        {
            const std::string& value = valueOf(name);

            if (value.empty())
            {
                return defaultValue;
            }

            const auto itr1 = value.find(',');
            const auto itr2 = value.find(',', itr1 + 1);
            const auto itr3 = value.find(',', itr2 + 1);

            float x = std::stof(value.substr(0, itr1));
            float y = std::stof(value.substr(itr1 + 1, itr2 - itr1 - 1));
            float z = std::stof(value.substr(itr2 + 1, itr3 - itr2 - 1));
            unsigned int node = std::stoul(value.substr(itr3 + 1), nullptr, 16);

            return SiegePos(x, y, z, node);
        }

        return defaultValue;
    }
#endif

    FuelBlock * FuelBlock::clone(FuelBlock * parent) const
    {
        FuelBlock * result = new FuelBlock(parent);

        result->mName = mName;
        result->mType = mType;

        for (const FuelBlock * child : mChildren)
        {
            result->mChildren.push_back(child->clone(result));
        }

        for (const Attribute & attr : mAttributes)
        {
            result->mAttributes.push_back(attr);
        }

        return result;
    }

    void FuelBlock::merge(FuelBlock * result) const
    {
        if (result)
        {
            result->mName = mName;
            result->mType = mType;

            if (!isEmpty())
            {
                for (const FuelBlock * i : mChildren)
                {
                    bool found = false;

                    for (FuelBlock * j : result->mChildren)
                    {
                        if (i->name() == j->name())
                        {
                            found = true;
                            i->merge(j);
                            break;
                        }
                    }

                    if (!found)
                    {
                        FuelBlock * copy = i->clone(result);
                        result->mChildren.push_back(copy);
                    }
                }

                for (const Attribute & i : mAttributes)
                {
                    bool found = false;

                    for (Attribute & j : result->mAttributes)
                    {
                        if (i.name == j.name)
                        {
                            found = true;
                            j = i;
                            break;
                        }
                    }

                    if (!found)
                    {
                        result->mAttributes.push_back(i);
                    }
                }
            }
        }
    }

    const Attribute * FuelBlock::attribute(const std::string & name) const
    {
        const auto index = name.find_last_of(':');

        const FuelBlock * parent;
        std::string actualName;

        if (index != std::string::npos)
        {
            parent = child(name.substr(0, index));
            actualName = name.substr(index + 1);
        }
        else
        {
            parent = this;
            actualName = name;
        }

        if (parent)
        {
            for (const Attribute & attr : parent->mAttributes)
            {
                if (attr.name == actualName)
                {
                    return &attr;
                }
            }
        }

        return nullptr;
    }

    bool Fuel::load(std::istream & stream)
    {
        const std::string data(std::istreambuf_iterator<char>(stream), {});

        FuelScanner scanner(data.c_str());
        FuelParser parser(scanner, this);

        return parser.parse() == 0;
    }

    bool Fuel::load(const std::string & filename)
    {
        std::ifstream stream(filename);

        return load(stream);
    }

    struct walkNode
    {
        walkNode(std::ostream & stream) : stream(stream), level(0)
        {
        }

        void write(const FuelBlock * node)
        {
            if (node->type() != "")
            {
                stream << indent() << "[t:" << node->type() << ",n:" << node->name() << "]" << std::endl;
            }
            else
            {
                stream << indent() << "[" << node->name() << "]" << std::endl;
            }

            stream << indent() << "{" << std::endl;

            level++;
            for (unsigned int i = 0; i < node->valueCount(); i++)
            {
                stream << indent() << node->nameOf(i) << " = " << node->valueOf(i) << ";" << std::endl;
            }

            for (FuelBlock * child : node->eachChild())
            {
                write(child);
            }
            level--;

            stream << indent() << "}" << std::endl;
        }

        inline std::string indent() const
        {
            return std::string(level * 4, ' ');
        }

        std::ostream & stream;
        unsigned int level;
    };

    bool Fuel::save(std::ostream & stream) const
    {
        walkNode f(stream);

        for (FuelBlock * child : this->eachChild())
        {
            child->write(stream);
        }

        return true;
    }

    bool Fuel::save(const std::string & filename) const
    {
        std::ofstream stream(filename);

        for (FuelBlock * child : this->eachChild())
        {
            child->write(stream);
        }

        stream.close();

        return true;
    }

    void FuelBlock::write(std::ostream & stream) const
    {
        walkNode f(stream);

        f.write(this);
    }
}
