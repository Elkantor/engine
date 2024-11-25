#pragma once

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include "../../../libs/raylib/raylib.h"
#include "../moduleHelper.c"
#include "../utils/memory/memoryStack.c"
#include "../utils/types.c"

#undef module
#define module gui

typedef struct 
{
    uPtr m_selected;
    uPtr m_hovered;
} guiGlobalData;

typedef struct
{
    vec2i32 m_pos;
    vec2i32 m_size;
    uPtr m_id;
} guiDiv;

typedef struct
{
    percent m_left, m_right, m_top, m_bottom;
} guiMargins;

fn(u32, beginGui, memoryStackStruct* _stack, const vec2i32 _pos, vec2i32 _size)
{
    memoryStack.push(_stack, sizeof(guiGlobalData));
    const u32 guiId = _stack->m_bytesAllocated - sizeof(guiGlobalData);

    guiDiv* root = memoryStack.push(_stack, sizeof(guiDiv));
    root->m_id = (uPtr)"root";
    root->m_pos = _pos;
    root->m_size = _size;

    DrawRectangle(root->m_pos.m_x, root->m_pos.m_y, root->m_size.m_x, root->m_size.m_y, (Color) { .r = 14, .g = 14, .b = 14, .a = 255 });

    return guiId;
}
end(beginGui)


fn(guiGlobalData, endGui, memoryStackStruct* _stack, const u32 _guiOffset)
{
    void* guiPtr = &_stack->m_bytesStack[_guiOffset];

    const guiGlobalData guiData = *((guiGlobalData*)&_stack->m_bytesStack[_guiOffset]);

    memoryStack.pop(_stack, guiPtr);

    return guiData;
}
end(endGui)


fn(u32, beginDiv, memoryStackStruct* _stack, const uPtr _id, const guiMargins _margins)
{
    const guiDiv* parent = (guiDiv*) &_stack->m_bytesStack[_stack->m_bytesAllocated - sizeof(guiDiv)];

    const u32 divID = _stack->m_bytesAllocated;
    guiDiv* div = memoryStack.push(_stack, sizeof(guiDiv));

    div->m_id = _id;
    
    const i32 w = parent->m_size.m_x;
    const i32 h = parent->m_size.m_y;
    const i32 marginLeft = (i32)(_margins.m_left * (f32)w);
    const i32 marginRight = (i32)(_margins.m_right * (f32)w);
    const i32 marginTop = (i32)(_margins.m_top * (f32)h);
    const i32 marginBottom = (i32)(_margins.m_bottom * (f32)h);

    div->m_pos.m_x = parent->m_pos.m_x + marginLeft;
    div->m_pos.m_y = parent->m_pos.m_y + marginTop;

    div->m_size.m_x = parent->m_size.m_x - marginRight - marginLeft;
    div->m_size.m_y = parent->m_size.m_y - marginBottom - marginTop;

    if (IsKeyDown(KEY_F1))
    {
        DrawRectangleLines(div->m_pos.m_x, div->m_pos.m_y, div->m_size.m_x, div->m_size.m_y, BLUE);
    }

    return divID;
}
end(beginDiv)


fn(void, endDiv, memoryStackStruct* _stack, const u32 _div)
{
    void* ptr = (void*) &_stack->m_bytesStack[_div];
    memoryStack.pop(_stack, ptr);
}
end(endDiv)


const struct internalTextMetrics { u32 m_fontIndex; f32 m_fontSize; f32 m_fontSpacing; Vector2 m_measuredSize; f32 m_x, m_y, m_w, m_h; }
guiInternalTextMetricsGet(const memoryStackStruct* _stack, const char* _text, const Font _fonts[], const u32 _count)
{
    const guiDiv* parent = (guiDiv*)&_stack->m_bytesStack[_stack->m_bytesAllocated - sizeof(guiDiv)];
    const f32 x = (f32)parent->m_pos.m_x;
    const f32 y = (f32)parent->m_pos.m_y;
    const f32 w = (f32)parent->m_size.m_x;
    const f32 h = (f32)parent->m_size.m_y;

    u32 fontIndex = 0;
    const f32 fontSpacing = 0.5f;
    f32 fontSize = 100.f;
    Vector2 measuredSize = MeasureTextEx(_fonts[fontIndex], _text, fontSize, fontSpacing);

    while ((measuredSize.x > w || measuredSize.y > h) && fontIndex < _count)
    {
        fontSize -= 1.f;
        measuredSize = MeasureTextEx(_fonts[fontIndex], _text, fontSize, fontSpacing);

        if (fontSize < (f32)_fonts[fontIndex].baseSize)
        {
            fontIndex += 1;
        }
    }

    printf("Font index: %d\n", fontIndex);
    printf("Font size: %f\n", fontSize);

    return (struct internalTextMetrics)
    { 
        .m_fontIndex = fontIndex, .m_fontSize = fontSize, .m_fontSpacing = fontSpacing, .m_measuredSize = measuredSize,
        .m_x = x, .m_y = y, .m_w = w, .m_h = h
    };
}


