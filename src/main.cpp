#include <vita2d.h>
#include <psp2/ctrl.h>
#include <psp2/display.h>      // ← this is the missing one for sceDisplayWaitVblankStart
#include <psp2/kernel/processmgr.h>

vita2d_pgf *pgf;

int main()
{
    vita2d_init();
    pgf = vita2d_load_default_pgf();

    SceCtrlData pad;

    while (1) {
        sceCtrlPeekBufferPositive(0, &pad, 1);

        if (pad.buttons & SCE_CTRL_START) {
            break;
        }

        vita2d_start_drawing();
        vita2d_clear_screen();

        vita2d_pgf_draw_text(pgf, 120, 120, RGBA8(255, 255, 255, 255), 1.2f, "VitaBrowser Lite");
        vita2d_pgf_draw_text(pgf, 120, 180, RGBA8(180, 220, 255, 255), 0.9f, "Press START to exit");

        vita2d_pgf_draw_text(pgf, 120, 280, RGBA8(100, 255, 100, 255), 0.8f, "Basic graphics test running...");

        vita2d_end_drawing();
        vita2d_swap_buffers();
        sceDisplayWaitVblankStart();
    }

    vita2d_free_pgf(pgf);
    vita2d_fini();
    sceKernelExitProcess(0);
    return 0;
}
