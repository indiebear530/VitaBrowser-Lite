diff --git a/src/main.cpp b/src/main.cpp
index e2538acce150cffd9a1f1253a5489aae8396f603..b93f49468016fd03ad0ba0f9547b6a2f24f9e2e4 100644
--- a/src/main.cpp
+++ b/src/main.cpp
@@ -13,29 +13,27 @@ int main()
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
     return 0;
 }
