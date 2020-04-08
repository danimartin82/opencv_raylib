#include "utils.h"

FILETIME prevSysIdle, prevSysKernel, prevSysUser;
int counter;
double accumulated_val=0;
int number_of_measures = 0;

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: getCPULoad()                                                               */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

double getCPULoad()
{
    double instant_val;
    FILETIME sysIdle, sysKernel, sysUser;
    
    if ((counter % 2)==0)
    {
        if (GetSystemTimes(&sysIdle, &sysKernel, &sysUser) == 0) // GetSystemTimes func FAILED return value is zero;
            return 0;

        if (prevSysIdle.dwLowDateTime != 0 && prevSysIdle.dwHighDateTime != 0)
        {
            ULONGLONG sysIdleDiff, sysKernelDiff, sysUserDiff;
            sysIdleDiff = SubtractTimes(sysIdle, prevSysIdle);
            sysKernelDiff = SubtractTimes(sysKernel, prevSysKernel);
            sysUserDiff = SubtractTimes(sysUser, prevSysUser);

            ULONGLONG sysTotal = sysKernelDiff + sysUserDiff;
            ULONGLONG kernelTotal = sysKernelDiff - sysIdleDiff; // kernelTime - IdleTime = kernelTime, because sysKernel include IdleTime

            if (sysTotal > 0) // sometimes kernelTime > idleTime
            {
                instant_val = (double)(((kernelTotal + sysUserDiff) * 100.0) / sysTotal);
                number_of_measures++;
                accumulated_val = accumulated_val + instant_val;
            }
        }

        prevSysIdle = sysIdle;
        prevSysKernel = sysKernel;
        prevSysUser = sysUser;
    }
    counter++;
    
    return accumulated_val/number_of_measures;
}


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: SubtractTimes()                                                            */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
ULONGLONG SubtractTimes(const FILETIME one, const FILETIME two)
{
    LARGE_INTEGER a, b;
    a.LowPart = one.dwLowDateTime;
    a.HighPart = one.dwHighDateTime;

    b.LowPart = two.dwLowDateTime;
    b.HighPart = two.dwHighDateTime;

    return a.QuadPart - b.QuadPart;
}