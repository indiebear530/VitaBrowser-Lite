#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <psp2/display.h>

#include <vita2d.h>

#include <stdio.h>

int main()
{
    // Initialize graphics
    vita2d_init();
    vita2d_set_clear_color(RGBA8(0, 0, 0, 255));  // Black background

    // Load default font (built into vita2d)
    vita2d_pgf *pgf = vita2d_load_default_pgf();

    SceCtrlData pad;

    while (1) {
        sceCtrlPeekBufferPositive(0, &pad, 1);

        // Exit on START
        if (pad.buttons & SCE_CTRL_START) {
            break;
        }

        vita2d_start_drawing();
        vita2d_clear_screen();

        // Draw title text
        vita2d_pgf_draw_text(pgf, 100, 100, RGBA8(255, 255, 255, 255), 1.0f, "VitaBrowser Lite");
        vita2d_pgf_draw_text(pgf, 100, 150, RGBA8(200, 200, 255, 255), 0.8f, "Press START to exit");

        // Simple status line
        vita2d_pgf_draw_text(pgf, 100, 250, RGBA8(100, 255, 100, 255), 0.7f, "Ready for networking...");

        vita2d_end_drawing();
        vita2d_swap_buffers();
        sceDisplayWaitVblankStart();
    }

    // Cleanup
    vita2d_free_pgf(pgf);
    vita2d_fini();

    sceAppMgrFinish(0);
    sceKernelExitProcess(0);
    return 0;
}
