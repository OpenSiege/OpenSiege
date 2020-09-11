
#pragma once

#include <optional>
#include <osg/Group>
#include "Rect.hpp"
#include "NormalizedRect.hpp"
#include "WidgetComponent.hpp"

namespace ehb
{
    class Widget : public osg::Group
    {
    public:

        Widget() = default;

        void setRect(int32_t left, int32_t top, int32_t right, int32_t bottom);

        void setUVRect(float left, float top, float right, float bottom);

        const Rect& effectiveRect() const { return rect; }

        void loadTexture(const std::string& textureFileName, bool resizeWidget);

        void addDebugData();

    private:

        Rect rect;
        NormalizedRect uv;

    protected:

        WidgetComponent* baseComponent = nullptr;
    };
}