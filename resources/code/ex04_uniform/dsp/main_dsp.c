/*
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== main_dsp.c ========
 *
 */

/* xdctools header files */
#include <xdc/std.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/System.h>

/* package header files */
#include <ti/ipc/Ipc.h>
#include <ti/ipc/MultiProc.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* local header files */
#include "Server.h"

/* private functions */
static Void smain(UArg arg0, UArg arg1);

/*
*  ======== main ========
*/
Int main(Int argc, Char* argv[])
{
    Error_Block eb;
    Task_Params taskParams;

    Log_print0(Diags_ENTRY, "--> main:");

    /* must initialize the error block before using it */
    Error_init(&eb);

    /* create main thread (interrupts not enabled in main on BIOS) */
    Task_Params_init(&taskParams);
    taskParams.instance->name = "smain";
    taskParams.stackSize = 0x1000;
    Task_create(smain, &taskParams, &eb);

    if (Error_check(&eb)) {
        System_abort("main: failed to create application startup thread");
    }

    /* start scheduler, this never returns */
    BIOS_start();

    /* should never get here */
    Log_print0(Diags_EXIT, "<-- main:");
    return (0);
}

/*
*  ======== smain ========
*/
Void smain(UArg arg0, UArg arg1)
{
    Int             status          = 0;
    UInt16          remoteProcId;
    Bool            running         = TRUE;

    Log_print0(Diags_ENTRY | Diags_INFO , "--> smain:");

    /* initialize modules */
    Server_init();

    /* turn on Diags_INFO trace */
    Diags_setMask("Server+F");

    /* loop forever */
    while (running) {

        /* only one thread must call start */
        do {
            status = Ipc_start();
        } while (status == Ipc_E_NOTREADY);

        if (status < 0) {
            Log_error0("smain: Ipc_start() failed");
            goto leave;
        }

        /* attach to the remote processor */
        remoteProcId = MultiProc_getId("HOST");

        /* connect to remote processor */
        do {
            status = Ipc_attach(remoteProcId);

            if (status == Ipc_E_NOTREADY) {
                Task_sleep(100);
            }

        } while (status == Ipc_E_NOTREADY);

        if (status < 0) {
            Log_error0("smain: Ipc_attach() failed");
            goto leave;
        }

        /* BEGIN server phase */

        /* server setup phase */
        status = Server_create(remoteProcId);

        if (status < 0) {
            goto leave;
        }

        /* server execute phase */
        status = Server_exec();

        if (status < 0) {
            goto leave;
        }

        /* server shutdown phase */
        status = Server_delete();

        if (status < 0) {
            goto leave;
        }

        /* END server phase */

        /* disconnect from remote processor */
        do {
            status = Ipc_detach(remoteProcId);

            if (status == Ipc_E_NOTREADY) {
                Task_sleep(100);
            }

        } while (status == Ipc_E_NOTREADY);

        if (status < 0) {
            Log_error0("smain: Failed to disconnect from remote process");
            goto leave;
        }

        /* only one thread must call stop */
        Ipc_stop();
    }

    /* finalize modules */
    Server_exit();

leave: 
    Log_print1(Diags_EXIT, "<-- smain: %d", (IArg)status);
    return;
}
