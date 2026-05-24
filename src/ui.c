#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "bst.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define LEFT_PANEL_WIDTH 320
#define STUDENT_LIST_MAX 256
#define STATUS_MAX 256
#define FONT_PATH "assets/nanum.ttf"
#define FONT_SIZE 20
#define HANGUL_START 0xAC00
#define HANGUL_END 0xD7A3
#define HANGUL_COUNT ((HANGUL_END - HANGUL_START) + 1)
#define ASCII_START 32
#define ASCII_END 126
#define ASCII_COUNT ((ASCII_END - ASCII_START) + 1)
#define FONT_CODEPOINT_COUNT (ASCII_COUNT + HANGUL_COUNT)
#define LEFT_PANEL_CONTENT_TOP_OFFSET 52

typedef struct {
    Student students[STUDENT_LIST_MAX];
    size_t count;
} StudentList;

typedef struct {
    const char *target_name;
    const Student *found;
} NameSearchContext;

static Font g_font;
static bool g_fontLoaded;

static bool load_korean_font(Font *font, int font_size)
{
    int *codepoints;
    int index;
    int cp;

    if (font == NULL) {
        return false;
    }

    if (!FileExists(FONT_PATH)) {
        return false;
    }

    codepoints = (int *)malloc((size_t)FONT_CODEPOINT_COUNT * sizeof(int));
    if (codepoints == NULL) {
        return false;
    }

    index = 0;
    for (cp = ASCII_START; cp <= ASCII_END; cp++) {
        codepoints[index++] = cp;
    }
    for (cp = HANGUL_START; cp <= HANGUL_END; cp++) {
        codepoints[index++] = cp;
    }

    *font = LoadFontEx(FONT_PATH, font_size, codepoints, index);
    free(codepoints);

    return IsFontValid(*font);
}

static void collect_student(const Student *student, void *user_data)
{
    StudentList *list;

    if (student == NULL || user_data == NULL) {
        return;
    }

    list = (StudentList *)user_data;
    if (list->count >= STUDENT_LIST_MAX) {
        return;
    }

    list->students[list->count++] = *student;
}

static void refresh_student_list(const Bst *tree, StudentList *list)
{
    if (list == NULL) {
        return;
    }

    list->count = 0;
    bst_inorder(tree, collect_student, list);
}

static bool parse_int(const char *text, int *value)
{
    char *end;

    if (text == NULL || value == NULL || text[0] == '\0') {
        return false;
    }

    *value = (int)strtol(text, &end, 10);
    return *end == '\0';
}

static void set_status(char *status, size_t status_size, const char *message)
{
    if (status == NULL || status_size == 0) {
        return;
    }

    if (message == NULL) {
        status[0] = '\0';
        return;
    }

    snprintf(status, status_size, "%s", message);
}

static void draw_text(const char *text, int x, int y, int font_size, Color color)
{
    if (text == NULL) {
        return;
    }

    if (g_fontLoaded) {
        DrawTextEx(g_font, text, (Vector2){ (float)x, (float)y }, (float)font_size, 1.0f, color);
    } else {
        DrawText(text, x, y, font_size, color);
    }
}

static void handle_add(Bst *tree, const char *id_text, const char *name_text,
                       const char *grade_text, char *status, size_t status_size,
                       StudentList *list)
{
    Student student;
    int id;
    int grade;

    if (!parse_int(id_text, &id) || id <= 0) {
        set_status(status, status_size, u8"\uCD94\uAC00 \uC2E4\uD328: \uC62C\uBC14\uB978 \uD559\uBC88\uC744 \uC785\uB825\uD558\uC138\uC694.");
        return;
    }

    if (name_text == NULL || name_text[0] == '\0') {
        set_status(status, status_size, u8"\uCD94\uAC00 \uC2E4\uD328: \uC774\uB984\uC744 \uC785\uB825\uD558\uC138\uC694.");
        return;
    }

    if (!parse_int(grade_text, &grade) || grade < 0 || grade > 100) {
        set_status(status, status_size, u8"\uCD94\uAC00 \uC2E4\uD328: \uC131\uC801\uC740 0~100 \uC0AC\uC774\uC5EC\uC57C \uD569\uB2C8\uB2E4.");
        return;
    }

    student.id = id;
    student.grade = grade;
    strncpy(student.name, name_text, BST_NAME_MAX - 1);
    student.name[BST_NAME_MAX - 1] = '\0';

    if (bst_insert(tree, &student) == NULL) {
        set_status(status, status_size, u8"\uCD94\uAC00 \uC2E4\uD328: \uC911\uBCF5 \uD559\uBC88\uC774\uAC70\uB098 \uBA54\uBAA8\uB9AC \uBD80\uC871\uC785\uB2C8\uB2E4.");
        return;
    }

    refresh_student_list(tree, list);
    set_status(status, status_size, u8"\uCD94\uAC00 \uC644\uB8CC: \uD559\uC0DD \uC815\uBCF4\uAC00 \uB4F1\uB85D\uB418\uC5C8\uC2B5\uB2C8\uB2E4.");
}

