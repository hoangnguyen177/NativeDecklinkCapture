#ifndef UIWIDGET_H
#define UIWIDGET_H
/*QT*/
//#include <QtOpenGL>
#include <QGLWidget>
#include <QMutex>

class GLWidget : public QGLWidget
{
 Q_OBJECT


public:
#if defined(GLSL_YUV)        
  static GLhandleARB programHandleYUV;    
#endif
  GLWidget(QWidget *parent = 0);
 ~GLWidget();

 QSize minimumSizeHint() const;
 QSize sizeHint() const;
 void setBuffer(unsigned char * _buffer);
 unsigned char * getBuffer()			{ return buffer;}

 void setTextureWidth(int _tWidth)		{ textureWidth = _tWidth;	}
 void setTextureHeight(int _tHeight)		{ textureHeight = _tHeight;	}
 int getTextureWidth()				{ return textureWidth;		}
 int getTextureHeight()				{ return textureHeight;		}

 GLuint getDisplayList();
 void setDisplayList(GLuint _displayList);
 GLuint getTexture();
 void setTexture(GLuint _texture);

 void initBuffer(); //init the buffer, called after setting texture width and height

#if defined(GLSL_YUV)       
 void initShaderProgram();
#endif

 int WritePNG(const char *fileName, int width, int height, GLenum imageFormat, GLenum imageType, GLvoid * imagePtr);

 bool isReadyToReceiveNewFrame()		{	return this->readyToReceiveNewFrame;	}


public slots:
 void updateGLSlot();
protected:
 void initializeGL();
 void paintGL();
 void resizeGL(int width, int height);
 void initDisplayList();

 void renewTexture();
 void deleteTexture();
#if !defined(GLSL_YUV)
  void createTableLookup();
  unsigned char red[256][256];
  unsigned char blue[256][256];
  unsigned char green[256][256][256];
#endif
private:
 GLuint displayList;
 GLuint texture;
 unsigned char *buffer;
 GLubyte* texture_buffer;
 int textureWidth;
 int textureHeight;
 bool testFileWritten;

 bool readyToReceiveNewFrame;
 bool bufferReady;
 
 double   bpp;
 QMutex mutex_lock;

};



#endif // UIWIDGET_H
