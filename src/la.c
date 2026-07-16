#include "la.h"
#include <math.h>

Vec2f vec_create(double x, double y)
{
    return (Vec2f){
        .x_axis = x,
        .y_axis = y,
    };
}

Vec2f vec_scale(Vec2f v, double s)
{
    return vec_create(v.x_axis * s, v.y_axis * s);
}

Vec2f vec_div_scalar(Vec2f v, double s)
{
    return vec_create(v.x_axis / s, v.y_axis / s);
}

Vec2f vec_mul(Vec2f v1, Vec2f v2)
{
    return vec_create(v1.x_axis * v2.x_axis, v1.y_axis * v2.y_axis);
}

Vec2f vec_div(Vec2f v1, Vec2f v2)
{
    return vec_create(v1.x_axis / v2.x_axis, v1.y_axis / v2.y_axis);
}

Vec2f vec_add(Vec2f v1, Vec2f v2)
{
    return vec_create(v1.x_axis + v2.x_axis, v1.y_axis + v2.y_axis);
}

Vec2f vec_sub(Vec2f v1, Vec2f v2)
{
    return vec_create(v1.x_axis - v2.x_axis, v1.y_axis - v2.y_axis);
}

void vec_add_assign(Vec2f *v1, Vec2f v2)
{
    v1->x_axis += v2.x_axis;
    v1->y_axis += v2.y_axis;
}

void vec_sub_assign(Vec2f *v1, Vec2f v2)
{
    v1->x_axis -= v2.x_axis;
    v1->y_axis -= v2.y_axis;
}

double vec_length(Vec2f v)
{
    return hypot(v.x_axis, v.y_axis);
}

Vec2f vec_normalize(Vec2f v)
{
    double len = vec_length(v);
    if (len == 0.0)
        return vec_create(0.0, 0.0);

    return vec_create(v.x_axis / len, v.y_axis / len);
}
