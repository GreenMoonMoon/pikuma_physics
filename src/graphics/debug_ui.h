//
// Created by josue on 2025-09-24.
//

#ifndef PIKUMA_PHYSICS_DEBUG_UI_H
#define PIKUMA_PHYSICS_DEBUG_UI_H
#include <vector>
#include "raylib.h"
#include "layout.h"

enum class LayoutItemType {
    WINDOWBOX,      // StatusBar, Panel
    GROUPBOX,       // Line
    LINE,
    PANEL,          // StatusBar
    SCROLLPANEL,    // StatusBar
    TABBAR,         // Button
 // Basic Controls
    LABEL,
    LABELBUTTON,    // Label
    BUTTON,
    TOGGLE,
    TOGGLEGROUP,    // Toggle
    TOGGLESLIDER,
    CHECKBOX,
    COMBOBOX,
    DROPDOWNBOX,
    TEXTBOX,
    VALUEBOX,       // TextBox
    SPINNER,        // Button, ValueBox
    SLIDER,
    SLIDERBAR,      // Slider
    PROGRESSBAR,
    STATUSBAR,
    DUMMYREC,
    GRID,
    // Advance Controls
    LISTVIEW,
    COLORPICKER,    // ColorPanel, ColorBarHue
    MESSAGEBOX,     // Window, Label, Button
    TEXTINPUTBOX,   // Window, Label, TextBox, Button
};

struct LayoutItem {
    lay_id id;
    LayoutItemType type;
};

class DebugWindow {
    // layout
    lay_context context;
    lay_id root;
    bool need_rebuilding;
    std::vector<LayoutItem> layout_items;

    int width, height;

    bool show;
    bool grabbing;
    Vector2 grab_offset;
    bool resizing;
    Vector2 resize_offset;

public:
    DebugWindow();
    ~DebugWindow() = default;

    void Inputs();
    void Update();
    void Draw();
};

#endif //PIKUMA_PHYSICS_DEBUG_UI_H