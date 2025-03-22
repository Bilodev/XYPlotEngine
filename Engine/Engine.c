#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "raylib.h"
#include "utils.h"

#define WIDTH 1920
#define HEIGTH 1080
#define FPS 60

#define GRAPH_WIDTH 1000
#define GRAPH_HEIGTH 1000
#define MAX_FUNCTIONS 5
#define FUNCTION_NAME_LEN 10
#define MAX_POINTS 10
#define POINT_NAME_LEN 2

const Color colors[] = {BLUE, YELLOW, PURPLE, GREEN, RED};
const Color GRID_COLOR = {255, 255, 255, 20};
const Color GRID_COLOR_2 = {255, 255, 255, 80};
const Color BLACK_ = {22, 22, 22, 22};

char window_title[30] = "Graphs";
char engine_error[100] = "";

typedef struct {
    char name[FUNCTION_NAME_LEN + 1];
    double (*function)(double);
    Color color;
    Rectangle hitbox;
    int index;
} function;

int func_index = 0;
function* functions[MAX_FUNCTIONS];
int func_to_draw[MAX_FUNCTIONS] = {-1};
int func_to_draw_index = 0;

typedef struct {
    char name[POINT_NAME_LEN + 1];
    double x;
    double y;
} point;

point* points[MAX_POINTS];
int points_index = 0;
int points_to_draw[MAX_POINTS] = {-1};
int points_to_draw_index = 0;

// Graph Displacement
int x_displacement = -10;
int y_displacement = 10;

// Text boxes for (x,y) input
Rectangle XtextBox = {130, HEIGTH - 150, 100, 20};
Rectangle YtextBox = {130, HEIGTH - 100, 100, 20};
char x_text[4] = "0", *xtext_ptr = x_text;
char y_text[4] = "0", *ytext_ptr = y_text;

int engine_setname(char title[]) { sprintf(window_title, "%s", title); }

char* engine_get_error() { return engine_error; }

int engine_feed_point(char name[], double x, double y)
{
    if (points_index == MAX_POINTS) {
        sprintf(engine_error,
                "Unable to feed %s point, too many points were fed while "
                "the max is %d\n",
                name, MAX_POINTS);
        return 1;
    };

    if (strlen(name) > POINT_NAME_LEN) {
        sprintf(engine_error,
                "Point name %s is too long, point names can be at maximum %d "
                "chars\n",
                name, POINT_NAME_LEN);
        return 1;
    }

    point* p = malloc(sizeof(function));
    sprintf(p->name, "%s", name);
    p->x = x;
    p->y = y;
    points[points_index] = p;

    points_to_draw[points_to_draw_index] = points_index;
    points_to_draw_index++;

    points_index++;

    return 0;
}

int engine_feed_function(char name[], double (*f)(double))
{
    if (func_index == MAX_FUNCTIONS) {
        sprintf(engine_error,
                "Unable to feed %s function, toom nay functions were fed while "
                "the max is %d.\n",
                name, MAX_FUNCTIONS);
        return 1;
    };

    if (strlen(name) > FUNCTION_NAME_LEN) {
        sprintf(engine_error,
                "Function name %s is too long, point names can be at maximum "
                "%d chars.\n",
                name, FUNCTION_NAME_LEN);
        return 1;
    }
    if (strcmp(name, "") == 0) {
        sprintf(engine_error, "Function name cannot be empty.\n");
        return 1;
    }

    function* fn = malloc(sizeof(function));
    sprintf(fn->name, "%s", name);
    fn->function = f;
    Rectangle r = {100, func_index * 0.8f * 100 + 300, strlen(name) * 20 + 40,
                   30};
    fn->hitbox = r;
    fn->index = func_index;
    fn->color = colors[func_index];
    functions[func_index] = fn;

    func_to_draw[func_to_draw_index] = func_index;
    func_to_draw_index++;

    func_index++;

    return 0;
};

char* handle_text_input(Rectangle* box, char* text, char* text_ptr)
{
    if (!CheckCollisionPointRec(GetMousePosition(), *box)) return text_ptr;

    char c = GetCharPressed();

    if (c == '-' && strcmp(text_ptr, "0") == 0) {
        *text_ptr = c;
        *(text_ptr + 1) = '\0';
        text_ptr++;
    }

    else if ('0' <= c && c <= '9' && strlen(text) < 3) {
        *text_ptr = c;
        *(text_ptr + 1) = '\0';
        text_ptr++;
    }
    else if (IsKeyPressed(KEY_BACKSPACE) && strcmp(text, "0") != 0) {
        text_ptr--;
        *text_ptr = '\0';
        if (strlen(text) == 0) {
            *text_ptr = '0';
            *(text_ptr + 1) = '\0';
        }
    }
    else if (IsKeyPressed(KEY_ENTER) && strcmp(text, "-") != 0) {
        x_displacement = -10 + atoi(x_text);
        y_displacement = 10 - atoi(y_text);
    }
    return text_ptr;
}

