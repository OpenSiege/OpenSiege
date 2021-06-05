
#pragma once

#include <optional>
#include <osg/Group>
#include "Rect.hpp"
#include "NormalizedRect.hpp"
#include "WidgetComponent.hpp"

namespace ehb
{
    class Shell;
    class Widget : public osg::Group
    {
    public:

        Widget() = default;

        //! before we can uncomment this I need to fix the console
        // Widget(Shell& shell);

        virtual ~Widget() = default;

        void setRect(int32_t left, int32_t top, int32_t right, int32_t bottom);

        void setUVRect(float left, float top, float right, float bottom);

        const Rect& effectiveRect() const;
        uint32_t drawOrder() const;

        void loadTexture(const std::string& textureFileName, bool resizeWidget);

        uint32_t width() const;
        uint32_t height() const;

        uint32_t screenWidth() const;
        uint32_t screenHeight() const;

        void addDebugData();

    protected:

        // TODO: fix console
        // Shell& shell;

    private:

        Rect rect;
        NormalizedRect uv;

    protected:

        WidgetComponent* baseComponent = nullptr;
    };

    inline const Rect& Widget::effectiveRect() const {

        return rect;
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