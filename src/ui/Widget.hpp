
#pragma once

#include <optional>
#include <osg/Group>
#include "Rect.hpp"
#include "NormalizedRect.hpp"
#include "WidgetComponent.hpp"

namespace ehb
{
    class FuelBlock;
    class Shell;
    class WidgetComponent;
    class Widget : public osg::Group
    {
    public:

        Widget(Shell& shell);

        virtual ~Widget() = default;

        void setRect(int32_t left, int32_t top, int32_t right, int32_t bottom);

        void setUVRect(float left, float top, float right, float bottom);

        const Rect& effectiveRect() const;
        uint32_t drawOrder() const;

        void setLayer(uint32_t value) { z.layer = value; }
        uint32_t layer() const { return z.layer; }

        bool isTopMost() const;

        void loadTexture(const std::string& textureFileName, bool resizeWidget = false);

        uint32_t width() const;
        uint32_t height() const;

        uint32_t screenWidth() const;
        uint32_t screenHeight() const;

        // TODO: remove in favour of a ReaderWriter?
        void buildWidgetFromFuelBlock(FuelBlock* fuel);

        void addDebugData();

    protected:

        Shell& shell;

    private:

        Rect rect;
        NormalizedRect uv;

        struct { uint32_t layer = 0; uint32_t value = 0; } z;

    protected:

        WidgetComponent* baseComponent = nullptr;
    };

    inline const Rect& Widget::effectiveRect() const {

        return rect;
    }

    inline uint32_t Widget::drawOrder() const
    {
        return z.value;
    }

    inline bool Widget::isTopMost() const
    {
        return z.layer == 1;
    }

    inline uint32_t Widget::width() const
    {
        return rect.width();
    }

    inline uint32_t Widget::height() const
    {
        return rect.height();
    }
}