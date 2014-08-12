#include <QtGui>
#include <QtOpenGL>
#include <stdio.h>
#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent)
     : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
 {
     this->initDisplayList();
     this->buffer=NULL;
 }

 GLWidget::~GLWidget()
 {

 }

 QSize GLWidget::minimumSizeHint() const
 {
     return QSize(800, 600);
 }

 QSize GLWidget::sizeHint() const
 {
     return QSize(1600, 1200);
 }

void GLWidget::updateGLSlot(){
 this->updateGL();
}

 void GLWidget::setBuffer(GLubyte* _buffer){
    fprintf(stderr, "setting buffer \n");	  
    buffer = _buffer; 
 }

 void GLWidget::initializeGL()
 {
    qglClearColor(QColor(Qt::black));
    //draw here
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
 }

void GLWidget::paintGL()
{
    fprintf(stderr, "paintGL texturewidth:%d textureheight: %d\n", this->getTextureWidth(), this->getTextureHeight());
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //do painting
    glPushMatrix();
    glCallList(this->getDisplayList());
    if(this->buffer!=NULL)
     glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, this->getTextureWidth(), this->getTextureHeight(), 0,GL_RGB, GL_UNSIGNED_BYTE, this->buffer); 

    glPopMatrix();
    glFlush();

}

void GLWidget::resizeGL(int w, int h)
{
    int width = w;
    int height = (h>0)?h:1;
    glViewport(0,0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, this->width() , 0 , this->height(), -1.0 , 1.0);
    glMatrixMode(GL_MODELVIEW);

     /*int side = qMin(width, height);
     glViewport((width - side) / 2, (height - side) / 2, side, side);

     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
 #ifdef QT_OPENGL_ES_1
     glOrthof(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
 #else
     glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
 #endif
     glMatrixMode(GL_MODELVIEW);*/
}


void GLWidget::initDisplayList()
{
     //std::cout<<"[start initDisplayList@GLWidget]" <<std::endl;
     /***creating texture **********/
     glEnable(GL_TEXTURE_2D);
     //get the texture
     glGenTextures(1, &this->texture);

     GLuint dList = glGenLists(1);
     glNewList(dList, GL_COMPILE);
     //    glColor3f(1, 1, 1);

     //bind the texture
     glBindTexture(GL_TEXTURE_2D, this->texture);
     /* Setup some parameters for texture filters */
     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
     //    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);  // how to set texture coords
     //    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
     //init texture

     //now openGL has its own copy of the texture, do not need dxt any more
     //now draw the outside frame
     glBegin(GL_QUADS);
     glColor3f(1, 1, 1);
     glTexCoord2d(0, 1);
     glVertex3i( 0, 0, 0);

     glTexCoord2d(1 , 1);
     glVertex3i(this->getTextureWidth(), 0, 0);

     glTexCoord2d(1, 0);
     glVertex3i(this->getTextureWidth(), this->getTextureHeight(), 0);

     glTexCoord2d(0, 0);
     glVertex3i(0, this->getTextureHeight(), 0);

     glEnd();
     glEndList();

     this->setDisplayList(dList);     
}


/**
  * get and set display list
  */
GLuint GLWidget::getDisplayList()
{
     return this->displayList;
}

void GLWidget::setDisplayList(GLuint dList)
{
     this->displayList = dList;
}

/**
  * get and set texture
  */
GLuint GLWidget::getTexture()
{
     return this->texture;
}

void GLWidget::setTexture(GLuint t)
{
     this->texture = t;
}
