/* 
 * Copyright (c) 2011, Texas Instruments Incorporated
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
 * 
 */
/*
 *  ======== sphdec1_copy.c ========
 *  Speech decoder "copy" algorithm, PCM codec family.
 *
 *  This file contains an implementation of the ISPHDEC1 interface
 *  defined by XDM.
 */
#include <xdc/std.h>
#include <string.h>

#include <ti/xdais/dm/isphdec1.h>
#include <ti/xdais/dm/ispeech1_pcm.h>

#ifdef __TI_COMPILER_VERSION__
/* XDAIS Rule 13 - this #pragma should only apply to TI codegen */
#pragma CODE_SECTION(SPHDEC1COPY_TI_control, ".text:algControl")
#pragma CODE_SECTION(SPHDEC1COPY_TI_process, ".text:algProcess")
#pragma CODE_SECTION(SPHDEC1COPY_TI_initObj, ".text:algInit")
#pragma CODE_SECTION(SPHDEC1COPY_TI_free,    ".text:algFree")
#pragma CODE_SECTION(SPHDEC1COPY_TI_alloc,   ".text:algAlloc")
#endif

#include "sphdec1_copy_ti.h"
#include "sphdec1_copy_ti_priv.h"

/*===========================================*/
#include "dsplib/DSPF_sp_bitrev_cplx.h"     /* using dsplib for C674X to test, by Guanqing 20210623  not used! */
#include "dsplib/DSPF_sp_cfftr2_dit.h"     /* using dsplib for C674X to test, by Guanqing 20210623   not used! */
void DSPF_sp_fftSPxSP(int N, float *ptr_x, float *ptr_w, float *ptr_y, unsigned char *brev, int n_min, int offset, int n_max);
/*int gen_twiddle(float *w, int n);
void bit_rev(float *x, int n);
void bitrev_index(short *index, int nx);
void gen_w_r2(float* w, int n);   */
void tw_gen(float* w, int n);
#define PI 3.14159265358979323846
#define N (256)                          /* Be carefully !!!  if size of FFT great than 256, must change N !!!! in this test,FFT only 256 ! */
#pragma DATA_ALIGN(w, 8);
float w[N * 2];
#pragma DATA_ALIGN(ptr_index, 8);
short ptr_index[256];                    /* Pls be sure this param is suitable !  not used ! */
short Size_of_fft; 
#pragma DATA_ALIGN(brev, 8);
unsigned char brev[64] = {
    0x0, 0x20, 0x10, 0x30, 0x8, 0x28, 0x18, 0x38,
    0x4, 0x24, 0x14, 0x34, 0xc, 0x2c, 0x1c, 0x3c,
    0x2, 0x22, 0x12, 0x32, 0xa, 0x2a, 0x1a, 0x3a,
    0x6, 0x26, 0x16, 0x36, 0xe, 0x2e, 0x1e, 0x3e,
    0x1, 0x21, 0x11, 0x31, 0x9, 0x29, 0x19, 0x39,
    0x5, 0x25, 0x15, 0x35, 0xd, 0x2d, 0x1d, 0x3d,
    0x3, 0x23, 0x13, 0x33, 0xb, 0x2b, 0x1b, 0x3b,
    0x7, 0x27, 0x17, 0x37, 0xf, 0x2f, 0x1f, 0x3f
};
/*=============================================*/

/* TODO, need to autogenerate this */
#define VERSIONSTRING "1.00.00.00"

/* buffer definitions */
#define MININBUFS       1
#define MINOUTBUFS      1
#define MININBUFSIZE    256*8    /* by guanqing 20210616 for test sqrt ! */   
#define MINOUTBUFSIZE   256*8    /* for FFT, pls be sure this param is suitable ! 256 points: 256*2*4 */

#define IALGFXNS  \
    &SPHDEC1COPY_TI_IALG,       /* module ID */                         \
    NULL,                       /* activate */                          \
    SPHDEC1COPY_TI_alloc,       /* alloc */                             \
    NULL,                       /* control (NULL => no control ops) */  \
    NULL,                       /* deactivate */                        \
    SPHDEC1COPY_TI_free,        /* free */                              \
    SPHDEC1COPY_TI_initObj,     /* init */                              \
    NULL,                       /* moved */                             \
    NULL                        /* numAlloc (NULL => IALG_MAXMEMRECS) */