fn(void, textCenter, memoryStackStruct* _stack, const char* _text, const Font _fonts[], const u32 _count, const Color _color)
{
    const struct internalTextMetrics metrics = guiInternalTextMetricsGet(_stack, _text, _fonts, _count);

    const f32 offsetX = (metrics.m_w - metrics.m_measuredSize.x) / 2.f;
    const f32 offsetY = (metrics.m_h - metrics.m_measuredSize.y) / 2.f;

    DrawTextEx(_fonts[metrics.m_fontIndex], _text, (Vector2) { metrics.m_x + offsetX, metrics.m_y + offsetY }, metrics.m_fontSize, metrics.m_fontSpacing, _color);
}
end(textCenter)


fn(void, textLeft, memoryStackStruct* _stack, const char* _text, const Font _fonts[], const u32 _count, const Color _color)
{
    const struct internalTextMetrics metrics = guiInternalTextMetricsGet(_stack, _text, _fonts, _count);
    
    const f32 offsetY = (metrics.m_h - metrics.m_measuredSize.y) / 2.f;

    DrawTextEx(_fonts[metrics.m_fontIndex], _text, (Vector2) { metrics.m_x, metrics.m_y + offsetY }, metrics.m_fontSize, metrics.m_fontSpacing, _color);
}
end(textLeft)


fn(void, textRight, memoryStackStruct* _stack, const char* _text, const Font _fonts[], const u32 _count, const Color _color)
{
    const struct internalTextMetrics metrics = guiInternalTextMetricsGet(_stack, _text, _fonts, _count);

    const f32 offsetX = (metrics.m_w - metrics.m_measuredSize.x);
    const f32 offsetY = (metrics.m_h - metrics.m_measuredSize.y) / 2.f;

    DrawTextEx(_fonts[metrics.m_fontIndex], _text, (Vector2) { metrics.m_x + offsetX, metrics.m_y + offsetY }, metrics.m_fontSize, metrics.m_fontSpacing, _color);
}
end(textRight)


fn(void, button, memoryStackStruct* _stack, const uPtr _id)
{
    const guiDiv* parent = (guiDiv*)&_stack->m_bytesStack[_stack->m_bytesAllocated - sizeof(guiDiv)];

    const f32 x = (f32)parent->m_pos.m_x;
    const f32 y = (f32)parent->m_pos.m_y;
    const f32 w = (f32)parent->m_size.m_x;
    const f32 h = (f32)parent->m_size.m_y;

    DrawRectangleRounded((RL_Rectangle) { x + 1.f, y - 1.f, w, h }, 0.5f, 5.f, (Color) { .r = 60, .g = 60, .b = 60, .a = 255 });
    DrawRectangleRounded((RL_Rectangle) { x - 1.f, y + 1.f, w, h }, 0.5f, 5.f, BLACK);
    DrawRectangleRounded((RL_Rectangle) { x, y, w, h }, 0.5f, 5.f, (Color) { .r = 30, .g = 30, .b = 30, .a = 255 });
}
end(button)


