#include <stdio.h>

int engine_setname(char window_title[]);
char* engine_get_error();
int engine_feed_function(char name[], double (*f)(double));
void engine_draw();
void engine_feed_point(char name[], double x, double y);