#include <psp2/appmgr.h>
#include <psp2/kernel/processmgr.h>

int main()
{
    sceAppMgrFinish(0);
    sceKernelExitProcess(0);
    return 0;
}
