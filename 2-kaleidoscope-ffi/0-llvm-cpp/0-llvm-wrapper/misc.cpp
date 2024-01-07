#include "misc.h"

#include <conio.h>
#include <windows.h>

int LW_Getch() {
    return _getch();
}

SHORT LW_GetKeyState(int virtual_key) {
    return GetKeyState(virtual_key);
}

void LW_CopyToClipboard(char* text) {
    size_t size = strlen(text) + 1;

    // This is adapted from: 
    // https://learn.microsoft.com/en-us/windows/win32/dataxchg/using-the-clipboard

    // Write to win32 allocated memory. 
    // Apparently malloc doesn't work well here, something about sharing 
    // memory between loaded modules.

    // I've seen this fail with an error code 5 (unauthorized access)
    // before, though it seems very rare and I don't know why it happens.
    // I don't have any error tracking code here, so this wouldn't be easy to debug!

    HGLOBAL buffer = GlobalAlloc(GMEM_MOVEABLE, size + 1);
    if (buffer == NULL) {
        return;
    }

    char *locked_buffer = static_cast<char*>(GlobalLock(buffer));
    if (locked_buffer == NULL) {
        return;
    }

    memcpy(locked_buffer, text, size);
    locked_buffer[size] = '\0';

    if ((!GlobalUnlock(buffer)) && GetLastError() != NO_ERROR) {
       return;
    }

    // Set clipboard to our win32 allocated memory.
    
    // This is obviously hardly a natural way to write it :')
    // But it's a way of making sure each operation only 
    // executes if the previous one succeeded... 
    (!OpenClipboard(NULL)) 
    && (!EmptyClipboard()) 
    && (!SetClipboardData(CF_TEXT, buffer)) 
    && (!CloseClipboard());
}