/*
 *  ======== SPHDEC1COPY_TI_ISPHDECCOPY ========
 *  This structure defines TI's implementation of the ISPHDEC1 interface
 *  for the SPHDEC1COPY_TI module.
 */
ISPHDEC1_Fxns SPHDEC1COPY_TI_ISPHDEC1COPY = { /* module_vendor_interface */
    {IALGFXNS},
    SPHDEC1COPY_TI_process,
    SPHDEC1COPY_TI_control,
};

/*
 *  ======== SPHDEC1COPY_TI_IALG ========
 *  This structure defines TI's implementation of the IALG interface
 *  for the SPHDEC1COPY_TI module.
 */
#ifdef __TI_COMPILER_VERSION__
/* satisfy XDAIS symbol requirement without any overhead */
#if defined(__TI_ELFABI__) || defined(__TI_EABI_SUPPORT__)

/* Symbol doesn't have any leading underscores */
asm("SPHDEC1COPY_TI_IALG .set SPHDEC1COPY_TI_ISPHDEC1COPY");

#else

/* Symbol has a single leading underscore */
asm("_SPHDEC1COPY_TI_IALG .set _SPHDEC1COPY_TI_ISPHDEC1COPY");

#endif
#else

/*
 *  We duplicate the structure here to allow this code to be compiled and
 *  run using non-TI toolchains at the expense of unnecessary data space
 *  consumed by the definition below.
 */
IALG_Fxns SPHDEC1COPY_TI_IALG = {      /* module_vendor_interface */
    IALGFXNS
};

#endif

ISPHDEC1_Params SPHDEC1COPY_TI_PARAMS = {
    sizeof(ISPHDEC1_Params),
    ISPEECH1_PCM_COMPAND_DEFAULT,
    0,                              /* packingType is a don't care for PCM */
    0,                              /* codecSelect is a don't care for PCM */
    0,                              /* bitRate is a don't care for PCM */
    0,                              /* reserved */
    NULL
};

/*
 *  ======== SPHDEC1COPY_TI_alloc ========
 *  Return a table of memory descriptors that describe the memory needed
 *  to construct our object.
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int SPHDEC1COPY_TI_alloc(const IALG_Params *algParams,
    IALG_Fxns **pf, IALG_MemRec memTab[])
{
    /* Request memory for my object */
    memTab[0].size = sizeof(SPHDEC1COPY_TI_Obj);
    memTab[0].alignment = 0;
    memTab[0].space = IALG_EXTERNAL;
    memTab[0].attrs = IALG_PERSIST;

    return (1);
}


/*
 *  ======== SPHDEC1COPY_TI_free ========
 *  Return a table of memory pointers that should be freed.  Note
 *  that this should include *all* memory requested in the
 *  alloc operation above.
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int SPHDEC1COPY_TI_free(IALG_Handle handle, IALG_MemRec memTab[])
{
    /*
     * Because our internal object size doesn't depend on create
     * params, we can just leverage the algAlloc() call with default
     * (NULL) create params.
     */
    return(SPHDEC1COPY_TI_alloc(NULL, NULL, memTab));
}


/*
 *  ======== SPHDEC1COPY_TI_initObj ========
 *  Initialize the memory allocated on our behalf (including our object).
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int SPHDEC1COPY_TI_initObj(IALG_Handle handle, const IALG_MemRec memTab[],
    IALG_Handle parent, const IALG_Params *algParams)
{
    SPHDEC1COPY_TI_Obj *obj = (SPHDEC1COPY_TI_Obj *)handle;
    const ISPHDEC1_Params *params = (ISPHDEC1_Params *)algParams;

    /*
     * In lieu of XDM defined default params, use our codec-specific ones.
     * Note that these default values _should_ be documented in your algorithm
     * documentation so users know what to expect.
     */
    if (params == NULL) {
        params = &SPHDEC1COPY_TI_PARAMS;
    }

    obj->compandingLaw = params->compandingLaw;
    obj->packingType = params->packingType;
    obj->codecSelection = params->codecSelection;
    obj->postFilter = ISPEECH1_POSTFILTER_DEFAULT;
    obj->bitRate = params->bitRate;
