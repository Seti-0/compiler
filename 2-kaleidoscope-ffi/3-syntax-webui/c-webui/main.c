#include "webui.h"

void test_webui() {
	size_t myWindow = webui_new_window();
    webui_show(myWindow, "<html><script src=\"webui.js\"></script> Hello World from C! </html>");
    webui_wait();
}