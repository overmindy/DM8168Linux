
/*
 *  Copyright (c) 2010-2011, Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Contact information for paper mail:
 *  Texas Instruments
 *  Post Office Box 655303
 *  Dallas, Texas 75265
 *  Contact information:
 *  http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
 *  DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
 *  ============================================================================
 *
 */

/* -------------------- system and platform files ----------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ti/Std.h>
#include <stdio.h>
#include <ti/xdais/dm/ispeech1_pcm.h>
#include <ti/xdais/dm/isphdec1.h>
#include <ti/syslink/utils/IHeap.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/xdais/dm/iauddec1.h>
#include <alsa/asoundlib.h>
#include <sys/time.h>

/*-------------------------program files --------------------------------------*/
#include "ti/rpe.h"
#include "system_init.h"

#define NUMBER_OF_TIMES               10000

void Speech_Decoder_example ()
{
    uint8_t                    *bufPtr = NULL;
    uint8_t                    *inputData = NULL;
    uint16_t                   *outputData = NULL;

    /* Heap to allocate input/output buffers */
    IHeap_Handle                heap = NULL;

    /* Decoder Create Parameter */
    ISPHDEC1_Params             decParams = { 0 };

    /* RPE component attribute structure used in ceate call */
    Rpe_Attributes              instAttr = { 0 };

    /* Client handle from create call */
    Rpe_ClientHandle            clientHandle = NULL;
    Rpe_CallDescHandle          processCallDesc = NULL,   /* Process call desc */
     				controlCallDesc = NULL;   /* Control call desc */

    /* Engaine call status */
    int32_t                     status;

    /* Pointers to decoder control call parameters located in call decriptor */
    ISPHDEC1_Cmd               *cmdId;
    ISPHDEC1_DynamicParams     *decDynParams = NULL;
    ISPHDEC1_Status            *decStatus = NULL;

    /* Pointers to decoder process call parameters located in call decriptor */
    XDM1_SingleBufDesc         *inBufDesc = NULL;
    XDM1_SingleBufDesc         *outBufDesc = NULL;
    ISPHDEC1_InArgs            *decInArgs = NULL;
    ISPHDEC1_OutArgs           *decOutArgs = NULL;

    uint32_t                    minInputBufSize, maxOutputBufSize;
    
    /* FILE pointer for config file, input file, output file */
    struct timeval              tv;
    int32_t                     i;
    double                      time_now, time_prev, diff;

    /*--------------------------------------------------------------------------*/
    /* Set Speech decoder create time parameters */
    /*--------------------------------------------------------------------------*/
    decParams.size = sizeof (ISPHDEC1_Params);
    decParams.compandingLaw = ISPEECH1_PCM_COMPAND_DEFAULT;
    decParams.packingType = 0;
    decParams.codecSelection = 0;
    decParams.bitRate = 0;
    decParams.reserved = 0;

    /*--------------------------------------------------------------------------*/
    /* Set inst attributes */
    /*--------------------------------------------------------------------------*/
    instAttr.priority = RPE_PROCESSING_PRIORITY_MEDIUM;
    instAttr.inBufCpuAccessMode = RPE_CPU_ACCESS_MODE_WRITE;
    instAttr.outBufCpuAccessMode = RPE_CPU_ACCESS_MODE_READ;

    status = Rpe_create ("SPHDEC1_COPY_TI",
                         &instAttr, &decParams, &clientHandle);
    if (RPE_S_SUCCESS != status) {
        printf ("Rpe_create failed, status: %d\n", status);
        return;
    }
    status = Rpe_acquireCallDescriptor (clientHandle,
                                        RPE_CALL_DESC_CONTROL,
                                        &controlCallDesc,
                                        &cmdId, &decDynParams, &decStatus);
    if (RPE_S_SUCCESS != status) {
        printf ("Rpe_acquireCallDescriptor failed, status: %d\n", status);
        return;
    }

    /*----------------------------------------------------------------------*/
    /* Call control api using XDM_GETBUFINFO to get I/O buffer requirements */
    /*----------------------------------------------------------------------*/
    *cmdId = XDM_GETBUFINFO;
    decDynParams->size = sizeof (*decDynParams);
    decStatus->size = sizeof (*decStatus);

    status = Rpe_control (controlCallDesc);
    if (RPE_S_SUCCESS != status) {
        printf ("Rpe control call XDM_GETBUFINFO failed, status: %d\n", status);
        return;
    }

    status = Rpe_acquireCallDescriptor (clientHandle,
                                        RPE_CALL_DESC_PROCESS,
                                        &processCallDesc,
                                        &inBufDesc,
                                        &outBufDesc, &decInArgs, &decOutArgs);
    if (RPE_S_SUCCESS != status) {
        printf ("Rpe_acquireCallDescriptor failed, status: %d\n", status);
        return;
    }

    inBufDesc->bufSize = minInputBufSize = decStatus->bufInfo.minInBufSize[0];
    outBufDesc->bufSize = maxOutputBufSize =
        decStatus->bufInfo.minOutBufSize[0];

    /*--------------------------------------------------------------------*/
    /* Allocate the I/O buffers from shared region */
    /*--------------------------------------------------------------------*/
    heap = SharedRegion_getHeap (IPC_SR_FRAME_BUFFERS_ID);

    inputData = (uint8_t *) Memory_alloc (heap, minInputBufSize, 128, NULL);

    if (inputData == NULL) {
        printf ("Allocation Failed for inputData \n");
        return;
    }

    bufPtr = inputData;

    outputData = (uint16_t *) Memory_alloc (heap, maxOutputBufSize, 128, NULL);

    if (outputData == NULL) {
        printf ("Allocation Failed for outputData \n");
        return;
    }

    inBufDesc->buf = (XDAS_Int8 *) inputData;
    outBufDesc->buf = (XDAS_Int8 *) outputData;

    *cmdId = XDM_SETDEFAULT;
    decDynParams->size = sizeof (*decDynParams);
    decStatus->size = sizeof (*decStatus);

    status = Rpe_control (controlCallDesc);
    if (RPE_S_SUCCESS != status) {
        printf ("Rpe control call XDM_SETDEFAULT failed, status: %d\n", status);
        return;
    }

    /*-------------------------------------------------------------------*/
    /* Call control api using XDM_SETPARAMS to set run-time parameters */
    /*-------------------------------------------------------------------*/
    *cmdId = XDM_SETPARAMS;
    decDynParams->size = sizeof (*decDynParams);
    decStatus->size = sizeof (*decStatus);

    status = Rpe_control (controlCallDesc);
    if (RPE_S_SUCCESS != status) {
        printf ("Rpe control call XDM_SETPARAMS failed, status: %d\n", status);
        return;
    }

    decInArgs->size = sizeof (*decInArgs);
    decOutArgs->size = sizeof (*decOutArgs);
    decOutArgs->extendedError = XDM_EOK;

    /* get system time */
    gettimeofday (&tv, NULL);

    /* convert to double */
    time_prev = tv.tv_sec + (tv.tv_usec * (1.0 / 1000000.0));

    for (i = 0; i < NUMBER_OF_TIMES; i++) {
        inBufDesc->buf = (XDAS_Int8 *) inputData;
        inBufDesc->bufSize = minInputBufSize;

        /*---------------------------------------------------------------*/
        /* Process call to decode the frame */

        /*---------------------------------------------------------------*/
        status = Rpe_process (processCallDesc);
        if (RPE_S_SUCCESS != status) {
            printf ("Rpe process call failed, status: %d\n", status);
            if ( decOutArgs->extendedError ) {
                printf("Codec failed error %0x to decode\n", 
                (uint32_t) decOutArgs->extendedError);
                       
                if (XDM_ISFATALERROR(decOutArgs->extendedError))
                        goto end;
            }
        }
    }

    /* get system time */
    gettimeofday (&tv, NULL);

    /* convert to double */
    time_now = tv.tv_sec + (tv.tv_usec * (1.0 / 1000000.0));

    diff = time_now - time_prev;
    printf ("\nThe time taken to make %d process calls for the speech copy \
		\ncodec is %f sec\n",i, diff);

    printf ("Completed process call %d times\n", i);

end:
    Memory_free (heap, bufPtr, minInputBufSize);
    Memory_free (heap, outputData, maxOutputBufSize);

    Rpe_delete (clientHandle);
    if (RPE_S_SUCCESS != status) {
        printf ("Rpe_create failed, status: %d\n", status);
        return;
    }

    printf ("Test completed\n");
}

/** 
********************************************************************************
 *  @fn     main
 *  @brief  This function does the platform specific initialization. It then 
 *          calls the DSP Copy Example IL Client function. Finally, it performs
 *          platform specific de-initializations                               
 * 
 *  @param[in ]  arg1  : Not used, Reserved for future use
 *  @param[in ]  arg2  : Not used, Reserved for future use
 * 
 *  @returns none 
********************************************************************************
*/

int main (int argc, char **argv)
{
    int32_t                     status;

    printf (" Test Case 1: Running Copy Speech Decoder Examples \n");
    printf ("======================\n");

    printf ("\nStarting threads\n");
    fflush (stdout);

    /* Initializing */

    System_procInit ();

    /*------------------------------------------------------------------------*/
    /* RPE API to initialize the client config array with configs available */
    /* on server. This API needs to called only once by the application after */
    /* the firmware is loaded */
    /*------------------------------------------------------------------------*/
    status = Rpe_init ();
    if (RPE_S_SUCCESS != status) {
        printf ("Rpe_init failed, status: %d\n", status);
        exit (-1);
    }

    /* Calling the copy codec speech decoder component */
    Speech_Decoder_example ();

    Rpe_deinit ();

    System_procDeInit ();

    exit (0);
}