/*=====================================================================================*/
    Size_of_fft =  params->reserved;   /* decParams.reserved=256 in ARM APP   */
    /* ---by guanqing 20210623------------------------------------------- */
    /* Generate index table.                                              */
    /* ------------------------------------------------------------------ */
    /*bitrev_index(ptr_index, Size_of_fft);   not used !   */
    /* -------------------------------------------------------------------- */
    /* Generate twiddle coefficient table.                                  */
    /* -------------------------------------------------------------------- */
    /*gen_twiddle(w, Size_of_fft); 
    gen_w_r2(w,Size_of_fft);    
    bit_rev(w, Size_of_fft>>1);        not used !*/
    tw_gen (w, Size_of_fft);      /* for FFT function to used by guanqing 20210623   */
/*=====================================================================================*/

    return (IALG_EOK);
}

#include "dsplib/sqrtsp.h"     /* using mathlib for C674X to test, by Guanqing 20210616  */

/*
 *  ======== SPHDEC1COPY_TI_process ========
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
XDAS_Int32 SPHDEC1COPY_TI_process(ISPHDEC1_Handle h,
    XDM1_SingleBufDesc *inBuf, XDM1_SingleBufDesc *outBuf,
    ISPHDEC1_InArgs *inArgs, ISPHDEC1_OutArgs *outArgs)
{
    XDAS_Int32 numInBytes,i;
    float *in_ftemp,*out_ftemp;    /* using mathlib for C674X to test, by Guanqing 20210616  */
    float ftemp1,ftemp2;

    /* validate arguments - this codec only supports "base" XDM. */
    if ((inArgs->size != sizeof(*inArgs)) ||
            (outArgs->size != sizeof(*outArgs))) {
        XDM_SETUNSUPPORTEDPARAM(outArgs->extendedError);

        return (ISPHDEC1_EUNSUPPORTED);
    }

    /*
     * The number of bytes we can consume is the lesser of the specified
     * number of bytes to decode (inBuf->bufSize) and the size of the
     * buffer that will contain the output (outBuf->bufSize).
     */
    numInBytes = (inBuf->bufSize <= outBuf->bufSize) ?
            inBuf->bufSize : outBuf->bufSize;

    /* process the data: read input, produce output */
    /*memcpy(outBuf->buf, inBuf->buf, numInBytes);  */

    /* by guanqing to use mathlib for C674x .......2021-6-16   */
    in_ftemp = (float *)inBuf->buf;
    out_ftemp = (float *)outBuf->buf;
    
    /*for(i=0;i< numInBytes/4;i++)
    {    *out_ftemp++ = sqrtsp(*in_ftemp++); }   */  /* using mathlib for C674X to test, by Guanqing  */
   
/*===================================================================================*/    
    /*memcpy(outBuf->buf, inBuf->buf, Size_of_fft*2*4);*/ /*  save inbuf to return spectrum  !   */
   
    /*DSPF_sp_cfftr4_dif(in_ftemp, w, Size_of_fft); */            /*  using DSPlib for testing FFT(dit,radix-2,256 points) ,by guanqing 20210623  */
    /*bit_rev(in_ftemp, Size_of_fft);  */
    /*DSPF_sp_bitrev_cplx(in_ftemp, ptr_index,Size_of_fft);  */   /* using DSPlib for bit-reversal in FFT result ! , by guanqing 20210623  */
/* ---------the following codes to cal rad in DSPF_sp_fftSPxSP  ----------
        j = 0;
        for (i = 0; i <= 31; i++)
            if ((N & (1 << i)) == 0)
                j++;
            else
                break;

        if (j % 2 == 0)
            rad = 4;
        else
            rad = 2;  --------------------------------------------- */   
    DSPF_sp_fftSPxSP (Size_of_fft, in_ftemp, w, out_ftemp, brev, 4, 0, Size_of_fft);  /* rad=4,for 256,1024,..., points FFT */
                                                                                      /* others, rad=2, for 128,512,2024 ..., points */
    out_ftemp = (float *)outBuf->buf;
    for(i=0;i< Size_of_fft;i++)
    {    ftemp1=*out_ftemp++ ; ftemp2=*out_ftemp--;
         *out_ftemp = sqrtsp(ftemp1*ftemp1+ftemp2*ftemp2); out_ftemp+=2; }    /* using mathlib to cal power spectrum ! by guanqing 20210623   */
