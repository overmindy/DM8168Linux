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
 *  ======== App.c ========
 *
 */

/* host header files */
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

/* package header files */
#include <ti/syslink/Std.h>     /* must be first */
#include <ti/syslink/utils/Memory.h>
#include <ti/ipc/Notify.h>
#include <ti/ipc/SharedRegion.h>


/* local header files */
#include "../shared/AppCommon.h"
#include "../shared/SystemCfg.h"
#include "App.h"

/* max number of outstanding commands minus one */
#define QUEUESIZE   8   
#define TI_STR      "texas instruments"

/* queue structure */
typedef struct 
{
    UInt32              queue[QUEUESIZE];   /* command queue */
    UInt                head;               /* queue head pointer */
    UInt                tail;               /* queue tail pointer */
    UInt32              error;              /* error flag */
    sem_t               semH;               /* handle to object above  */
} Event_Queue;   

/* module structure */
typedef struct {
    Event_Queue     eventQueue;
    UInt16          remoteProcId;
    UInt16          lineId;     /* notify line id */
    UInt32          eventId;    /* notify event id */
    Char*           bufferPtr;  /* string buffer */
} App_Module;

/* private functions */
static UInt32 App_waitForEvent(Event_Queue* eventQueue);
static Void App_notifyCB( UInt16 procId, UInt16 lineId, UInt32 eventId, 
        UArg arg, UInt32 payload);
        
/* private data */
static App_Module Module;

/*-----By guanqing added test function from file: testa.txt ,20231017-------*/
FILE 	*testin;
UInt16	Buffersize;   
/*-----End,20231017-----------------*/

/*
 *  ======== App_create ========
 *  1. create sync object
 *  2. register notify callback
 *  3. wait until remote core has also register notify callback
 *  4. create buffer from shared region 1 heap
 */

Int App_create(UInt16 remoteProcId)
{
    Int status;
    int retStatus;
    IHeap_Handle heap;
    
    printf("--> App_create:\n");
    
    /* setting default values */
    Module.eventQueue.head      = 0;               
    Module.eventQueue.tail      = 0;              
    Module.eventQueue.error     = 0;       
    Module.lineId               = SystemCfg_LineId;
    Module.eventId              = SystemCfg_EventId;
    Module.remoteProcId         = remoteProcId;
    
    /* 1. create sync object */
    retStatus = sem_init(&Module.eventQueue.semH, 0, 0);
    if(retStatus == -1) {
        printf("App_create: Could not initialize a semaphore\n");
        status = -1;
        goto leave;
    }

    /* 2. register notify callback */
    status = Notify_registerEvent(Module.remoteProcId, Module.lineId, 
            Module.eventId, App_notifyCB, (UArg) &Module.eventQueue);

    if (status < 0) {
        printf("App_create: Host failed to register event\n");
        goto leave;
    }

    /* 3. wait until remote core has also registered notify callback */
    do {
        status = Notify_sendEvent(Module.remoteProcId, Module.lineId,
                Module.eventId,APP_CMD_NOP, TRUE);

        if (status == Notify_E_EVTNOTREGISTERED) {
            usleep(100);
        }
        
    } while (status == Notify_E_EVTNOTREGISTERED);

    if (status != Notify_S_SUCCESS) {
        printf("App_create: Waiting for remote core to register has failed\n");
        status = -1;
        goto leave;
    }

    /* get handle to shared region 1 heap */
    heap = (IHeap_Handle) SharedRegion_getHeap(SHARED_REGION_1);

    if (heap == NULL) {
        status = -1;
        printf("App_create: Shared region heap does not exist\n");
        goto leave;
    }

    /* 4. create buffer from shared region 1 heap */
    Module.bufferPtr = (Char *) Memory_calloc(heap,sizeof(TI_STR),0,NULL);
    
    if(Module.bufferPtr == NULL) {
        printf("App_create: Failed to create buffer from shared region 1 "
                "heap\n");
        status = -1;
        goto leave;
    }
    
    printf("App_create: Host is ready\n");

/*-----By guanqing added test function from file: testa.txt ,20231017-------*/
    testin = fopen("testa.txt","r");
    if(testin == NULL)
    {    printf("Cannot find file : testa.txt ! You should copy this file !\n");  return(status);   }
    else
         printf("Open file:testa.txt & ");
/*-----End,20231017-----------------*/


leave: 
    printf("<-- App_create:\n");
    return (status);
}


