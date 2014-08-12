#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>

#include "decklinkcapturedelegate.h"
#include "DeckLinkAPI.h"
class GLWidget;

class Window : public QWidget
{
 Q_OBJECT

public:
	Window(int argc, char *argv[]);
	~Window();
private:
	int processArguments(int argc, char* argv[]);
	void usage();

private:
	GLWidget 						*glWidget;

	IDeckLink 						*deckLink;
	IDeckLinkInput					*deckLinkInput;
	IDeckLinkDisplayModeIterator	*displayModeIterator;
	IDeckLinkIterator        		*deckLinkIterator;
     
	int 							card;

	BMDTimecodeFormat				g_timecodeFormat;
	int	     						g_videoModeIndex;
	int								g_maxFrames;

	int 							displayWidth, displayHeight;
	double							displayFPS;	
	BMDTimeValue 					frameRateDuration, frameRateScale;
};

#endif
