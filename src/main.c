#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/ime_dialog.h>
#include <psp2/touch.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/net/http.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>

#include <vita2d.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PAGE 200000
#define MAX_HISTORY 20

static char url[256] = "http://example.com";
static char page[MAX_PAGE];

static char history[MAX_HISTORY][256];
static int history_index = -1;
static int history_top = -1;

static int scroll_y = 0;

/* ---------------- HTTP ---------------- */
char* http_get(const char *u) {
    memset(page, 0, sizeof(page));

    SceUID tpl = sceHttpCreateTemplate("vita", 1, 1);
    SceUID conn = sceHttpCreateConnectionWithURL(tpl, u, 0);
    SceUID req  = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_GET, u, 0);

    sceHttpSendRequest(req, NULL, 0);
    sceHttpReadData(req, page, sizeof(page) - 1);

    sceHttpDeleteRequest(req);
    sceHttpDeleteConnection(conn);
    sceHttpDeleteTemplate(tpl);

    return page;
}

/* ---------------- HISTORY ---------------- */
void push_history(const char *u) {
    if (history_index < MAX_HISTORY - 1) {
        history_index++;
        strcpy(history[history_index], u);
        history_top = history_index;
    }
}

void go_back() {
    if (history_index > 0) {
        history_index--;
        strcpy(url, history[history_index]);
        http_get(url);
    }
}

void go_forward() {
    if (history_index < history_top) {
        history_index++;
        strcpy(url, history[history_index]);
        http_get(url);
    }
}

/* ---------------- KEYBOARD ---------------- */
void open_keyboard() {
    SceImeDialogParam param;
    sceImeDialogParamInit(&param);

    param.type = SCE_IME_TYPE_URL;
    param.title = "Enter URL";
    param.maxTextLength = 256;

    strcpy(param.inputTextBuffer, url);

    sceImeDialogInit(&param);

    while (sceImeDialogGetStatus() != SCE_COMMON_DIALOG_STATUS_FINISHED) {
        sceKernelDelayThread(10000);
    }

    SceImeDialogResult result;
    sceImeDialogGetResult(&result);

    if (result.button == SCE_IME_DIALOG_BUTTON_ENTER) {
        strcpy(url, param.inputTextBuffer);
        push_history(url);
        http_get(url);
    }

    sceImeDialogTerm();
}

/* ---------------- TOUCH ---------------- */
void handle_touch() {
    SceTouchData touch;
    sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);

    if (touch.reportNum > 0) {
        int y = touch.report[0].y / 2;

        if (y < 60) {
            open_keyboard();
        } else {
            scroll_y += (y - 240);
        }
    }
}

/* ---------------- MAIN ---------------- */
int main() {
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);

    vita2d_init();

    push_history(url);
    http_get(url);

    while (1) {
        SceCtrlData pad;
        sceCtrlPeekBufferPositive(0, &pad, 1);

        if (pad.buttons & SCE_CTRL_START)
            break;

        if (pad.buttons & SCE_CTRL_TRIANGLE)
            open_keyboard();

        if (pad.buttons & SCE_CTRL_CIRCLE)
            http_get(url);

        if (pad.buttons & SCE_CTRL_LTRIGGER)
            go_back();

        if (pad.buttons & SCE_CTRL_RTRIGGER)
            go_forward();

        handle_touch();

        vita2d_start_drawing();
        vita2d_clear_screen();

        /* URL BAR */
        vita2d_draw_rectangle(0, 0, 960, 60, RGBA8(25,25,25,255));
        vita2d_draw_string(10, 20, RGBA8(255,255,0,255), url);

        /* PAGE RENDER */
        int y = 80 + scroll_y;
        int x = 10;

        for (int i = 0; page[i]; i++) {

            if (page[i] == '<') {
                while (page[i] && page[i] != '>') i++;
                continue;
            }

            vita2d_draw_char(x, y, RGBA8(255,255,255,255), page[i]);
            x += 8;

            if (x > 920) {
                x = 10;
                y += 18;
            }
        }

        vita2d_end_drawing();
        vita2d_swap_buffers();
        sceDisplayWaitVblankStart();
    }

    vita2d_fini();
    sceKernelExitProcess(0);
    return 0;
}