fn(void, buttonHovered, memoryStackStruct* _stack, const uPtr _id)
{
    const guiDiv* parent = (guiDiv*)&_stack->m_bytesStack[_stack->m_bytesAllocated - sizeof(guiDiv)];

    const f32 x = (f32)parent->m_pos.m_x;
    const f32 y = (f32)parent->m_pos.m_y;
    const f32 w = (f32)parent->m_size.m_x;
    const f32 h = (f32)parent->m_size.m_y;

    // Border top
    DrawRectangleRounded((RL_Rectangle) { x + 1.f, y - 1.f, w, h }, 0.5f, 5.f, (Color) { .r = 60, .g = 60, .b = 60, .a = 255 });
    
    // Border bottom
    DrawRectangleRounded((RL_Rectangle) { x - 3.f, y + 3.f, w, h }, 0.5f, 5.f, (Color) { .r = 0, .g = 0, .b = 0, .a = 50});
    DrawRectangleRounded((RL_Rectangle) { x - 2.f, y + 2.f, w, h }, 0.5f, 5.f, (Color) { .r = 0, .g = 0, .b = 0, .a = 100});
    DrawRectangleRounded((RL_Rectangle) { x - 1.f, y + 1.f, w, h }, 0.5f, 5.f, BLACK);
    
    DrawRectangleRounded((RL_Rectangle) { x, y, w, h }, 0.5f, 5.f, (Color) { .r = 40, .g = 40, .b = 40, .a = 255 });
}
end(buttonHovered)


fn(void, buttonClicked, memoryStackStruct* _stack, const uPtr _id)
{
    const guiDiv* parent = (guiDiv*)&_stack->m_bytesStack[_stack->m_bytesAllocated - sizeof(guiDiv)];

    const f32 x = (f32)parent->m_pos.m_x;
    const f32 y = (f32)parent->m_pos.m_y;
    const f32 w = (f32)parent->m_size.m_x;
    const f32 h = (f32)parent->m_size.m_y;

    DrawRectangleRounded((RL_Rectangle) { x + 4.f, y - 4.f, w, h }, 0.5f, 5.f, (Color) { .r = 30, .g = 30, .b = 30, .a = 255 });
    DrawRectangleRounded((RL_Rectangle) { x + 2.f, y - 2.f, w, h }, 0.5f, 5.f, (Color) { .r = 60, .g = 60, .b = 60, .a = 255 });
    DrawRectangleRounded((RL_Rectangle) { x - 4.f, y + 4.f, w, h }, 0.5f, 5.f, BLACK);
    DrawRectangleRounded((RL_Rectangle) { x, y, w, h }, 0.5f, 5.f, (Color) { .r = 60, .g = 60, .b = 60, .a = 255 });
}
end(buttonClicked)


fn(void, tooltip, memoryStackStruct* _stack, const char* _text, const Font _fonts[], const u32 _count, const Color _textColor)
{
    const guiDiv* parent = (guiDiv*)&_stack->m_bytesStack[_stack->m_bytesAllocated - sizeof(guiDiv)];

    const i32 posX = parent->m_pos.m_x;
    const i32 posY = (parent->m_pos.m_y + parent->m_size.m_y) + 5;

    const u32 root = gui_beginGui(_stack, (vec2i32) { posX, posY }, (vec2i32) { parent->m_size.m_x, parent->m_size.m_y });
    {
        const guiDiv* parentRoot = (guiDiv*)&_stack->m_bytesStack[_stack->m_bytesAllocated - sizeof(guiDiv)];
        const f32 w = (f32)parentRoot->m_size.m_x;
        const f32 h = (f32)parentRoot->m_size.m_y;
        const f32 x = (f32)parentRoot->m_pos.m_x;
        const f32 y = (f32)parentRoot->m_pos.m_y;
        
        DrawRectangleRounded((RL_Rectangle) { x + 1.f, y - 1.f, w, h }, 0.5f, 5.f, (Color) { .r = 60, .g = 60, .b = 60, .a = 255 });
        DrawRectangleRounded((RL_Rectangle) { x - 1.f, y + 1.f, w, h }, 0.5f, 5.f, (Color) { .r = 60, .g = 60, .b = 60, .a = 255 });
        DrawRectangleRounded((RL_Rectangle) { x, y, w, h }, 0.5f, 5.f, (Color) { .r = 33, .g = 36, .b = 42, .a = 255 });

        const u32 divText = gui_beginDiv(_stack, (uPtr)"divTooltip", (guiMargins) { .m_left = 0.2f, .m_right = 0.2f, .m_top = 0.2f, .m_bottom = 0.2f });
        {
            gui_textCenter(_stack, _text, _fonts, _count, _textColor);
        }
        gui_endDiv(_stack, divText);
    }
    gui_endGui(_stack, root);
}
end(tooltip)


