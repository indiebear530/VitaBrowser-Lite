#include <psp2/kernel/processmgr.h>
#include <psp2/appmgr.h>
#include <psp2/ctrl.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/http.h>
#include <psp2/display.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_POOL_SIZE (4 * 1024 * 1024)

int main()
{
    int ret;

    // Init network
    sceNetInit();
    sceNetCtlInit();

    // Wait for Wi-Fi connection (you may need to connect manually first)
    int state;
    do {
        sceNetCtlGetState(&state);
        sceKernelDelayThread(100 * 1000);
    } while (state != SCE_NETCTL_STATE_CONNECTED);

    // Init HTTP
    SceHttpInitParam httpInitParam;
    memset(&httpInitParam, 0, sizeof(SceHttpInitParam));
    httpInitParam.httpPoolSize = MEMORY_POOL_SIZE;
    ret = sceHttpInit(&httpInitParam);
    if (ret < 0) {
        // error handling
    }

    // Create template + connection + request (very basic GET example)
    int tpl = sceHttpCreateTemplate("VitaBrowserLite/1.0", 1, 1);
    int conn = sceHttpCreateConnectionWithURL(tpl, "http://example.com", 0);
    int req = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_GET, "http://example.com", 0);

    sceHttpSendRequest(req, NULL, 0);

    // TODO: Read response with sceHttpReadData() in a loop...

    // For now just loop until START is pressed
    while (1) {
        SceCtrlData pad;
        sceCtrlReadBufferPositive(0, &pad, 1);
        if (pad.buttons & SCE_CTRL_START) break;

        sceDisplayWaitVblankStart();
    }

    sceHttpTerm();
    sceNetCtlTerm();
    sceNetTerm();

    sceAppMgrFinish(0);
    sceKernelExitProcess(0);
    return 0;
}
