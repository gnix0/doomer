#include "screenshot.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

static int try_shm_screenshot(Screenshot *screenshot, Display *display,
                              Window window,
                              const XWindowAttributes *attributes)
{
    if (!XShmQueryExtension(display))
        return 0;

    int screen = DefaultScreen(display);

    screenshot->image = XShmCreateImage(
        display, DefaultVisual(display, screen), DefaultDepth(display, screen),
        ZPixmap, NULL, &screenshot->shminfo, (unsigned int)attributes->width,
        (unsigned int)attributes->height);

    if (screenshot->image == NULL)
        return 0;

    size_t image_size = (size_t)screenshot->image->bytes_per_line *
                        (size_t)screenshot->image->height;

    screenshot->shminfo.shmid =
        shmget(IPC_PRIVATE, image_size, IPC_CREAT | 0600);
    if (screenshot->shminfo.shmid == -1) {
        XDestroyImage(screenshot->image);

        screenshot->image = NULL;
        return 0;
    }

    screenshot->shminfo.shmaddr = shmat(screenshot->shminfo.shmid, NULL, 0);
    if (screenshot->shminfo.shmaddr == (void *)-1) {
        shmctl(screenshot->shminfo.shmid, IPC_RMID, NULL);

        XDestroyImage(screenshot->image);

        screenshot->image = NULL;
        return 0;
    }

    screenshot->image->data = screenshot->shminfo.shmaddr;
    screenshot->shminfo.readOnly = False;

    if (!XShmAttach(display, &screenshot->shminfo)) {
        shmdt(screenshot->shminfo.shmaddr);
        shmctl(screenshot->shminfo.shmid, IPC_RMID, NULL);

        XDestroyImage(screenshot->image);

        screenshot->image = NULL;
        return 0;
    }

    XSync(display, False);

    if (!XShmGetImage(display, window, screenshot->image, 0, 0, AllPlanes)) {
        XShmDetach(display, &screenshot->shminfo);
        XSync(display, False);

        shmdt(screenshot->shminfo.shmaddr);
        shmctl(screenshot->shminfo.shmid, IPC_RMID, NULL);

        XDestroyImage(screenshot->image);

        screenshot->image = NULL;
        return 0;
    }

    screenshot->uses_shm = 1;

    return 1;
}

Screenshot new_screenshot(Display *display, Window window)
{
    Screenshot screenshot = {0};
    XWindowAttributes attributes;

    if (!XGetWindowAttributes(display, window, &attributes))
        return screenshot;

    if (try_shm_screenshot(&screenshot, display, window, &attributes))
        return screenshot;

    screenshot.image =
        XGetImage(display, window, 0, 0, (unsigned int)attributes.width,
                  (unsigned int)attributes.height, AllPlanes, ZPixmap);

    screenshot.uses_shm = 0;

    return screenshot;
}

void destroy_screenshot(Screenshot *screenshot, Display *display)
{
    if (screenshot == NULL || screenshot->image == NULL)
        return;

    if (screenshot->uses_shm) {
        XSync(display, False);
        XShmDetach(display, &screenshot->shminfo);
        XSync(display, False);

        XDestroyImage(screenshot->image);

        shmdt(screenshot->shminfo.shmaddr);
        shmctl(screenshot->shminfo.shmid, IPC_RMID, NULL);
    } else {
        XDestroyImage(screenshot->image);
    }

    *screenshot = (Screenshot){0};
}

int refresh_screenshot(Screenshot *screenshot, Display *display, Window window)
{
    if (screenshot == NULL || screenshot->image == NULL)
        return 0;

    XWindowAttributes attributes;

    if (!XGetWindowAttributes(display, window, &attributes))
        return 0;

    if (attributes.width != screenshot->image->width ||
        attributes.height != screenshot->image->height) {
        destroy_screenshot(screenshot, display);
        *screenshot = new_screenshot(display, window);

        return screenshot->image != NULL;
    }

    if (screenshot->uses_shm) {
        if (!XShmGetImage(display, window, screenshot->image, 0, 0,
                          AllPlanes)) {
            destroy_screenshot(screenshot, display);
            *screenshot = new_screenshot(display, window);
        }
    } else {
        XImage *image = XGetSubImage(
            display, window, 0, 0, (unsigned int)screenshot->image->width,
            (unsigned int)screenshot->image->height, AllPlanes, ZPixmap,
            screenshot->image, 0, 0);

        if (image == NULL) {
            destroy_screenshot(screenshot, display);
            *screenshot = new_screenshot(display, window);
        } else {
            screenshot->image = image;
        }
    }

    return screenshot->image != NULL;
}

int save_screenshot_to_ppm(const XImage *image, const char *file_path)
{
    if (image == NULL || file_path == NULL)
        return 0;

    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        perror("fopen");
        return 0;
    }

    if (fprintf(file, "P6\n%d %d\n255\n", image->width, image->height) < 0) {
        fclose(file);
        return 0;
    }

    for (int y = 0; y < image->height; y++) {
        const unsigned char *row = (const unsigned char *)image->data +
                                   (size_t)y * (size_t)image->bytes_per_line;

        for (int x = 0; x < image->width; x++) {
            const unsigned char *pixel = row + (size_t)x * 4;

            unsigned char rgb[3] = {pixel[2], pixel[1], pixel[0]};

            if (fwrite(rgb, sizeof rgb, 1, file) != 1) {
                fclose(file);
                return 0;
            }
        }
    }

    if (fclose(file) == EOF)
        return 0;

    return 1;
}
