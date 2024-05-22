#include "fw_debug.h"


#if (MODULE_TEST == ON)

#if 0
__weak void Test(void){}
#endif


int main(void)
{
    Test();
}

#else

__weak void Task_Entry(void){}


int main(void)
{
    Task_Entry();
    return 0;
}

#endif

