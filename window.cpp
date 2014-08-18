#include <QtGui>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

#include "glwidget.h"
#include "window.h"


/*constructor*/
Window::Window(int argc, char *argv[])
{
     fprintf(stderr, "initing glWidget\n");	
     glWidget = new GLWidget;
     //process the arguments
     deckLinkIterator = CreateDeckLinkIteratorInstance();
     fprintf(stderr, "Processing arguments\n");	
     if(processArguments(argc, argv) < 0)
          return;     
     //add glWidget to the window
     fprintf(stderr, "Setting up windows\n");	
     QHBoxLayout *mainLayout = new QHBoxLayout;
     mainLayout->addWidget(glWidget);
     setLayout(mainLayout);
     setWindowTitle(tr("Tele-Human"));
}

/*destructor*/
Window::~Window(){
     if (displayModeIterator != NULL)
     {
          displayModeIterator->Release();
          displayModeIterator = NULL;
     }

    if (deckLinkInput != NULL)
    {
        deckLinkInput->Release();
        deckLinkInput = NULL;
    }

    if (deckLink != NULL)
    {
        deckLink->Release();
        deckLink = NULL;
    }

     if (deckLinkIterator != NULL)
          deckLinkIterator->Release();

     if(glWidget != NULL){
          delete glWidget;
          glWidget = NULL;          
     }

}

/**
* usage
*/
void Window::usage(){
     /*          "-i <video input number>\n"; 
          "-p <pixelformat>\n" 
          "         0:  8 bit YUV (4:2:2) (default)\n"
          "         1:  10 bit YUV (4:2:2)\n"
          "         2:  10 bit RGB (4:4:4)\n"
          "    -t <format>          Print timecode\n"
          "     rp188:  RP 188\n"
          "      vitc:  VITC\n"
          "    serial:  Serial Timecode\n"
          "    -f <filename>        Filename raw video will be written to\n"
          "    -a <filename>        Filename raw audio will be written to\n"
          "    -c <channels>        Audio Channels (2, 8 or 16 - default is 2)\n"
          "    -s <depth>           Audio Sample Depth (16 or 32 - default is 32)\n"
          "    -n <frames>          Number of frames to capture (default is unlimited)\n"
          "    -3                   Capture Stereoscopic 3D (Requires 3D Hardware support)\n"
          "    -u <SAM IP>          stream using SAM (audio), 127.0.0.1 by default \n"
          "    -j                   physical inputs for SAM inputs (audio)\n"
          "    -v                   stream using SAGE (video)\n"
          "    -y                   apply deinterlacing filter\n"
          "    -ns         no sage mode \n"
          "\n"
          "Capture video and/or audio to a file. Raw video and/or audio can be viewed with mplayer eg:\n"
          "\n"
          "    Capture -m2 -n 50 -f video.raw -a audio.raw\n"
          "    mplayer video.raw -demuxer rawvideo -rawvideo pal:uyvy -audiofile audio.raw -audio-demuxer 20 -rawaudio rate=48000\n";     
     */
}