void slider(const memoryStackStruct* const _stack, const f32 _percent)
{
    const guiDiv* parent = (guiDiv*)&_stack->m_bytesStack[_stack->m_bytesAllocated - sizeof(guiDiv)];

    const i32 x = parent->m_pos.m_x;
    const i32 y = parent->m_pos.m_y;
    const i32 w = parent->m_size.m_x;
    const i32 h = parent->m_size.m_y;

    DrawRectangleRounded((RL_Rectangle) { (f32)x + 1.f, (f32)y - 1.f, (f32)w, (f32)h }, 0.5f, 5.f, (Color) { .r = 60, .g = 60, .b = 60, .a = 255 });
    DrawRectangleRounded((RL_Rectangle) { (f32)x - 1.f, (f32)y + 1.f, (f32)w, (f32)h }, 0.5f, 5.f, (Color) { .r = 60, .g = 60, .b = 60, .a = 255 });
    DrawRectangleRounded((RL_Rectangle) { (f32)x, (f32)y, (f32)w, (f32)h }, 0.5f, 5.f, (Color) { .r = 14, .g = 14, .b = 14, .a = 255 });

    // Draw the line
    const f32 lineThickness = 3.f;
    const Color lineColor = (Color){ .r = 120, .g = 120, .b = 120, .a = 200 };
    const Vector2 lineP1 = { .x = (f32)x, .y = (f32)y + (f32)h / 2.f };
    const Vector2 lineP2 = { .x = (f32)x + (f32)w, .y = (f32)y + (f32)h / 2.f };
    DrawLineEx(lineP1, lineP2, lineThickness, lineColor);

    // Draw the percentages
    const f32 fractionThickness = 2.f;
    const Color fractionColor = (Color){ .r = 170, .g = 170, .b = 170, .a = 230 };

    for (i32 i = 1; i < 10; ++i)
    {
        const Vector2 p1 = { .x = lineP1.x + (f32)w * (f32)i / 10.f, .y = (f32)y + (f32)h / 2.f };
        const Vector2 p2 = { .x = lineP1.x + (f32)w * (f32)i / 10.f, .y = (f32)y + (f32)h / 2.f + 2.f};
        DrawLineEx(p1, p2, fractionThickness, fractionColor);
    }

    // Draw the cursor
    const f32 thickness = (f32)h * 0.75f;
    const RL_Rectangle pointerRec = { .x = (f32)x + (f32)w / 2.f, .y = (f32)y, .width = thickness, .height = (f32)h - (f32)h / 2.f };
    const Color pointerColor = (Color){ .r = 240, .g = 240, .b = 240, .a = 255 };
    DrawRectangleRec(pointerRec, pointerColor);

    const Vector2 p1 = { .x = pointerRec.x, .y = pointerRec.y + pointerRec.height };
    const Vector2 p2 = { .x = pointerRec.x + pointerRec.width / 2.f, .y = (f32)y + (f32)h };
    const Vector2 p3 = {.x = pointerRec.x + pointerRec.width, .y = pointerRec.y + pointerRec.height };

    DrawTriangle(p1, p2, p3, pointerColor);
}


fn(_Bool, hovered, const memoryStackStruct* const _stack, const uPtr _id)
{
    const guiDiv* parent = (guiDiv*)&_stack->m_bytesStack[_id];

    const f32 x = (f32)parent->m_pos.m_x;
    const f32 y = (f32)parent->m_pos.m_y;
    const f32 w = (f32)parent->m_size.m_x;
    const f32 h = (f32)parent->m_size.m_y;

    return CheckCollisionPointRec(GetMousePosition(), (RL_Rectangle) { x, y, w, h });
}
end(hovered)

fn(void, sliderHorizontal, memoryStackStruct* _stack, const uPtr _id, const f32 _percent)
{

}
end(sliderHorizontal)

module_export
(
    beginGui,
    endGui,
    beginDiv,
    endDiv,
    button,
    buttonHovered,
    buttonClicked,
    hovered,
    textCenter,
    textLeft,
    textRight,
    tooltip
)

#undef guiInternalBeginText
#undef guiInternalEndText