static bool is_blank(const char *text)
{
    return text == NULL || text[0] == '\0';
}

static void search_by_name_visitor(const Student *student, void *user_data)
{
    NameSearchContext *ctx;

    if (student == NULL || user_data == NULL) {
        return;
    }

    ctx = (NameSearchContext *)user_data;
    if (ctx->found == NULL && strcmp(student->name, ctx->target_name) == 0) {
        ctx->found = student;
    }
}

static void handle_search(Bst *tree, const char *id_text, const char *name_text,
                          char *status, size_t status_size)
{
    BstNode *node;
    int id;
    char message[STATUS_MAX];
    const Student *found_by_name;
    NameSearchContext name_ctx;

    if (is_blank(id_text) && is_blank(name_text)) {
        set_status(status, status_size,
                   u8"[\uC624\uB958] \uAC80\uC0C9\uD560 \uD559\uBC88 \uB610\uB294 \uC774\uB984\uC744 \uC785\uB825\uD558\uC138\uC694.");
        return;
    }

    if (!is_blank(id_text)) {
        if (!parse_int(id_text, &id) || id <= 0) {
            set_status(status, status_size,
                       u8"\uAC80\uC0C9 \uC2E4\uD328: \uC62C\uBC14\uB978 \uD559\uBC88\uC744 \uC785\uB825\uD558\uC138\uC694.");
            return;
        }

        node = bst_search(tree, id);
        if (node == NULL) {
            set_status(status, status_size,
                       u8"\uAC80\uC0C9 \uC2E4\uD328: \uD574\uB2F9 \uD559\uBC88\uC758 \uD559\uC0DD\uC774 \uC5C6\uC2B5\uB2C8\uB2E4.");
            return;
        }

        snprintf(message, sizeof(message), u8"\uAC80\uC0C9 \uC131\uACF5: %d | %s | %d\uC810",
                 node->data.id, node->data.name, node->data.grade);
        set_status(status, status_size, message);
        return;
    }

    name_ctx.target_name = name_text;
    name_ctx.found = NULL;
    bst_inorder(tree, search_by_name_visitor, &name_ctx);
    found_by_name = name_ctx.found;

    if (found_by_name == NULL) {
        set_status(status, status_size,
                   u8"\uAC80\uC0C9 \uC2E4\uD328: \uD574\uB2F9 \uC774\uB984\uC758 \uD559\uC0DD\uC774 \uC5C6\uC2B5\uB2C8\uB2E4.");
        return;
    }

    snprintf(message, sizeof(message), u8"\uAC80\uC0C9 \uC131\uACF5: %d | %s | %d\uC810",
             found_by_name->id, found_by_name->name, found_by_name->grade);
    set_status(status, status_size, message);
}

static void handle_delete(Bst *tree, const char *id_text, char *status, size_t status_size,
                          StudentList *list)
{
    int id;

    if (!parse_int(id_text, &id) || id <= 0) {
        set_status(status, status_size, u8"\uC0AD\uC81C \uC2E4\uD328: \uC62C\uBC14\uB978 \uD559\uBC88\uC744 \uC785\uB825\uD558\uC138\uC694.");
        return;
    }

    if (!bst_delete(tree, id)) {
        set_status(status, status_size, u8"\uC0AD\uC81C \uC2E4\uD328: \uD574\uB2F9 \uD559\uBC88\uC758 \uD559\uC0DD\uC774 \uC5C6\uC2B5\uB2C8\uB2E4.");
        return;
    }

    refresh_student_list(tree, list);
    set_status(status, status_size, u8"\uC0AD\uC81C \uC644\uB8CC: \uD559\uC0DD \uC815\uBCF4\uAC00 \uC81C\uAC70\uB418\uC5C8\uC2B5\uB2C8\uB2E4.");
}

static void draw_student_list(Rectangle bounds, const StudentList *list, int scroll_y,
                              int line_height)
{
    int y;
    size_t i;
    char line[128];
    Rectangle content;

    DrawRectangleRec(bounds, (Color){ 245, 245, 245, 255 });
    DrawRectangleLinesEx(bounds, 1, GRAY);

    draw_text(u8"\uD559\uBC88 \uC624\uB984\uCC28\uC21C (In-order)", (int)bounds.x + 12,
              (int)bounds.y + 8, 18, DARKGRAY);

    content = bounds;
    content.y += 36;
    content.height -= 44;

    BeginScissorMode((int)content.x, (int)content.y, (int)content.width, (int)content.height);

    if (list == NULL || list->count == 0) {
        draw_text(u8"\uB4F1\uB85D\uB41C \uD559\uC0DD\uC774 \uC5C6\uC2B5\uB2C8\uB2E4.",
                  (int)content.x + 12, (int)content.y + 8, 18, GRAY);
    } else {
        y = (int)content.y + 8 - scroll_y;
        for (i = 0; i < list->count; i++) {
            snprintf(line, sizeof(line), u8"%04zu. %d  |  %s  |  %d\uC810",
                     i + 1, list->students[i].id, list->students[i].name, list->students[i].grade);
            draw_text(line, (int)content.x + 12, y, 18, BLACK);
            y += line_height;
        }
    }

    EndScissorMode();
}

