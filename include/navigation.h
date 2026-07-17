#ifndef NAVIGATION_H
#define NAVIGATION_H
#include "config.h"
#include "la.h"
#include <stdbool.h>

#define VELOCITY_THRESHOLD 15.0

typedef struct {
    Vec2f curr;
    Vec2f prev;
    bool drag;
} Mouse;

typedef struct {
    Vec2f position;
    Vec2f velocity;
    Vec2f scale_pivot;
    double scale;
    double delta_scale;
} Camera;

Vec2f world(Camera camera, Vec2f v);
void update(Camera *camera, Config config, double dt, Mouse mouse,
            Vec2f window_size);

#endif
