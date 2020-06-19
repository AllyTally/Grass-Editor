#pragma once

#ifndef EDITOR_H
#define EDITOR_H

#include "map.h"

namespace editor
{
    map::room current_room;
    int current_tileset;
    void EditorInit();
    void EditorLogic();
    void EditorRender();
    map::room CreateEmptyRoom();
};

#endif