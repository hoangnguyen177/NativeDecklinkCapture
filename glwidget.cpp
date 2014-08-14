#include <QtGui>
#include <QtOpenGL>
#include <stdio.h>
#include <png.h>
#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent)
     : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
 {
     this->buffer=NULL;
     testFileWritten = false;
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
    if(!testFileWritten){
        std::string _fileName("test.png");
        this->WritePNG(_fileName.c_str(), this->getTextureWidth(), this->getTextureHeight(), GL_RGB, GL_UNSIGNED_BYTE, this->buffer);
        testFileWritten = true;
    }
 }

 void GLWidget::initializeGL()
 {
    qglClearColor(QColor(Qt::black));
    //draw here
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    this->initDisplayList();
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
    glOrtho(0, this->getTextureWidth() , 0 , this->getTextureHeight(), 0.0 , 1.0);
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

int GLWidget::WritePNG(const char *fileName, int width, int height,
                    GLenum imageFormat, GLenum imageType,
                    GLvoid * imagePtr)
{
   FILE *fp;
   png_structp pngPtr;
   png_infop infoPtr;
   int colorType, bitDepth;
   int k;
   png_bytep rowPointers[4096];
   png_bytep rowPtr;
   int rowWidth;

   /* open the file */
   fp = fopen(fileName, "wb");
   if (fp == NULL) {
      printf("Viewperf: could not open image dump file: %s\n", fileName);
      return 1;
   }

   /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also check that
    * the library version is compatible with the one used at compile time,
    * in case we are using dynamically linked libraries.  REQUIRED.
    */
   pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
      (png_voidp) NULL, (png_error_ptr)NULL, (png_error_ptr)NULL);

   if (pngPtr == NULL)
   {
      fclose(fp);
      printf("Viewperf: PNG image creation failed\n");
      return 1;
   }

   /* Allocate/initialize the image information data.  REQUIRED */
   infoPtr = png_create_info_struct(pngPtr);
   if (infoPtr == NULL)
   {
      fclose(fp);
      png_destroy_write_struct(&pngPtr,  (png_infopp)NULL);
      printf("Viewperf: PNG image information data creation failed\n");
      return 1;
   }

// MWC: 2014-06-10: This section of code problematic on newer versions of Linux
// if (setjmp(pngPtr->jmpbuf)) {
//    /* If we get here, we had a problem reading the file */
//    fclose(fp);
//    png_destroy_write_struct(&pngPtr, (png_infopp)NULL);
//    printf("Viewperf: PNG write error occurred\n");
//    return 1;
// }

   /* set up the output control if you are using standard C streams */
   png_init_io(pngPtr, fp);

   /* Set the image information here.  Width and height are up to 2^31,
    * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
    * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
    * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
    * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
    * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
    * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
    */
   switch (imageFormat) {
     case GL_COLOR_INDEX:
     case GL_LUMINANCE:
    colorType = PNG_COLOR_TYPE_GRAY;
    rowWidth = 1;
    break;
     case GL_LUMINANCE_ALPHA:
    colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
    rowWidth = 2;
    break;
     case GL_RGB:
    colorType = PNG_COLOR_TYPE_RGB;
    rowWidth = 3;
    break;
     case GL_RGBA:
    colorType = PNG_COLOR_TYPE_RGB_ALPHA;
    rowWidth = 4;
    break;
     default:
        printf("Viewperf: internal error, unknown imageFormat found on dumping\n");
    fflush(stdout);
    exit(1);
   }
   if (imageType == GL_UNSIGNED_SHORT) {
     bitDepth = 16;
     rowWidth *= 2;
   } else {
     bitDepth = 8;
     rowWidth *= 1;
   }

   png_set_IHDR(pngPtr, infoPtr, width, height, bitDepth, colorType,
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

   /* Optional gamma chunk is strongly suggested if you have any guess
    * as to the correct gamma of the image.
    */
   //png_set_gAMA(pngPtr, infoPtr, GAMMA);

   /* Write the file header information.  REQUIRED */
   png_write_info(pngPtr, infoPtr);

   /* set row pointers in reverse order */
   rowWidth *= width;
   for (k = 0; k < height; k++) {
     rowPointers[k] = (png_bytep)imagePtr + (height-k-1)*rowWidth;
   }

   /* One of the following output methods is REQUIRED */
   png_write_image(pngPtr, rowPointers);

   /* It is REQUIRED to call this to finish writing the rest of the file */
   png_write_end(pngPtr, infoPtr);

   /* clean up after the write, and free any memory allocated */
   png_destroy_write_struct(&pngPtr, (png_infopp)NULL);

   /* close the file */
   fclose(fp);

   /* that's it */
   return 0;
}

