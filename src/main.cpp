#include <psp2/kernel/processmgr.h>
#include <psp2/appmgr.h>
#include <psp2/display.h>

int main()
{
    sceAppMgrFinish(0);
    sceKernelExitProcess(0);
    return 0;
}
