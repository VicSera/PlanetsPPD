//
// Created by vsera on 1/5/2022.
//

#include "graphics.h"
#include <iostream>

void drawCircle(const Planet& p, HDC hdc, HWND hwnd)
{
    int radius = (int)p.radius;

    // Draw new circle
    int cx = (int)p.x, cy = (int)p.y;
    Ellipse(hdc, cx - radius, cy - radius, cx + radius, cy + radius);

    // Erase invalid pixels from old circle
    int oldCx = (int)p.oldX, oldCy = (int)p.oldY;
    for (int x = oldCx - radius; x <= oldCx + radius; x += 1)
        for (int y = oldCy - radius; y <= oldCy + radius; y += 1)
            if ((x - cx) * (x - cx) + (y - cy) * (y - cy) > radius * radius)
            {
                SetPixel(hdc, x, y, RGB(12, 12, 12));
            }
}
