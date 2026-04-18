#include <psp2/common_dialog.h>
#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/ime_dialog.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/net/http.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/touch.h>

#include <psp2/debugScreen.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define URL_MAX 256
#define PAGE_SIZE 100000
#define HTTP_BUFFER_SIZE (1 * 1024 * 1024)
#define NET_MEM_SIZE (1 * 1024 * 1024)
#define MAX_VISIBLE_LINES 28
#define MAX_LINE_CHARS 66

static char url[URL_MAX] = "http://example.com";
static char page[PAGE_SIZE];
static int scroll_line = 0;
static bool touch_was_active = false;

static unsigned char net_memory[NET_MEM_SIZE];
static unsigned char http_memory[HTTP_BUFFER_SIZE];

static void clamp_scroll_line(int max_scroll) {
    if (scroll_line < 0) {
        scroll_line = 0;
    } else if (scroll_line > max_scroll) {
        scroll_line = max_scroll;
    }
}

static void ascii_to_utf16(const char *src, uint16_t *dst, size_t dst_len) {
    size_t i = 0;
    while (i + 1 < dst_len && src[i] != '\0') {
        dst[i] = (uint16_t)(unsigned char)src[i];
        i++;
    }
    dst[i] = 0;
}

static void utf16_to_ascii(const uint16_t *src, char *dst, size_t dst_len) {
    size_t i = 0;
    while (i + 1 < dst_len && src[i] != 0) {
        uint16_t ch = src[i];
        dst[i] = (ch <= 0x7F) ? (char)ch : '?';
        i++;
    }
    dst[i] = '\0';
}

static int net_http_init(void) {
    int ret = sceNetInit(net_memory, NET_MEM_SIZE, 0, 0, 0);
    if (ret < 0) {
        return ret;
    }

    ret = sceNetCtlInit();
    if (ret < 0) {
        sceNetTerm();
        return ret;
    }

    ret = sceHttpInit(http_memory, HTTP_BUFFER_SIZE);
    if (ret < 0) {
        sceNetCtlTerm();
        sceNetTerm();
        return ret;
    }

    return 0;
}

static void net_http_term(void) {
    sceHttpTerm();
    sceNetCtlTerm();
    sceNetTerm();
}

static int http_get(const char *target_url) {
    memset(page, 0, sizeof(page));

    SceUID tpl = sceHttpCreateTemplate("vita-browser", 1, 1);
    if (tpl < 0) {
        return tpl;
    }

    SceUID conn = sceHttpCreateConnectionWithURL(tpl, target_url, 0);
    if (conn < 0) {
        sceHttpDeleteTemplate(tpl);
        return conn;
    }

    SceUID req = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_GET, target_url, 0);
    if (req < 0) {
        sceHttpDeleteConnection(conn);
        sceHttpDeleteTemplate(tpl);
        return req;
    }

    int ret = sceHttpSendRequest(req, NULL, 0);
    if (ret >= 0) {
        int total = 0;
        while (total < (PAGE_SIZE - 1)) {
            int read_bytes = sceHttpReadData(req, page + total, PAGE_SIZE - 1 - total);
            if (read_bytes <= 0) {
                break;
            }
            total += read_bytes;
        }
        page[total] = '\0';
    }

    sceHttpDeleteRequest(req);
    sceHttpDeleteConnection(conn);
    sceHttpDeleteTemplate(tpl);

    scroll_line = 0;
    return ret;
}

static void open_keyboard(void) {
    uint16_t input_buffer[URL_MAX];
    ascii_to_utf16(url, input_buffer, URL_MAX);

    SceImeDialogParam param;
    sceImeDialogParamInit(&param);

    param.supportedLanguages = 0x0001FFFF;
    param.languagesForced = SCE_TRUE;
    static uint16_t title[] = {'E', 'n', 't', 'e', 'r', ' ', 'U', 'R', 'L', 0};
    param.type = SCE_IME_TYPE_URL;
    param.title = title;
    param.maxTextLength = URL_MAX - 1;
    param.initialText = input_buffer;
    param.inputTextBuffer = input_buffer;

    if (sceImeDialogInit(&param) < 0) {
        return;
    }

    while (1) {
        int status = sceImeDialogGetStatus();
        if (status == SCE_COMMON_DIALOG_STATUS_FINISHED) {
            break;
        }
        sceKernelDelayThread(10000);
    }

    SceImeDialogResult result;
    if (sceImeDialogGetResult(&result) == 0 && result.button == SCE_IME_DIALOG_BUTTON_ENTER) {
        utf16_to_ascii(input_buffer, url, URL_MAX);
        http_get(url);
    }

    sceImeDialogTerm();
}

