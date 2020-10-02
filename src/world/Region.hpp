
#pragma once

#include <osg/MatrixTransform>

namespace ehb
{
    class Region : public osg::MatrixTransform
    {
    public:

        Region() = default;

    protected:

        virtual ~Region() = default;
    };
}