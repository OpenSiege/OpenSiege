
#pragma once

#include "Widget.hpp"

#if defined (WIN32) && defined (DialogBox)
#undef DialogBox
#endif

namespace ehb
{
    class DialogBox : public Widget
    {
    public:

        DialogBox(Shell & shell);

        virtual ~DialogBox() = default;

        virtual void createCommonCtrl(const std::string & value) override;
    };
}
