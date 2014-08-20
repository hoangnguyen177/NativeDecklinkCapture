#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include "decklinkcapturedelegate.h"
#include <QtOpenGL>

struct timeval tv_start;

#define DROP_THRESHOLD 0.95



DeckLinkCaptureDelegate::DeckLinkCaptureDelegate(GLWidget* _glWidget) : m_refCount(0)
{
	fprintf(stderr, "initing DeckLinkCaptureDelegate\n");
	pthread_mutex_init(&m_mutex, NULL);
	glWidget = _glWidget;
	frameCount = 0;
		
}

DeckLinkCaptureDelegate::~DeckLinkCaptureDelegate()
{
	pthread_mutex_destroy(&m_mutex);
}




ULONG DeckLinkCaptureDelegate::AddRef(void)
{
	pthread_mutex_(&m_mutex);
		m_refCount++;
	pthread_mutex_un(&m_mutex);

	return (ULONG)m_refCount;
}

ULONG DeckLinkCaptureDelegate::Release(void)
{
	pthread_mutex_(&m_mutex);
		m_refCount--;
	pthread_mutex_un(&m_mutex);

	if (m_refCount == 0)
	{
		delete this;
		return 0;
	}

	return (ULONG)m_refCount;
}



HRESULT DeckLinkCaptureDelegate::VideoInputFrameArrived(IDeckLinkVideoInputFrame* videoFrame, IDeckLinkAudioInputPacket* audioFrame)
{
	IDeckLinkVideoFrame*	                rightEyeFrame = NULL;
	IDeckLinkVideoFrame3DExtensions*        threeDExtensions = NULL;
	unsigned char * 			frameBytes;
	static int warmup = (int)fps; // 1 sec of frames
	static BMDTimeValue startoftime;
	static int skipf = 0;
	static double lastframetime = 0.0;

	// Handle Video Frame
	if(videoFrame)
	{	

#if 0
		if (warmup > 0) {
		    warmup--;
		    fprintf(stderr, "Warmup: frame %03d\r", warmup);
		    return S_OK;
		}
		if (warmup == 0) {
		    fprintf(stderr, "\nWarmup done\n");
		    warmup--;
		}
#endif

		if (frameCount==0) {
			BMDTimeValue hframedur;
			HRESULT h = videoFrame->GetHardwareReferenceTimestamp(frameRateScale, &startoftime, &hframedur);
			lastframetime = DeckLinkCaptureDelegate::sageTime();
		}

		if (videoFrame->GetFlags() & bmdFrameHasNoInputSource)
		{
			fprintf(stderr, "Frame received (#%lu) - No input signal detected\n", frameCount);
		}
		else
		{
			BMDTimeScale htimeScale = frameRateScale;
			BMDTimeValue hframeTime;
			BMDTimeValue hframeDuration;
			HRESULT h = videoFrame->GetHardwareReferenceTimestamp(htimeScale, &hframeTime, &hframeDuration);
			if (h == S_OK) {
				double frametime = (double)(hframeTime-startoftime) / (double)hframeDuration;
				double instantfps = 1000000.0/(DeckLinkCaptureDelegate::sageTime()-lastframetime);
				if ( ((int)frametime) > frameCount) { skipf = 1; frameCount++; }
				if ( instantfps <= (fps*DROP_THRESHOLD) ) { skipf = 1; frameCount++; }   // if lower than 95% of target FPS
				//if (skipf)
					//sage::printLog("HARD Time: %10ld %10ld %10ld: %f %d %f --", hframeTime, hframeDuration, htimeScale, frametime, frameCount, instantfps );
				//else
					//sage::printLog("HARD Time: %10ld %10ld %10ld: %f %d %f", hframeTime, hframeDuration, htimeScale, frametime, frameCount, instantfps );
				lastframetime = DeckLinkCaptureDelegate::sageTime();
			} else {
				BMDTimeValue frameTime, frameDuration;
				videoFrame->GetStreamTime(&frameTime, &frameDuration, frameRateScale);

				fprintf(stderr, "SOFT Time: %10ld %10ld %10ld", frameTime, frameDuration, frameRateScale);
			}

			//double actualFPS = (double)frameRateScale / (double)frameRateDuration;

			if (!skipf) {
				// Get the pixels from the capture
				videoFrame->GetBytes((void**)&frameBytes);

				
					int localskip = 0;
					if (fps >= 50.0) {                // if 50p or 60p rate
						if ( (frameCount%2) == 0 ) {  // skip every other frame
							localskip = 1;
						}
					}
					if (!localskip){
						//swapWithBuffer(sageInf, (unsigned char *)frameBytes);
						//TODO HERE: get glWidge to display frameBytes
						if(glWidget->isReadyToReceiveNewFrame()){
							fprintf(stderr, "glWidget is ready. Set buffer \n");
							glWidget->setBuffer(frameBytes);
							emit this->updateGLSignal();
						}
						else
						fprintf(stderr, "glWidget is not ready to receive new frame. Skip.\n");						
					}

			} else {
				fprintf(stderr, "#");
			}

		



			/*videoFrame->GetBytes(&frameBytes);
			write(videoOutputFile, frameBytes, videoFrame->GetRowBytes() * videoFrame->GetHeight());
			
			if (rightEyeFrame)
			{
				rightEyeFrame->GetBytes(&frameBytes);
				write(videoOutputFile, frameBytes, videoFrame->GetRowBytes() * videoFrame->GetHeight());
			}*/
		      
		}
		frameCount++;

		/*if (g_maxFrames > 0 && frameCount >= g_maxFrames)
		{
			pthread_cond_signal(&sleepCond);
		}*/
	}

	// Handle Audio Frame
	//IGNORED at the moment Hoang
	/*if (audioFrame)
	{
		if (audioOutputFile != -1)
		{
			audioFrame->GetBytes(&audioFrameBytes);
			write(audioOutputFile, audioFrameBytes, audioFrame->GetSampleFrameCount() * g_audioChannels * (g_audioSampleDepth / 8));
		}
	}*/


    //frameCount++;
	skipf = 0;

    return S_OK;
}

HRESULT DeckLinkCaptureDelegate::VideoInputFormatChanged(BMDVideoInputFormatChangedEvents events, IDeckLinkDisplayMode *mode, BMDDetectedVideoInputFormatFlags)
{
    fprintf(stderr, "VideoInputFormatChanged VideoInputFormatChanged VideoInputFormatChanged\n");
    fprintf(stderr, "VideoInputFormatChanged VideoInputFormatChanged VideoInputFormatChanged\n");
    fprintf(stderr, "VideoInputFormatChanged VideoInputFormatChanged VideoInputFormatChanged\n");
    return S_OK;
}


/***************************** MISC ************************/
double DeckLinkCaptureDelegate::sageTime(){
	double timeStamp;
	struct timeval tv;
	gettimeofday(&tv, NULL);
    timeStamp = (double)(tv.tv_sec-tv_start.tv_sec)*1000000.0+(double)(tv.tv_usec-tv_start.tv_usec);
    return timeStamp;
}