void runUI(void)
{
    Bst tree;
    StudentList list;
    char id_text[32] = { 0 };
    char name_text[BST_NAME_MAX] = { 0 };
    char grade_text[32] = { 0 };
    char status[STATUS_MAX] = u8"\uC900\uBE44\uB428";
    int list_scroll = 0;
    const int line_height = 28;
    bool id_edit = false;
    bool name_edit = false;
    bool grade_edit = false;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BST Student Grade Manager");
    SetTargetFPS(60);

    bst_init(&tree);
    refresh_student_list(&tree, &list);

    g_fontLoaded = load_korean_font(&g_font, FONT_SIZE);
    if (g_fontLoaded) {
        GuiSetFont(g_font);
    }

    while (!WindowShouldClose()) {
        Rectangle left_panel = { 16, 16, LEFT_PANEL_WIDTH, SCREEN_HEIGHT - 32 };
        Rectangle right_panel = { left_panel.x + left_panel.width + 16, 16,
                                  SCREEN_WIDTH - left_panel.width - 48, SCREEN_HEIGHT - 32 };
        Rectangle list_area = { right_panel.x + 8, right_panel.y + 40,
                                right_panel.width - 16, right_panel.height - 48 };
        float y = left_panel.y + LEFT_PANEL_CONTENT_TOP_OFFSET;
        float field_width = left_panel.width - 24;
        float label_width = 70.0f;
        int content_height;
        int max_scroll;

        if (GetMouseWheelMove() != 0.0f) {
            Vector2 mouse = GetMousePosition();
            if (CheckCollisionPointRec(mouse, list_area)) {
                list_scroll -= (int)(GetMouseWheelMove() * line_height);
                if (list_scroll < 0) {
                    list_scroll = 0;
                }
            }
        }

        content_height = (int)list.count * line_height + 16;
        max_scroll = content_height - ((int)list_area.height - 44);
        if (max_scroll < 0) {
            max_scroll = 0;
        }
        if (list_scroll > max_scroll) {
            list_scroll = max_scroll;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        GuiPanel(left_panel, "Control Panel");
        draw_text(u8"\uD559\uBC88", (int)(left_panel.x + 12), (int)y, 18, DARKGRAY);
        if (GuiTextBox((Rectangle){ left_panel.x + 12 + label_width, y, field_width - label_width, 28 },
                       id_text, (int)sizeof(id_text), id_edit)) {
            id_edit = true;
            name_edit = false;
            grade_edit = false;
        }
        y += 40;

        draw_text(u8"\uC774\uB984", (int)(left_panel.x + 12), (int)y, 18, DARKGRAY);
        if (GuiTextBox((Rectangle){ left_panel.x + 12 + label_width, y, field_width - label_width, 28 },
                       name_text, (int)sizeof(name_text), name_edit)) {
            name_edit = true;
            id_edit = false;
            grade_edit = false;
        }
        y += 40;

        draw_text(u8"\uC131\uC801", (int)(left_panel.x + 12), (int)y, 18, DARKGRAY);
        if (GuiTextBox((Rectangle){ left_panel.x + 12 + label_width, y, field_width - label_width, 28 },
                       grade_text, (int)sizeof(grade_text), grade_edit)) {
            grade_edit = true;
            id_edit = false;
            name_edit = false;
        }
        y += 48;

        if (GuiButton((Rectangle){ left_panel.x + 12, y, field_width, 32 },
                      u8"\uD559\uC0DD \uCD94\uAC00")) {
            handle_add(&tree, id_text, name_text, grade_text, status, sizeof(status), &list);
            id_edit = false;
            name_edit = false;
            grade_edit = false;
        }
        y += 40;

        if (GuiButton((Rectangle){ left_panel.x + 12, y, field_width, 32 },
                      u8"\uD559\uC0DD \uAC80\uC0C9")) {
            handle_search(&tree, id_text, name_text, status, sizeof(status));
            id_edit = false;
            name_edit = false;
            grade_edit = false;
        }
        y += 40;

        if (GuiButton((Rectangle){ left_panel.x + 12, y, field_width, 32 },
                      u8"\uD559\uC0DD \uC0AD\uC81C")) {
            handle_delete(&tree, id_text, status, sizeof(status), &list);
            id_edit = false;
            name_edit = false;
            grade_edit = false;
        }

        GuiPanel((Rectangle){ left_panel.x + 12, left_panel.y + left_panel.height - 96,
                             field_width, 72 }, "Status");
        draw_text(status, (int)(left_panel.x + 20), (int)(left_panel.y + left_panel.height - 72),
                  16, DARKBLUE);

        GuiPanel(right_panel, "Student List (In-order)");
        draw_student_list(list_area, &list, list_scroll, line_height);

        EndDrawing();
    }

    if (g_fontLoaded) {
        UnloadFont(g_font);
    }

    bst_free(&tree);
    CloseWindow();
}