/*
 *  ======== App_exec ========
 *  1. convert local address space pointer to generic shared region pointer
 *  2. send shared region pointer address lower two bytes
 *  3. send shared region pointer address upper two bytes
 *  4. wait for operation complete command 
 */
Int App_exec()
{
    Int                 status          = 0;
    UInt32              command         = 0;
    SharedRegion_SRPtr  sharedBufferPtr = 0;
    UInt32              event           = 0;
    int i;    //20231017

    printf("--> App_exec:\n");

    printf("App_exec: Writing string \"%s\" to shared region 1 buffer\n"
            ,TI_STR);
    sprintf(Module.bufferPtr,"%s",TI_STR);
    
    printf("App_exec: Command the remote core to convert the lowercase string"
            " to uppercase\n");
            
    /* 1. convert local address space pointer to generic shared region pointer */
    sharedBufferPtr = SharedRegion_getSRPtr(Module.bufferPtr, SHARED_REGION_1);
    
    /* store only the lower two bytes of the shared region address pointer */
    command = (sharedBufferPtr & 0xFFFF);
    
    /* add shared region pointer address low command to payload */
    command = APP_SPTR_LADDR | command;

    /* 2. send shared region pointer address lower two bytes */
    status = Notify_sendEvent(Module.remoteProcId, Module.lineId, 
            Module.eventId, command, TRUE);

    if (status < 0 )  {
        printf("App_exec: Error sending shared region pointer address lower "
                "two bytes\n");
        goto leave;
    }
    
    /* wait for shared region address acknowledge command */
    event = App_waitForEvent(&Module.eventQueue);

    if (event >= APP_E_FAILURE) {
        printf("App_exec: Received queue error: %d\n",event); 
        status = -1;       
        goto leave;
    }
    
    /* store only the upper two bytes of the shared region address pointer */
    command = ((sharedBufferPtr >> 16) & 0xFFFF);

    /* add shared region pointer address high command to payload */
    command = APP_SPTR_HADDR | command;
    
    /* 3. send shared region pointer address upper two bytes */
    status = Notify_sendEvent(Module.remoteProcId, Module.lineId, 
            Module.eventId, command, TRUE);

    if (status < 0 )  {
        printf("App_exec: Error sending shared region pointer address upper "
                "two bytes\n");
        goto leave;
    }    
       
    /* wait for shared region address acknowledge command */
    event = App_waitForEvent(&Module.eventQueue);

    if (event >= APP_E_FAILURE) {
        printf("App_exec: Received queue error: %d\n",event); 
        status = -1;       
        goto leave;
    } 
    
/* by guanqing 20231017 added notify to DSP core to start procesing  */
    Buffersize = sizeof(TI_STR);   /* how many data will be sent to DSP, and should be even ! */
    if(Buffersize%2 != 0)
        Buffersize--;
   
    command = APP_CMD_PRCESSING | Buffersize;     /* send COMMAND & datasize to DSP ! */
    
   for(i=0;i<10;i++)   /* only repeat 10 times  */
   { 
       fread(Module.bufferPtr,1,Buffersize, testin);    /* read data from testa.txt  ! */ 
       status = Notify_sendEvent(Module.remoteProcId, Module.lineId, Module.eventId, command, TRUE);
       if (status < 0 )  {
          printf("App_exec: Error sending processing command! by 20231017\n");
        goto leave;
       }    
       
       /* 4. wait for processed acknowledge command */
       event = App_waitForEvent(&Module.eventQueue);
       if (event >= APP_E_FAILURE) {
          printf("App_exec: DSP core processed error: %d\n",event); 
          status = -1;       
          goto leave;
       }
       write(1,Module.bufferPtr,Buffersize);    /* send result to STDOUT ! */
       printf("\n");
    }
    fclose(testin);
 
    /* send CMD to DSP to End ! */
    command = APP_CMD_SHUTDOWN;     /* send COMMAND & datasize to DSP ! */
    status = Notify_sendEvent(Module.remoteProcId, Module.lineId, Module.eventId, command, TRUE);
    if (status < 0 )  {
       printf("App_exec: Error sending processing_complete command! by 20231017\n");
       goto leave;
    }    
    event = App_waitForEvent(&Module.eventQueue);
    if (event >= APP_E_FAILURE) {
        printf("App_exec: DSP core process op_complete_cmd error: %d\n",event); 
        status = -1;       
        goto leave;
    }
   
    printf("\nApp_exec: Received-> Operation complete: read from file to convert 10 times !\n");
    
    //printf("App_exec: Transformed string: %s\n",Module.bufferPtr);
 /*----end 20231017 ----------*/

leave:
    printf("<-- App_exec:\n");
    return (status);
}

