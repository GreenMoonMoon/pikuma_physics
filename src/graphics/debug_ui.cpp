
//
// Created by josue on 2025-09-24.
//

#include "debug_ui.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define LAY_IMPLEMENTATION
#include "layout.h"

#define VEC4_2_RECTANGLE(V) ((Rectangle){static_cast<float>(V[0]), static_cast<float>(V[1]), static_cast<float>(V[2]), static_cast<float>(V[3])})

DebugWindow::DebugWindow() {
    lay_init_context(&context);
    lay_reserve_items_capacity(&context, 64);
    root = lay_item(&context);
    layout_items.emplace_back(root, LayoutItemType::WINDOWBOX);

    width = 400;
    height = 600;

    show = true;
    grabbing = false;
    grab_offset = (Vector2){};
    resizing = false;
    resize_offset = (Vector2){};

    // Build the layout
    lay_set_size_xy(&context, root, width, height);

    // top container
    lay_set_contain(&context, root, LAY_ROW);
    lay_set_margins(&context, root, (lay_vec4){10, 10, 35, 10});

    // scene panel
    lay_id panel = lay_item(&context);
    lay_insert(&context, root, panel);
    lay_set_behave(&context, panel, LAY_FILL);
    lay_set_contain(&context, panel, LAY_COLUMN);
    lay_set_margins_ltrb(&context, panel, 10, 35, 10, 10);
    layout_items.emplace_back(panel, LayoutItemType::PANEL);

    // scene buttons
    const int scene_count = 3;
    for (int i = 0; i < scene_count; ++i) {
        lay_id button = lay_item(&context);
        lay_insert(&context, panel, button);
        lay_set_size_xy(&context, button, 100, 0);
        lay_set_behave(&context, button, LAY_FILL);
        lay_set_margins_ltrb(&context, button, 5, 30, 2, 5);
        layout_items.emplace_back(button, LayoutItemType::BUTTON);
    }

    need_rebuilding = true;
}

void DebugWindow::Inputs() {
    if (IsKeyPressed(KEY_GRAVE)) { show = true; }
    if (!show) { return; } // Don't handle inputs if the window is not visible

    // Handle grabbing the window title bar
    if (const Vector2 mouse_position = GetMousePosition(); IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && (!grabbing || !resizing)) {
        if (const auto rect = VEC4_2_RECTANGLE(lay_get_rect(&context, root)); CheckCollisionPointRec(mouse_position, (Rectangle){rect.x, rect.y, rect.width, 25})) {
            grabbing = true;
            grab_offset = Vector2Subtract(mouse_position, (Vector2){rect.x, rect.y});
        // Handle window resizing
        } else if (CheckCollisionPointRec(mouse_position, (Rectangle){rect.x + rect.width - 10, rect.y + rect.height - 10, 10, 10})) {
            resizing = true;
            resize_offset = Vector2Subtract(mouse_position, (Vector2){rect.x + rect.width, rect.y + rect.height});
        }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        grabbing = false;
        resizing = false;
    }
}

void DebugWindow::Update() {
    if (need_rebuilding) {
        lay_run_context(&context);
    }
}

void DebugWindow::Draw() {
    const lay_vec4 window_rect = lay_get_rect(&context, root);
    GuiWindowBox((Rectangle){static_cast<float>(window_rect[0]), static_cast<float>(window_rect[1]), static_cast<float>(window_rect[2]), static_cast<float>(window_rect[3])}, "DEBUG");

    for (const auto layout_item: layout_items) {
        const auto rect = VEC4_2_RECTANGLE(lay_get_rect(&context, layout_item.id));

        switch (layout_item.type) {
            case LayoutItemType::WINDOWBOX:
                GuiWindowBox(rect, "WINDOWBOX"); break;
            case LayoutItemType::GROUPBOX:
            case LayoutItemType::LINE:
            case LayoutItemType::PANEL:
                GuiPanel(rect, "PANEL"); break;
            case LayoutItemType::SCROLLPANEL:
            case LayoutItemType::TABBAR:
            case LayoutItemType::LABEL:
            case LayoutItemType::LABELBUTTON:
            case LayoutItemType::BUTTON:
                GuiButton(rect, "BUTTON"); break;
            case LayoutItemType::TOGGLE:
            case LayoutItemType::TOGGLEGROUP:
            case LayoutItemType::TOGGLESLIDER:
            case LayoutItemType::CHECKBOX:
            case LayoutItemType::COMBOBOX:
            case LayoutItemType::DROPDOWNBOX:
            case LayoutItemType::TEXTBOX:
            case LayoutItemType::VALUEBOX:
            case LayoutItemType::SPINNER:
            case LayoutItemType::SLIDER:
            case LayoutItemType::SLIDERBAR:
            case LayoutItemType::PROGRESSBAR:
            case LayoutItemType::STATUSBAR:
            case LayoutItemType::DUMMYREC:
            case LayoutItemType::GRID:
            case LayoutItemType::LISTVIEW:
            case LayoutItemType::COLORPICKER:
            case LayoutItemType::MESSAGEBOX:
            case LayoutItemType::TEXTINPUTBOX:
            default:
                break;
        }
    }

}
