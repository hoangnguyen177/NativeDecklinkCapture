#ifndef UIWIDGET_H
#define UIWIDGET_H
/*QT*/
#include <QtOpenGL>
#include <QGLWidget>


class GLWidget : public QGLWidget
{
 Q_OBJECT

public:
 GLWidget(QWidget *parent = 0);
 ~GLWidget();

 QSize minimumSizeHint() const;
 QSize sizeHint() const;

 void setBuffer(GLubyte* _buffer)			{ buffer = _buffer;			}
 GLubyte* getBuffer()						{ return buffer;			}

 void setTextureWidth(int _tWidth)			{ textureWidth = _tWidth;	}
 void setTextureHeight(int _tHeight)		{ textureHeight = _tHeight;	}
 void getTextureWidth()						{ return textureWidth;		}
 void getTextureHeight()					{ return textureHeight;		}


protected:
 void initializeGL();
 void paintGL();
 void resizeGL(int width, int height);
 void initDisplayList();
private:
 GLuint displayList;
 GLuint texture;
 GLubyte* buffer;
 int textureWidth;
 int textureHeight;


};



#endif // UIWIDGET_H