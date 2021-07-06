
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

        friend class ReaderWriterUI;

        Widget(Shell& shell);

        virtual ~Widget() = default;

        void setRect(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);

        void setUVRect(float left, float top, float right, float bottom);

        const Rect& effectiveRect() const;
        uint32_t drawOrder() const;

        void setLayer(uint32_t value) { z.layer = value; }
        uint32_t layer() const { return z.layer; }

        void setTopMost(bool value);
        bool isTopMost() const;

        void setVisible(bool value);
        bool isVisible() const;

        void loadTexture(const std::string& textureFileName, bool resizeWidget = false);

        virtual void createCommonCtrl(const std::string& value);

        void resize(uint32_t width, uint32_t height);

        //! this is where the "magic" happens
        virtual void resizeToScreenResolution(uint32_t newScreenWidth, uint32_t newScreenHeight);

        bool isCommonControl() const;
        const std::string& commonTemplate() const;

        uint32_t width() const;
        uint32_t height() const;

        uint32_t screenWidth() const;
        uint32_t screenHeight() const;

        void setTiledTexture(bool value);

        void addDebugData();

    protected:

        Shell& shell;

    private:

        std::optional<Rect> base;
        Rect rect;
        NormalizedRect uv;

        struct { std::string group, dockGroup, tmpl; } attr;
        struct { bool consumable = false; bool passThrough = false; uint32_t layer = 0; uint32_t value = 0; } z;
        struct { bool x = false, y = false; } center;
        struct { bool x = false, y = false; } stretch;
        struct { int32_t x = 0, y = 0; } shift;
        struct { std::optional<int32_t> left, right, top, bottom; } anchor;
        std::optional<uint32_t> resX, resY; // interface intended resolution width x height
        bool tiled = false;
        std::string parentInterface;

        struct { std::optional<uint32_t> background, border; } color;

        Widget* parentWidget = nullptr;

    protected:

        WidgetComponent* baseComponent = nullptr;
    };

    inline void Widget::resize(uint32_t width, uint32_t height)
    {
        setRect(rect.left, rect.top, rect.left + static_cast<int32_t>(width), rect.top + static_cast<int32_t>(height));
    }

    inline const Rect& Widget::effectiveRect() const {

        return rect;
    }

    inline uint32_t Widget::drawOrder() const
    {
        return z.value;
    }

    inline void Widget::setTopMost(bool value)
    {
        z.layer = value ? 1 : 0;
    }

    inline bool Widget::isTopMost() const
    {
        return z.layer == 1;
    }

    inline void Widget::setVisible(bool value)
    {
        setNodeMask(value ? ~0 : 0);
    }

    inline bool Widget::isVisible() const
    {
        return getNodeMask() != 0;
    }

    inline bool Widget::isCommonControl() const
    {
        return !attr.tmpl.empty();
    }

    inline const std::string& Widget::commonTemplate() const
    {
        return attr.tmpl;
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