/**
* process the arugments
* return negative value of failed
*/
int Window::processArguments(int argc, char* argv[]){
     IDeckLinkAttributes    *deckLinkAttributes = NULL;
     DeckLinkCaptureDelegate           *delegate;
     IDeckLinkDisplayMode               *displayMode;
     BMDVideoInputFlags              inputFlags = 0;
     BMDDisplayMode         selectedDisplayMode = bmdModeNTSC;
     BMDPixelFormat                 pixelFormat = bmdFormat8BitYUV;
     int                       displayModeCount = 0;
     int                             exitStatus = 1;
     int                                         ch;
     bool                      foundDisplayMode = false;
     HRESULT                                 result;
     int                                   dnum = 0;
     IDeckLink                        *tempLink = NULL;
     int                                  found = 0;
     bool                             supported = 0;
     int64_t                                  ports;
     int                                  itemCount;
     int                                 vinput = 0;
     int64_t                              vport = 0;
     IDeckLinkConfiguration *deckLinkConfiguration = NULL;
     bool flickerremoval                        = true;
     bool pnotpsf                               = true;
     
     // Parse command line options
     while ((ch = getopt(argc, argv, "?h3c:d:s:f:a:m:n:p:t:u::vi:jy")) != -1) 
     {
          switch (ch) 
          {
               case 'i':
                    vinput = atoi(optarg);
                    break;
               case 'd':
                    card = atoi(optarg);
                    break;
               case 'm':
                    g_videoModeIndex = atoi(optarg);
                    break;
               case 'n':
                    g_maxFrames = atoi(optarg);
                    break;
               case '3':
                    inputFlags |= bmdVideoInputDualStream3D;
                    break;
               case 'p':
                    switch(atoi(optarg))
                    {
                         case 0: pixelFormat = bmdFormat8BitYUV; break;
                         case 1: pixelFormat = bmdFormat10BitYUV; break;
                         case 2: pixelFormat = bmdFormat10BitRGB; break;
                         default:
                              fprintf(stderr, "Invalid argument: Pixel format %d is not valid", atoi(optarg));
                              exit(1);
                    }
                    break;
               case 't':
                    if (!strcmp(optarg, "rp188"))
                         g_timecodeFormat = bmdTimecodeRP188Any;
               else if (!strcmp(optarg, "vitc"))
                         g_timecodeFormat = bmdTimecodeVITC;
               else if (!strcmp(optarg, "serial"))
                         g_timecodeFormat = bmdTimecodeSerial;
                    else
                    {
                         fprintf(stderr, "Invalid argument: Timecode format \"%s\" is invalid\n", optarg);
                         exit(1);
                    }
                    break;
               case '?':
               case 'h':
                    usage();
          }
     }

     if (!deckLinkIterator)
     {
          fprintf(stderr, "This application requires the DeckLink drivers installed.\n");
          return -1;
     }


     /* Connect to the first DeckLink instance */
     while (deckLinkIterator->Next(&tempLink) == S_OK)
     {
          if (card != dnum) {
               dnum++;
               // Release the IDeckLink instance when we've finished with it to prevent leaks
               tempLink->Release();
               continue;
          }
          else {
               deckLink = tempLink;
               found = 1;
          }
          dnum++;
     }

     if (! found ) {
          fprintf(stderr, "No DeckLink PCI cards found.\n");
          return -1;
     }
     if (deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&deckLinkInput) != S_OK)
          return -1;

     
     // Query the DeckLink for its attributes interface
     result = deckLink->QueryInterface(IID_IDeckLinkAttributes, (void**)&deckLinkAttributes);
     if (result != S_OK)
     {
          fprintf(stderr, "Could not obtain the IDeckLinkAttributes interface - result = %08x\n", result);
     }
     

     result = deckLinkAttributes->GetFlag(BMDDeckLinkSupportsInputFormatDetection, &supported);
     if (result == S_OK)
     {
          fprintf(stderr, " %-40s %s\n", "Input mode detection supported ?", (supported == true) ? "Yes" : "No");
     }
     else
     {
          fprintf(stderr, "Could not query the input mode detection attribute- result = %08x\n", result);
     }

     fprintf(stderr, "Supported video input connections (-i [input #]:\n  ");
     itemCount = 0;
     result = deckLinkAttributes->GetInt(BMDDeckLinkVideoInputConnections, &ports);
     if (result == S_OK)
     {
          if (ports & bmdVideoConnectionSDI)
          {
               fprintf(stderr, "%d: SDI, ", bmdVideoConnectionSDI);
               itemCount++;
          }

          if (ports & bmdVideoConnectionHDMI)
          {
               fprintf(stderr, "%d: HDMI, ", bmdVideoConnectionHDMI);
               itemCount++;
          }

          if (ports & bmdVideoConnectionOpticalSDI)
          {
               fprintf(stderr, "%d: Optical SDI, ", bmdVideoConnectionOpticalSDI);
               itemCount++;
          }

          if (ports & bmdVideoConnectionComponent)
          {
               fprintf(stderr, "%d: Component, ", bmdVideoConnectionComponent);
               itemCount++;
          }

          if (ports & bmdVideoConnectionComposite)
          {
               fprintf(stderr, "%d: Composite, ", bmdVideoConnectionComposite);
               itemCount++;
          }

          if (ports & bmdVideoConnectionSVideo)
          {
               fprintf(stderr, "%d: S-Video, ", bmdVideoConnectionSVideo);
               itemCount++;
          }
     }
     fprintf(stderr, "\n");



     delegate = new DeckLinkCaptureDelegate(glWidget);
     connect(delegate, SIGNAL(updateGLSignal()), glWidget, SLOT(updateGLSlot()));
     deckLinkInput->SetCallback(delegate);
   
     // Obtain an IDeckLinkDisplayModeIterator to enumerate the display modes supported on output
     result = deckLinkInput->GetDisplayModeIterator(&displayModeIterator);
     if (result != S_OK)
     {
          fprintf(stderr, "Could not obtain the video output display mode iterator - result = %08x\n", result);
          return -1;
     }


     if (g_videoModeIndex < 0)
     {
          fprintf(stderr, "No video mode specified\n");
          usage();
          return -1;
     }

     while (displayModeIterator->Next(&displayMode) == S_OK)
     {
          if (g_videoModeIndex == displayModeCount)
          {
               BMDDisplayModeSupport result;
               const char *displayModeName;
               
               foundDisplayMode = true;
               displayMode->GetName(&displayModeName);
               selectedDisplayMode = displayMode->GetDisplayMode();
               
               deckLinkInput->DoesSupportVideoMode(selectedDisplayMode, pixelFormat, bmdVideoInputFlagDefault, &result, NULL);

               if (result == bmdDisplayModeNotSupported)
               {
                    fprintf(stderr, "The display mode %s is not supported with the selected pixel format\n", displayModeName);
                    return -1;
               }

               if (inputFlags & bmdVideoInputDualStream3D)
               {
                    if (!(displayMode->GetFlags() & bmdDisplayModeSupports3D))
                    {
                         fprintf(stderr, "The display mode %s is not supported with 3D\n", displayModeName);
                         return -1;
                    }
               }
               fprintf(stderr, "Selecting mode: %s\n", displayModeName);
               
               break;
          }
          displayModeCount++;
          displayMode->Release();
     }

     if (!foundDisplayMode)
     {
          fprintf(stderr, "Invalid mode %d specified\n", g_videoModeIndex);
          return -1;
     }

     // Query the DeckLink for its configuration interface
     result = deckLinkInput->QueryInterface(IID_IDeckLinkConfiguration, (void**)&deckLinkConfiguration);
     if (result != S_OK)
     {
          fprintf(stderr, "Could not obtain the IDeckLinkConfiguration interface: %08x\n", result);
     }
    

     BMDVideoConnection conn;
     switch (vinput) {
     case 0:
          conn = bmdVideoConnectionSDI;
          break;
     case 1:
          conn = bmdVideoConnectionHDMI;
          break;
     case 2:
          conn = bmdVideoConnectionComponent;
          break;
     case 3:
          conn = bmdVideoConnectionComposite;
          break;
     case 4:
          conn = bmdVideoConnectionSVideo;
          break;
     case 5:
          conn = bmdVideoConnectionOpticalSDI;
          break;
     default:
          break;
     }
     conn = vinput;
     // Set the input desired
     result = deckLinkConfiguration->SetInt(bmdDeckLinkConfigVideoInputConnection, conn);
     if(result != S_OK) {
          fprintf(stderr, "Cannot set the input to [%d]\n", conn);
          return -1;
     }

     // check input
     result = deckLinkConfiguration->GetInt(bmdDeckLinkConfigVideoInputConnection, &vport);
     if (vport == bmdVideoConnectionSDI)
          fprintf(stderr, "Before Input configured for SDI\n");
     if (vport == bmdVideoConnectionHDMI)
          fprintf(stderr, "Before Input configured for HDMI\n");


     if (deckLinkConfiguration->SetFlag(bmdDeckLinkConfigFieldFlickerRemoval, flickerremoval) == S_OK) {
          fprintf(stderr, "Flicker removal set : %d\n", flickerremoval);
     }
     else {
          fprintf(stderr, "Flicker removal NOT set\n");
     }

     if (deckLinkConfiguration->SetFlag(bmdDeckLinkConfigUse1080pNotPsF, pnotpsf) == S_OK) {
          fprintf(stderr, "bmdDeckLinkConfigUse1080pNotPsF: %d\n", pnotpsf);
     }
     else {
          fprintf(stderr, "bmdDeckLinkConfigUse1080pNotPsF NOT set\n");
     }

    //if (deckLinkConfiguration->SetFlag(bmdDeckLinkConfigVideoInputConnection, conn) == S_OK) {
      //fprintf(stderr, "Input set to: %d\n", vinput);
    //}

     result = deckLinkConfiguration->GetInt(bmdDeckLinkConfigVideoInputConnection, &vport);
     if (vport == bmdVideoConnectionSDI)
          fprintf(stderr, "After Input configured for SDI\n");
     if (vport == bmdVideoConnectionHDMI)
          fprintf(stderr, "After Input configured for HDMI\n");



     result = deckLinkInput->EnableVideoInput(selectedDisplayMode, pixelFormat, inputFlags);
     if(result != S_OK)
     {
          fprintf(stderr, "Failed to enable video input. Is another application using the card?\n");
          return -1;
     }

     displayWidth = displayMode->GetWidth();
     displayHeight = displayMode->GetHeight();
     displayMode->GetFrameRate(&frameRateDuration, &frameRateScale);     
     displayFPS = (double)frameRateScale / (double)frameRateDuration;
     //set to delegate
     delegate->setWidth(displayWidth);
     delegate->setHeight(displayHeight);
     delegate->setFPS(displayFPS);
     delegate->setFrameRateDuration(frameRateDuration);
     delegate->setFrameRateScale(frameRateScale);
     //set texture width and height
     glWidget->setTextureWidth(displayWidth);
     glWidget->setTextureHeight(displayHeight);
     fprintf(stderr, "GetFrameRate: %10ld %10ld --> fps %g\n", (long)frameRateScale, (long)frameRateDuration, displayFPS);
     result = deckLinkInput->StartStreams();
     if(result != S_OK){
     	fprintf(stderr, "Cannot start streams...\n");
	return -1;
     }
     fprintf(stderr, "Finish procesing arguments \n");
}





int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //pass all the arguments to window
    //exception here ? 
    Window window(argc, argv);
    window.resize(window.sizeHint());
    int desktopArea = QApplication::desktop()->width() *
                  QApplication::desktop()->height();
    int widgetArea = window.width() * window.height();
    if (((float)widgetArea / (float)desktopArea) < 0.75f)
     window.show();
    else
     window.showMaximized();
    return app.exec();
}