/*===================================================================================*/
    
    /* -------------------------------------------------------*/
    /* report _how_ we accessed the 2 data buffers */

    XDM_CLEARACCESSMODE_WRITE(inBuf->accessMask);
    XDM_SETACCESSMODE_READ(inBuf->accessMask);

    XDM_CLEARACCESSMODE_READ(outBuf->accessMask);
    XDM_SETACCESSMODE_WRITE(outBuf->accessMask);

    /* Fill out the rest of the outArgs struct */
    outArgs->dataSize = 0;  /* TBD */
    outArgs->extendedError = 0;

    return (ISPHDEC1_EOK);
}


/*
 *  ======== SPHDEC1COPY_TI_control ========
 */
XDAS_Int32 SPHDEC1COPY_TI_control(ISPHDEC1_Handle handle, ISPHDEC1_Cmd id,
    ISPHDEC1_DynamicParams *dynParams, ISPHDEC1_Status *status)
{
    XDAS_Int32 retVal;
    SPHDEC1COPY_TI_Obj *obj = (SPHDEC1COPY_TI_Obj *)handle;

    /* validate arguments - this codec only supports "base" XDM. */
    if ((dynParams->size != sizeof(*dynParams)) ||
            (status->size != sizeof(*status))) {
        return (ISPHDEC1_EUNSUPPORTED);
    }

    /* initialize for the general case where we don't access the data buffer */
    XDM_CLEARACCESSMODE_READ(status->data.accessMask);
    XDM_CLEARACCESSMODE_WRITE(status->data.accessMask);

    switch (id) {
        case XDM_GETSTATUS:
        case XDM_GETBUFINFO:
            status->postFilter = obj->postFilter;
            status->extendedError = 0;
            status->compandingLaw = obj->compandingLaw;
            status->packingType = obj->packingType;
            status->codecSelection = obj->codecSelection;
            status->bitRate = obj->bitRate;

            status->bufInfo.minNumInBufs = MININBUFS;
            status->bufInfo.minNumOutBufs = MINOUTBUFS;
            status->bufInfo.minInBufSize[0] = MININBUFSIZE;
            status->bufInfo.minOutBufSize[0] = MINOUTBUFSIZE;

            retVal = ISPHDEC1_EOK;
            break;

        case XDM_SETPARAMS:
            obj->postFilter = dynParams->postFilter;
            retVal = ISPHDEC1_EOK;
            break;

        case XDM_SETDEFAULT:
            obj->compandingLaw = SPHDEC1COPY_TI_PARAMS.compandingLaw;
            obj->packingType = SPHDEC1COPY_TI_PARAMS.packingType;
            obj->codecSelection = SPHDEC1COPY_TI_PARAMS.codecSelection;
            obj->postFilter = ISPEECH1_POSTFILTER_DEFAULT;

            retVal = ISPHDEC1_EOK;
            break;

        case XDM_RESET:
        case XDM_FLUSH:
            retVal = ISPHDEC1_EOK;
            break;

        case XDM_GETVERSION:
            if ((status->data.buf != NULL) &&
                (status->data.bufSize >= strlen(VERSIONSTRING))) {
                strncpy((char *)status->data.buf, VERSIONSTRING,
                    strlen(VERSIONSTRING));

                /* null terminate the string */
                status->data.buf[strlen(VERSIONSTRING)] = '\0';

                /* strncpy wrote to the data buffer */
                XDM_SETACCESSMODE_WRITE(status->data.accessMask);

                retVal = ISPHDEC1_EOK;
            }
            else {
                retVal = ISPHDEC1_EFAIL;
            }

            break;

        default:
            /* unsupported cmd */
            retVal = ISPHDEC1_EFAIL;
            break;
    }

    return (retVal);
}

