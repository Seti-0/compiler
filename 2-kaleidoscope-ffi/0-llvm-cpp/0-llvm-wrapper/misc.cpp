#include "misc.h"

#include <conio.h>
#include <windows.h>

int LW_Getch() {
    return _getch();
}

SHORT LW_GetKeyState(int virtual_key) {
    return GetKeyState(virtual_key);
}