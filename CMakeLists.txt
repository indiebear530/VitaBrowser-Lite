target_link_libraries(${PROJECT_NAME}
    SceAppMgr_stub
    SceProcessmgr_stub
    SceDisplay_stub          # already had, but needed
    SceCtrl_stub
    SceNet_stub
    SceNetCtl_stub
    SceHttp_stub
    SceSysmem_stub
    SceGxm_stub              # ← super important for vita2d graphics
    SceCommonDialog_stub
    SceFont_stub             # ← for pgf font stuff
    SceSharedFb_stub         # ← for shared framebuffer
    vita2d
)