void handle_fn_name_click()
{
    int function_id = -1;
    for (size_t i = 0; i < func_index; i++)
        if (CheckCollisionPointRec(GetMousePosition(), functions[i]->hitbox) &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            function_id = i;

    if (function_id == -1) return;

    int function_removed = 0;
    for (size_t i = 0; i < MAX_FUNCTIONS; i++)
        if (func_to_draw[i] == functions[function_id]->index) {
            func_to_draw[i] = -1;
            function_removed = 1;
        }

    // bisogna ributtarla dentro
    if (!function_removed) {
        for (size_t i = 0; i < MAX_FUNCTIONS; i++) {
            if (func_to_draw[i] == -1) {
                func_to_draw[i] = functions[function_id]->index;
                return;
            }
        }
    }
}

void draw_grid()
{
    int offsetx = (WIDTH - GRAPH_WIDTH) / 2;
    int offsety = (HEIGTH - GRAPH_HEIGTH) / 2;
    for (int i = -10; i <= 10; i++) {
        Color color = i == 0 ? GRID_COLOR_2 : GRID_COLOR;
        DrawLine(pam(i) + offsetx, offsety, pam(i) + offsetx,
                 GRAPH_HEIGTH + offsety, color);
        DrawLine(offsetx, pam(i) + offsety, GRAPH_WIDTH + offsetx,
                 pam(i) + offsety, color);
    }
}

void draw_graph()
{
    // x axis
    if (0 <= y_displacement && y_displacement <= 20)
        DrawLine(WIDTH / 2 - GRAPH_WIDTH / 2, HEIGTH / 2 + pam(-y_displacement),
                 WIDTH / 2 + GRAPH_WIDTH / 2, HEIGTH / 2 + pam(-y_displacement),
                 WHITE);

    // y axis
    if (-20 <= x_displacement && x_displacement <= 0)

        DrawLine(WIDTH / 2 + -pam(x_displacement),
                 HEIGTH / 2 - GRAPH_HEIGTH / 2,
                 WIDTH / 2 + -pam(x_displacement),
                 HEIGTH / 2 + GRAPH_HEIGTH / 2, WHITE);

    int offsetx = (WIDTH - GRAPH_WIDTH) / 2;
    int offsety = (HEIGTH - GRAPH_HEIGTH) / 2;

    int font_size = 20;

    char a[10];
    // linee per i punti sul grafico
    for (int i = -10; i <= 10; i++) {
        if (0 <= y_displacement && y_displacement <= 20)

            DrawLine(pam(i) + offsetx, HEIGTH / 2 - 10 + pam(-y_displacement),
                     pam(i) + offsetx, HEIGTH / 2 + 10 + pam(-y_displacement),
                     WHITE);
        if (-20 <= x_displacement && x_displacement <= 0)

            DrawLine(WIDTH / 2 - 10 - pam(x_displacement), pam(i) + offsety,
                     WIDTH / 2 + 10 - pam(x_displacement), pam(i) + offsety,
                     WHITE);

        if (i == 0 && x_displacement == -10 && y_displacement == 10) continue;
        // numeri
        itoa(i + x_displacement + 10, a, 1);
        int text_offset_x = (i < 0) ? 15 : 5;

        if (0 <= y_displacement && y_displacement <= 20)
            if (strcmp(a, "0") != 0 || x_displacement != -10)
                DrawText(a, pam(i) + offsetx - text_offset_x,
                         HEIGTH / 2 + 10 + pam(-y_displacement), font_size,
                         WHITE);

        // quando i = -10 pam ritorna 0(posY), ma in alto a tutto dobbiamo
        // mettere 10 (-i)

        // quando invece è 10 pam ritorna 1000(posY), ma in basso a
        // tutto dobbiamo mettere -10 (-i)
        itoa(-i - y_displacement + 10, a, 1);
        text_offset_x = (-i < 0) ? 30 : 25;
        if (-20 <= x_displacement && x_displacement <= 0)
            if (strcmp(a, "0") != 0 || y_displacement != 10)
                DrawText(a, WIDTH / 2 - text_offset_x - pam(x_displacement),
                         pam(i) + offsety - 10, font_size, WHITE);
    }
}

void draw_function(int index)
{
    char name[FUNCTION_NAME_LEN + 3 + 1];
    int offsetx = (WIDTH - GRAPH_WIDTH) / 2;
    int offsety = (HEIGTH - GRAPH_HEIGTH) / 2;

    for (double i = 0; i < 1000; i += 0.5) {
        double x0 = map(i + pam(x_displacement));
        double y = functions[index]->function(x0);

        // controllo per mantenere da -10, 10 a -10, 10 (formule ricavate
        // semplficando equazione) (membro a sinistra è la pos della y, a destra
        // margine inf e sup)
        if (GRAPH_HEIGTH - pam(y) + pam(-y_displacement) < 0 ||
            -pam(y) + pam(-y_displacement) > 0)
            continue;

        DrawCircle(i + offsetx,
                   GRAPH_HEIGTH - pam(y) + offsety + pam(-y_displacement), 2,
                   functions[index]->color);
    }
}

void draw_point(int index)
{
    point* p = points[index];

    int offsetx = (WIDTH - GRAPH_WIDTH) / 2;
    int offsety = (HEIGTH - GRAPH_HEIGTH) / 2;

    // Non mostra in punti fuori dal grafico
    if (pam(p->x) - pam(x_displacement) < 0 ||
        pam(p->x) - pam(x_displacement) > GRAPH_WIDTH)
        return;
    if (pam(p->y) + pam(-y_displacement) < 0 ||
        p->y + pam(-y_displacement) > GRAPH_HEIGTH)
        return;

    DrawCircle(pam(p->x) - pam(x_displacement) + offsetx,
               pam(p->y * -1) + offsety + pam(-y_displacement), 4, WHITE);
    DrawText(p->name, pam(p->x) - pam(x_displacement) + offsetx - 30,
             pam(p->y * -1) + offsety + pam(-y_displacement) - 30, 30, WHITE);
}

void draw_goto()
{
    DrawRectangle(XtextBox.x, XtextBox.y, XtextBox.width, XtextBox.height,
                  WHITE);
    DrawRectangle(YtextBox.x, YtextBox.y, YtextBox.width, YtextBox.height,
                  WHITE);
    DrawText("GOTO:", 100, HEIGTH - 200, 20, WHITE);
    DrawText("x:  ", 100, HEIGTH - 150, 20, WHITE);
    DrawText("y:  ", 100, HEIGTH - 100, 20, WHITE);
    DrawText(x_text, 140, HEIGTH - 150, 20, BLACK);
    DrawText(y_text, 140, HEIGTH - 100, 20, BLACK);
}

void engine_draw()
{
    SetTraceLogLevel(100);
    InitWindow(WIDTH, HEIGTH, window_title);
    SetTargetFPS(FPS);
    ToggleFullscreen();

    while (!WindowShouldClose()) {
        ClearBackground(BLACK_);

        char name[FUNCTION_NAME_LEN + 3 + 1];
        for (size_t i = 0; i < func_to_draw_index; i++) {
            // disegna i nomi delle funzioni anche se queste non vanno disegnate
            sprintf(name, "%s(x)", functions[i]->name);
            DrawText(name, 100, i * 0.8f * 100 + 300, 30, functions[i]->color);
            int index = func_to_draw[i];
            if (index != -1) draw_function(index);
        }
        for (size_t i = 0; i < points_to_draw_index; i++) {
            int index = points_to_draw[i];
            if (index != -1) draw_point(index);
        }

        DrawText("Press ESC to exit", 20, 20, 20, WHITE);

        if (IsKeyPressed(KEY_UP))
            y_displacement--;
        else if (IsKeyPressed(KEY_DOWN))
            y_displacement++;
        else if (IsKeyPressed(KEY_RIGHT))
            x_displacement++;
        else if (IsKeyPressed(KEY_LEFT))
            x_displacement--;

        draw_grid();
        draw_graph();

        xtext_ptr = handle_text_input(&XtextBox, x_text, xtext_ptr);
        ytext_ptr = handle_text_input(&YtextBox, y_text, ytext_ptr);

        handle_fn_name_click();

        draw_goto();
        EndDrawing();
    }
    CloseWindow();
}