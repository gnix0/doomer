#ifndef SCREENSHOT_H
#define SCREENSHOT_H
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

typedef struct {
    XImage *image;
    XShmSegmentInfo shminfo;
    int uses_shm;
} Screenshot;

Screenshot new_screenshot(Display *display, Window window);
void destroy_screenshot(Screenshot *screenshot, Display *display);
int refresh_screenshot(Screenshot *screenshot, Display *display, Window window);
int save_screenshot_to_ppm(const XImage *image, const char *file_path);

#endif
