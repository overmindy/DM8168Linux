/*

This example reads from the default PCM device
and writes to standard output for 5 seconds of data.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include <stdio.h>

int main() {
	long loops;
	int rc;
	int size;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	unsigned int val;
	int dir;
	snd_pcm_uframes_t frames;
	char *buffer;
        FILE *fp;

	
	printf("\nDES8168 ALSA recording...\n\n");	
	fp = fopen("alsa_wav.bin", "wb"); /*可读性，读从文件开头，写从文件结尾处*/
    	if (NULL == fp) {
           printf("open file failed ! \n");
           return -1;
        }
	/* Open PCM device for recording (capture). */
	rc = snd_pcm_open(&handle, "default",
		            SND_PCM_STREAM_CAPTURE, 0);
	if (rc < 0) {
	    fprintf(stderr,
		    "unable to open pcm device: %s\n",
		    snd_strerror(rc));
	    exit(1);
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_malloc(&params);

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(handle, params);

	/* Set the desired hardware parameters. */

	/* Interleaved mode */
	snd_pcm_hw_params_set_access(handle, params,
		              SND_PCM_ACCESS_RW_INTERLEAVED);

	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format(handle, params,
		                      SND_PCM_FORMAT_S16_LE);

	/* Two channels (stereo) */
	snd_pcm_hw_params_set_channels(handle, params, 2);

	/* 44100 bits/second sampling rate (CD quality) */
	val = 8000;
	snd_pcm_hw_params_set_rate_near(handle, params,
		                          &val, &dir);

	/* Set period size to 32 frames. */
	frames = 32;
	snd_pcm_hw_params_set_period_size_near(handle,
		                      params, &frames, &dir);

	/* Write the parameters to the driver */
	rc = snd_pcm_hw_params(handle, params);
	if (rc < 0) {
	    fprintf(stderr,
		    "unable to set hw parameters: %s\n",
		    snd_strerror(rc));
	    exit(1);
	}

	/* Use a buffer large enough to hold one period */
	snd_pcm_hw_params_get_period_size(params,
		                              &frames, &dir);
	size = frames * 4; /* 2 bytes/sample, 2 channels */
	buffer = (char *) malloc(size);

	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(params,
		                                 &val, &dir);
	loops = 4000000 / val;  //8k_sameple/s, 32 samples, --> 4ms, so 1000 loops =  4 seconds recording....
        printf("Loop times = %d,val = %d\n",loops,val);
	while (loops > 0)
	//while(1)	//still record 
	{
	    loops--;
	    rc = snd_pcm_readi(handle, buffer, frames);
	    if (rc == -EPIPE) {
	      /* EPIPE means overrun */
	      fprintf(stderr, "overrun occurred\n");
	      snd_pcm_prepare(handle);
	    } else if (rc < 0) {
	      fprintf(stderr,
		      "error from read: %s\n",
		      snd_strerror(rc));
	    } else if (rc != (int)frames) {
	      fprintf(stderr, "short read, read %d frames\n", rc);
	    }
	    rc = fwrite(buffer, 1,size,fp);
	    if (rc != size)
	      fprintf(stderr,
		      "short write: wrote %d bytes\n", rc);
	}

	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	free(buffer);
        fclose(fp);
	return 0;
}
