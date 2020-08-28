#pragma once

#ifndef TILESETEDITOR_H
#define TILESETEDITOR_H

namespace tileset_editor
{
    int Logic();
    void Render();
    void DrawBlock(int x, int y, int tileset);
    void DrawTile(int x, int y, int tileset, int index);
};

#endif