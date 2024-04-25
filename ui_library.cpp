#include "raylib.h"
#include <iostream>

struct UiLibrary
{
    int hot = -1;
    int active = -1;

    bool Button(int id, const std::string& text, const Rectangle& bounds);
    void ProgressBar(const std::string& label, float remainingTime, float totalTime, const Rectangle& bounds);
};

bool UiLibrary::Button(int id, const std::string& text, const Rectangle& bounds)
{
    bool result = false;
    int fontSize = 30;

    // If this button is the currently active widget, that means
    // the user is currently interacting with this widget
    if (id == active)
    {
        // If the user released the mouse button while we are active,
        // register as a click
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            // Make sure that we are still the hot widget
            // (User might have dragged mouse to another widget, in which case we will not count as a button click)
            if (id == hot)
            {
                result = true;
            }

            // Set ourselves to not be active anymore
            active = -1;
        }
    }

    // If we are currently the hot widget
    if (id == hot)
    {
        // If the user pressed the left mouse button, that means the user started
        // interacting with this widget, so we set this widget as active
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            active = id;
        }
    }

    if (CheckCollisionPointRec(GetMousePosition(), bounds))
    {
        // Set this widget to be the hot widget
        hot = id;

        // Change background color on hover
        DrawRectangleRec(bounds, VIOLET);

        // Center the text horizontally and vertically based on font size
        DrawText(text.c_str(), bounds.x + bounds.width / 2 - MeasureText(text.c_str(), fontSize) / 2, 
                bounds.y + bounds.height / 2 - (fontSize/2), fontSize, BLACK);

        // Change background and text color on click
        if (id == active)
        {
            DrawRectangleRec(bounds, DARKPURPLE);

            // Center the text horizontally and vertically based on font size
            DrawText(text.c_str(), bounds.x + bounds.width / 2 - MeasureText(text.c_str(), fontSize) / 2, 
                    bounds.y + bounds.height / 2 - (fontSize/2), fontSize, RAYWHITE);
        }
    }
    else
    {
        if (hot == id)
        {
            hot = -1;
        }

        // Revert to original colors
        DrawRectangleRec(bounds, PURPLE);

        // Center the text horizontally and vertically based on font size
        DrawText(text.c_str(), bounds.x + bounds.width / 2 - MeasureText(text.c_str(), fontSize) / 2, 
                bounds.y + bounds.height / 2 - (fontSize/2), fontSize, BLACK);
    }

    return result;
}

void UiLibrary::ProgressBar(const std::string& label, float remainingTime, float totalTime, const Rectangle& bounds)
{
    // calculate progress
    float progress = 1.0f - (remainingTime / totalTime);

    // background
    DrawRectangleRec(bounds, LIGHTGRAY);

    // progress filling thing
    Rectangle progressRect = { bounds.x, bounds.y, bounds.width * progress, bounds.height };
    DrawRectangleRec(progressRect, remainingTime <= 0.0f ? BLUE : RED);

    // label text
    DrawText(label.c_str(), bounds.x, bounds.y - 22, 20, WHITE);
}