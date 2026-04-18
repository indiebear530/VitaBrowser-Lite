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

static char url[256] = "http://example.com";
static char page[200000];

static int scroll_y = 0;
static int dragging = 0;
static int last_y = 0;

/* -------- HTTP -------- */
char* http_get(const char *u) {
    memset(page, 0, sizeof(page));

    SceUID tpl = sceHttpCreateTemplate("vita", 1, 1);
    SceUID conn = sceHttpCreateConnectionWithURL(tpl, u, 0);
    SceUID req  = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_GET, u, 0);

    sceHttpSendRequest(req, NULL, 0);
    sceHttpReadData(req, page, sizeof(page)-1);

    sceHttpDeleteRequest(req);
    sceHttpDeleteConnection(conn);
    sceHttpDeleteTemplate(tpl);

    return page;
}

/* -------- Keyboard -------- */
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
        http_get(url);
    }

    sceImeDialogTerm();
}

/* -------- Touch -------- */
void handle_touch() {
    SceTouchData touch;
    sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);

    if (touch.reportNum > 0) {
        int y = touch.report[0].y / 2;

        if (y < 60) {
            open_keyboard();
        } else {
            if (!dragging) {
                dragging = 1;
                last_y = y;
            } else {
                scroll_y += (y - last_y);
                last_y = y;
            }
        }
    } else {
        dragging = 0;
    }
}

/* -------- MAIN -------- */
int main() {
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);

    vita2d_init();
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

        handle_touch();

        vita2d_start_drawing();
        vita2d_clear_screen();

        /* URL BAR */
        vita2d_draw_rectangle(0, 0, 960, 60, RGBA8(20,20,20,255));
        vita2d_draw_string(10, 20, RGBA8(255,255,0,255), url);

        /* PAGE */
        int y = 80 + scroll_y;

        for (int i = 0; page[i]; i++) {
            if (page[i] == '<') {
                while (page[i] && page[i] != '>') i++;
            } else {
                vita2d_draw_char(10, y, RGBA8(255,255,255,255), page[i]);
                y += 10;
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