/*
 *  ======== App_delete ========
 *  1. sending shutdown command
 *  2. free buffer memory
 *  3. unregister notify callback
 *  4. delete sync object
 */
Int App_delete() 
{
    Int             status      = 0;
    UInt32          event       = 0;
    IHeap_Handle    heap; 

    printf("--> App_delete:\n");
   
    /* 1. sending shutdown command */
    status = Notify_sendEvent(Module.remoteProcId, Module.lineId, 
        Module.eventId, APP_CMD_SHUTDOWN, TRUE);

    if (status < 0 )  {
        printf("App_delete: Error sending shutdown command\n");
        goto leave;
    }    
    
    /* wait for shutdown acknowledge command */                               
    event = App_waitForEvent(&Module.eventQueue);

    if (event >= APP_E_FAILURE) {
        printf("App_delete: Received queue error: %d\n",event); 
        status = -1;       
        goto leave;
    }   
    
    /* 2. free buffer memory */
    heap = (IHeap_Handle) SharedRegion_getHeap(SHARED_REGION_1);
    
    Memory_free(heap,Module.bufferPtr,sizeof(TI_STR));

    /* 3. unregister notify callback */
    status = Notify_unregisterEvent(Module.remoteProcId, Module.lineId, 
            Module.eventId, App_notifyCB, (UArg) &Module.eventQueue);

    if (status < 0) {
        printf("App_delete: Unregistering notify callback failed\n"); 
        goto leave;
    }

    /* 4. delete sync object */
    sem_destroy(&Module.eventQueue.semH);
    
    printf("App_delete: Cleanup complete\n");

leave:
    printf("<-- App_delete:\n");
    return (status);
}

/*
 *  ======== App_notifyCB ========
 */
Void App_notifyCB(UInt16 procId, UInt16 lineId, UInt32 eventId, UArg arg,
        UInt32 payload)
{
    UInt            next;
    Event_Queue*    eventQueue = (Event_Queue *) arg;

    /* ignore no-op events */
    if (payload == APP_CMD_NOP) {
        return;
    }

    /* compute next slot in queue */
    next = (eventQueue->head + 1) % QUEUESIZE;

    if (next == eventQueue->tail) {
        /* queue is full, drop event and set error flag */
        eventQueue->error = APP_E_OVERFLOW;
    }
    else {
        /* queue head is only written to within this function */
        eventQueue->queue[eventQueue->head] = payload;
        eventQueue->head = next;
        
        /* signal semaphore (counting) that new event is in queue */
        sem_post(&eventQueue->semH);
    }

    return;
}

/*
 *  ======== App_waitForEvent ========
 */
static UInt32 App_waitForEvent(Event_Queue* eventQueue)
{
    UInt32 event;

    if (eventQueue->error >= APP_E_FAILURE) {
        event = eventQueue->error;
    }
    else {
        /* use counting semaphore to wait for next event */
        sem_wait(&eventQueue->semH);

        /* remove next command from queue */
        event = eventQueue->queue[eventQueue->tail];
        
        /* queue tail is only written to within this function */
        eventQueue->tail = (eventQueue->tail + 1) % QUEUESIZE;
    }

    return (event);
}
