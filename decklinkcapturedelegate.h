#ifndef __CAPTURE_H__
#define __CAPTURE_H__

#include "DeckLinkAPI.h"
#include "glwidget.h"
#include <QObject>

class DeckLinkCaptureDelegate : public QObject, public IDeckLinkInputCallback
{
	Q_OBJECT
public:
	DeckLinkCaptureDelegate(GLWidget* _glWidget);
	~DeckLinkCaptureDelegate();

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { return E_NOINTERFACE; }
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE  Release(void);
	virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags);
	virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame*, IDeckLinkAudioInputPacket*);

	void setWidth(int w)														{	width = w;		}	
	void setHeight(int h)														{	height = h;		}
	void setFPS(double _fps)													{	fps   = _fps;	}		

	void setFrameRateDuration(BMDTimeValue _frameRateDuration)					{frameRateDuration = _frameRateDuration;}
	void setFrameRateScale(BMDTimeValue _frameRateScale)						{frameRateScale = _frameRateScale;}

	long getFrameCount()	    {return frameCount;		}

	
	static double sageTime(); //replace sage::getTime
signals:
	void updateGLSignal();
private:
	ULONG						m_refCount;
	pthread_mutex_t				m_mutex;

	//variables from main
	int  						width, height; //from sageW, sageH
	double 						fps;			// from sageFPS

	BMDTimeValue 				frameRateDuration, frameRateScale;

	unsigned long 				frameCount;

	//for drawing the video frame
	GLWidget*					glWidget;
};

#endif
