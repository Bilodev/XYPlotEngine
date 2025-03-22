#include <math.h>
#include <stdio.h>

#include "Engine/Engine.h"
#define M_PI 3.14159265358979323846

double f(double x) { return x * x; }
double constant(double x)
{
    if (20 < x && x < 30) return 3;
    return 1;
}

int main()
{
    int error;

    engine_setname("My Loved Functions");

    error = engine_feed_function("log", log);
    error = engine_feed_function("exp", exp);
    error = engine_feed_function("sqr", f);
    error = engine_feed_function("sqrt", sqrt);

    printf("%s", engine_get_error());

    engine_draw();
    return 0;
}