static void handle_touch(void) {
    SceTouchData t;
    memset(&t, 0, sizeof(t));

    if (sceTouchPeek(SCE_TOUCH_PORT_FRONT, &t, 1) < 0) {
        return;
    }

    bool touch_active = t.reportNum > 0;
    if (touch_active && !touch_was_active) {
        int y = t.report[0].y / 2;

        if (y < 60) {
            open_keyboard();
        } else if (y > 280) {
            scroll_line++;
        } else if (scroll_line > 0) {
            scroll_line--;
        }
    }

    touch_was_active = touch_active;
}

static int compute_total_lines(void) {
    int logical_line = 0;
    int col = 0;
    bool in_tag = false;

    for (int i = 0; page[i] != '\0'; i++) {
        char ch = page[i];

        if (ch == '<') {
            in_tag = true;
            continue;
        }

        if (in_tag) {
            if (ch == '>') {
                in_tag = false;
            }
            continue;
        }

        if (ch == '\r') {
            continue;
        }

        if (ch == '\n' || col >= MAX_LINE_CHARS) {
            logical_line++;
            col = 0;
            if (ch == '\n') {
                continue;
            }
        }
        col++;
    }

    return logical_line;
}

static void draw_wrapped_text(void) {
    int printable_line = 0;
    int logical_line = 0;
    int col = 0;
    bool in_tag = false;

    pspDebugScreenSetXY(0, 2);

    for (int i = 0; page[i] != '\0'; i++) {
        char ch = page[i];

        if (ch == '<') {
            in_tag = true;
            continue;
        }

        if (in_tag) {
            if (ch == '>') {
                in_tag = false;
            }
            continue;
        }

        if (ch == '\r') {
            continue;
        }

        if (ch == '\n' || col >= MAX_LINE_CHARS) {
            if (logical_line >= scroll_line && printable_line < MAX_VISIBLE_LINES) {
                pspDebugScreenPutChar('\n');
            }
            logical_line++;
            printable_line = logical_line - scroll_line;
            col = 0;

            if (ch == '\n') {
                continue;
            }
        }

        if (logical_line >= scroll_line && printable_line < MAX_VISIBLE_LINES) {
            pspDebugScreenPutChar(ch);
        }
        col++;
    }
}

int main(void) {
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);

    pspDebugScreenInit();

    if (net_http_init() < 0) {
        pspDebugScreenPrintf("Network/HTTP init failed.\n");
        sceKernelDelayThread(3 * 1000 * 1000);
        sceKernelExitProcess(1);
    }

    http_get(url);

    while (1) {
        SceCtrlData pad;
        memset(&pad, 0, sizeof(pad));
        sceCtrlPeekBufferPositive(0, &pad, 1);

        if (pad.buttons & SCE_CTRL_START) {
            break;
        }
        if (pad.buttons & SCE_CTRL_TRIANGLE) {
            open_keyboard();
        }
        if (pad.buttons & SCE_CTRL_CIRCLE) {
            http_get(url);
        }
        if ((pad.buttons & SCE_CTRL_DOWN) != 0) {
            scroll_line++;
        }
        if ((pad.buttons & SCE_CTRL_UP) != 0 && scroll_line > 0) {
            scroll_line--;
        }

        handle_touch();
        {
            int total_lines = compute_total_lines();
            int max_scroll = total_lines - MAX_VISIBLE_LINES;
            if (max_scroll < 0) {
                max_scroll = 0;
            }
            clamp_scroll_line(max_scroll);
        }

        pspDebugScreenClear(COLOR_BLACK);
        pspDebugScreenSetTextColor(COLOR_YELLOW);
        pspDebugScreenSetXY(0, 0);
        pspDebugScreenPrintf("URL: %s", url);
        pspDebugScreenSetTextColor(COLOR_WHITE);
        draw_wrapped_text();

        sceDisplayWaitVblankStart();
    }

    net_http_term();
    sceKernelExitProcess(0);
    return 0;
}