/*############################################################################### added by guanqing 20210623 */
/* -------------------------------------------------------- */
/* This routine calculates the index for bit reversal of    */
/* an array of length nx. The length of the index table is  */
/* 2^(2*ceil(k/2)) where nx = 2^k.                          */
/*                                                          */
/* In other words, the length of the index table is:        */
/* - for even power of radix: sqrt(nx)                      */
/* - for odd power of radix: sqrt(2*nx)                     */
/* -------------------------------------------------------- */
void bitrev_index(short *index, int nx)
{
    int i, j, k, radix = 4;    /* using radix-2 FFT,by guanqing 20210623  */
    short nbits, nbot, ntop, ndiff, n2, raddiv2;
    nbits = 0;
    i = nx;

    while (i > 1)
    {
        i = i >> 1;
        nbits++;
    }

    raddiv2 = radix >> 1;
    nbot = nbits >> raddiv2;
    nbot = nbot << raddiv2 - 1;
    ndiff = nbits & raddiv2;
    ntop = nbot + ndiff;
    n2 = 1 << ntop;
    index[0] = 0;

    for ( i = 1, j = n2/radix + 1; i < n2 - 1; i++)
    {
        index[i] = j - 1;
        for (k = n2/radix; k*(radix-1) < j; k /= radix)
            j -= k*(radix-1);
        j += k;
    }

    index[n2 - 1] = n2 - 1;
}
/* ======================================================================== */
/*  GEN_TWIDDLE -- Generate twiddle factors for TI's custom FFTs.           */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is called as follows:                                  */
/*                                                                          */
/*          int gen_twiddle(float *w, int n)                                */
/*                                                                          */
/*          float  *w     Pointer to twiddle-factor array                   */
/*          int    n      Size of FFT                                       */
/*                                                                          */
/*      The routine will generate the twiddle-factors directly into the     */
/*      array you specify.  The array needs to be 3*N/2 elements long.      */
/* ======================================================================== */
#include "dsplib/cossp.h"     /* using mathlib for C674X , by Guanqing 20210623  */
#include "dsplib/sinsp.h"     /* using mathlib for C674X , by Guanqing 20210623  */

int gen_twiddle(float *w, int n)
{
    float angle, step;
    int i;

    step = (2.0 * PI) / (float)n;
    for (i = 0, angle = 0.0; i < 3*n/4; i++, angle += step)
    {
        w[2*i + 1] = sinsp(angle);
        w[2*i + 0] = cossp(angle);
    }

    return 3*n/2;
}

/* generate real and imaginary twiddle table of size n/2 complex numbers */
void gen_w_r2(float* w, int n)
{
    int i;
   /* float pi = 4.0 * atan(1.0);  */
    float e = PI * 2.0 /(float) n;

    for(i = 0; i < (n >> 1); i++)
    {
        w[2 * i] = cossp(i * e);
        w[2 * i + 1] = sinsp(i * e);
    }
}

/* Function for generating Specialized sequence of twiddle factors */
void tw_gen (float *w, int n)
{
    int i, j, k;
    float x_t, y_t, theta1, theta2, theta3;
    /*const double PI = 3.141592654;  */

    for (j = 1, k = 0; j <= n >> 2; j = j << 2)
    {
        for (i = 0; i < n >> 2; i += j)
        {
            theta1 = 2 * PI * i / n;
            x_t = cossp (theta1);
            y_t = sinsp (theta1);
            w[k] = (float) x_t;
            w[k + 1] = (float) y_t;

            theta2 = 4 * PI * i / n;
            x_t = cossp (theta2);
            y_t = sinsp (theta2);
            w[k + 2] = (float) x_t;
            w[k + 3] = (float) y_t;

            theta3 = 6 * PI * i / n;
            x_t = cossp (theta3);
            y_t = sinsp (theta3);
            w[k + 4] = (float) x_t;
            w[k + 5] = (float) y_t;
            k += 6;
        }
    }
}

/* bit reverse coefficients */
void bit_rev(float* x, int n)
{
    int i, j, k;
    float rtemp, itemp;
    j = 0;

    for(i=1; i < (n-1); i++)
    {
        k = n >> 1;

        while(k <= j)
        {
            j -= k;
            k >>= 1;
        }

        j += k;

        if(i < j)
        {
            rtemp = x[j*2];
            x[j*2] = x[i*2];
            x[i*2] = rtemp;
            itemp = x[j*2+1];
            x[j*2+1] = x[i*2+1];
            x[i*2+1] = itemp;
        }
    }
}

