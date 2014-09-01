#if defined(GLSL_YUV)      
  #include "sageShader.h"
#else
  #include <QtOpenGL>
#endif

#include <QtGui>
#include <stdio.h>
#include <png.h>
#include "glwidget.h"

//////////////////////////////////////////////////////////////////////////////
#if defined(GLSL_YUV)       
  GLhandleARB GLWidget::programHandleYUV = 0;
#endif

////////////////////////////////////////////////////////////////////////////////

static const GLchar *yuv_fragment = "                                           \
uniform sampler2D yuvtex;                                                       \
uniform float texel_width;                                                      \
uniform float texture_width;                                                    \
void main()                                                                     \
{                                                                               \
  float red, green, blue;                                                       \
  vec4 luma_chroma;                                                             \
  float luma, chroma_u,  chroma_v;                                              \
  float pixelx, pixely;                                                         \
  float xcoord, ycoord;                                                         \
  pixelx = gl_TexCoord[0].x;                                                    \
  pixely = gl_TexCoord[0].y;                                                    \
  xcoord = floor (pixelx * texture_width);                                      \
  luma_chroma = texture2D(yuvtex, vec2(pixelx, pixely));                        \
  if (0.0 == mod(xcoord , 2.0))                                          \
  {                                                                             \
    chroma_u = luma_chroma.r;                                                   \
    chroma_v = texture2D(yuvtex, vec2(pixelx + texel_width, pixely)).r;         \
  }                                                                             \
  else                                                                   \
  {                                                                             \
    chroma_v = luma_chroma.r;                                                   \
    chroma_u = texture2D(yuvtex, vec2(pixelx - texel_width, pixely)).r;         \
  }                                                                             \
  luma = 1.1640625 * (luma_chroma.a - 0.0625);                                     \
  chroma_u = chroma_u - 0.5;                                                    \
  chroma_v = chroma_v - 0.5;                                                    \
  red = luma + 1.59765625 * chroma_v;                                               \
  green = luma - 0.390625 * chroma_u - 0.8125 * chroma_v;                       \
  blue = luma + 2.015625 * chroma_u;                                               \
  gl_FragColor = vec4(red, green, blue, 1.0);                                   \
}";




#if !defined(GLSL_YUV)

inline unsigned char Clamp(int value)
{
  if(value > 255) return 255;
  if(value < 0) return 0;
  return value;
}

void GLWidget::createTableLookup(){
   int yy, uu, vv, ug_plus_vg, ub, vr, val;
  // Red
  for (int y = 0; y < 256; y++) {
    for (int v = 0; v < 256; v++) {
      yy = y << 8;
      vv = v - 128;
      vr = vv * 359;
      val = (yy + vr) >> 8;
      red[y][v] = Clamp(val);
    }
  }
  // Blue
  for (int y = 0; y < 256; y++) {
    for (int u = 0; u < 256; u++) {
      yy = y << 8;
      uu = u - 128;
      ub = uu * 454;
      val = (yy + ub) >> 8;
      blue[y][u] = Clamp(val);
    }
  }
  // Green
  for (int y = 0; y < 256; y++) {
    for (int u = 0; u < 256; u++) {
      for (int v = 0; v < 256; v++) {
      	yy = y << 8;
      	uu = u - 128;
      	vv = v - 128;
      	ug_plus_vg = uu * 88 + vv * 183;
      	val = (yy - ug_plus_vg) >> 8;
      	green[y][u][v] = Clamp(val);
      }
    }
  }
  
}
#endif

GLWidget::GLWidget(QWidget *parent)
     : QGLWidget(QGLFormat(QGL::DoubleBuffer), parent) //SampleBuffers::DoubleBuffer
 {
    fprintf(stderr, "initing GLWIDGET\n");	

#if !defined(GLSL_YUV)
    this->createTableLookup();
#endif
    this->buffer=NULL;
    this->texture_buffer = NULL;
    testFileWritten = false;
    this->bpp = 2;
    this->readyToReceiveNewFrame = false;
    this->bufferReady = false;
 }

#if defined(GLSL_YUV)      
 void GLWidget::initShaderProgram(){
    GLenum err = glewInit();
    if(GLEW_OK!=err){
       fprintf(stderr, "Failed to init glew. exit\n");
       exit(1);
    }
    if (programHandleYUV==0) {
    	//fprintf(stderr, "initing gl shader\n");	 
      programHandleYUV = GLSLinstallFragmentShader(yuv_fragment);
      glActiveTexture(GL_TEXTURE0);
      int h=glGetUniformLocation(GLWidget::programHandleYUV,"yuvtex");
      glUniform1i(h, 0);
      int texture_width_location, texel_width_location;
      //texture withd
      texture_width_location =  glGetUniformLocation(GLWidget::programHandleYUV, "texture_width");
      //fprintf(stderr, "texture width location:%d\n", texture_width_location);
      if (-1 == texture_width_location)
        fprintf(stderr, "Warning: can't get texture_width location\n");
      else
        glUniform1f(texture_width_location, (float)this->getTextureWidth());
      //texel width
      texel_width_location =  glGetUniformLocation(GLWidget::programHandleYUV, "texel_width");
      //fprintf(stderr, "texel_width_location:%d\n", texel_width_location);
      if (-1 == texel_width_location)
        fprintf(stderr, "Warning: can't get texel_width location\n");
      else
        glUniform1f(texel_width_location, (1.0 / (float)this->getTextureWidth())); 
    	//fprintf(stderr, "done initing gl shader %d\n", programHandleYUV);	 
    }
 }
#endif
 GLWidget::~GLWidget()
 {
  if(this->buffer)
    free(this->buffer);
  this->deleteTexture();
 }

 QSize GLWidget::minimumSizeHint() const
 {
     return QSize(960, 540);
 }

 QSize GLWidget::sizeHint() const
 {
     return QSize(1920, 1080);
 }

void GLWidget::updateGLSlot(){
  //fprintf(stderr, "updateGLSlot\n");
  if(this->buffer ==NULL)
    return;
  if(this->texture_buffer == NULL)
    return;
#define clip(A) ( ((A)<0) ? 0 : ( ((A)>255) ? 255 : (A) ) )

#define YUV444toRGB888(Y,U,V,R,G,B)					\
    R = clip(( 298 * (Y-16)                 + 409 * (V-128) + 128) >> 8); \
    G = clip(( 298 * (Y-16) - 100 * (U-128) - 208 * (V-128) + 128) >> 8); \
    B = clip(( 298 * (Y-16) + 516 * (U-128)                 + 128) >> 8);

while(this->bufferReady ==false);
    
this->mutex_lock.lock();
this->readyToReceiveNewFrame = false;

  
#if defined(GLSL_YUV)    
  glBindTexture(GL_TEXTURE_2D, this->texture);
  glTexImage2D(GL_TEXTURE_2D , 0, GL_LUMINANCE_ALPHA, this->getTextureWidth(), this->getTextureHeight(), 
                               0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, this->buffer);
  this->readyToReceiveNewFrame = true;
  this->mutex_lock.unlock();
#else
    /*
    unsigned char *yuv = buffer;
    unsigned char u,v,y1,y2;
    unsigned char r1,r2,g1,g2,b1,b2;
    int i, k;
    k = 0;
    for (i=0;i< (this->getTextureWidth()*this->getTextureHeight())/2;i++) {
	u  = yuv[4*i+0];
	y1 = yuv[4*i+1];
	v  = yuv[4*i+2];
	y2 = yuv[4*i+3];
    
	YUV444toRGB888(y1, u, v, r1,g1,b1);
	YUV444toRGB888(y2, u, v, r2,g2,b2);
	texture_buffer[k + 0] = r1;
	texture_buffer[k + 1] = g1;
	texture_buffer[k + 2] = b1;
	texture_buffer[k + 3] = r2;
	texture_buffer[k + 4] = g2;
	texture_buffer[k + 5] = b2;
	k += 6;
    }*/
//       
    unsigned int boundry = this->getTextureWidth()* this->getTextureHeight() * bpp;
    unsigned char y, u, v;
    unsigned char * yuv = this->buffer;
    unsigned int j=0;
    //#pragma omp for
    for(unsigned int i=0; i<boundry; i+=4, j+=6){
      y = yuv[i+1];
      u = yuv[i];
      v = yuv[i+2];
      texture_buffer[j] = red[y][v];
      texture_buffer[j+1] = green[y][u][v];
      texture_buffer[j+2] = blue[y][u];
      y = yuv[i+3];
      texture_buffer[j+3] = red[y][v];
      texture_buffer[j+4] = green[y][u][v];
      texture_buffer[j+5] = blue[y][u];
    }
    this->readyToReceiveNewFrame = true;
    this->mutex_lock.unlock();
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->getTextureWidth(), this->getTextureHeight(),
		    GL_RGB, GL_UNSIGNED_BYTE, texture_buffer);  
#endif    
    //this->updateGL();
	this->update();
}

 void GLWidget::setBuffer(unsigned char * _buffer){
    if(_buffer == NULL){
      fprintf(stderr, "NULL return \n");
      return;	
    }
    this->mutex_lock.lock();
    this->bufferReady = false;
    memcpy(this->buffer, _buffer, this->getTextureWidth() * this->getTextureHeight() * bpp);
    this->bufferReady = true;
    this->mutex_lock.unlock();
 }

 void GLWidget::initializeGL()
 {
    qglClearColor(QColor(Qt::black));
    //draw here
    /*this->initDisplayList();*/
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    this->renewTexture();
#if defined(GLSL_YUV)      
    this->initShaderProgram();
#endif
 }

void GLWidget::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();    
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  //draw quad
  //drawQuad(depth, alpha, tempAlpha, left, right, bottom, top);
  glBegin(GL_QUADS);
     //glColor3f(1, 1, 1);
     glTexCoord2d(0, 1);
     glVertex3i( 0, 0, 0);

     glTexCoord2d(1 , 1);
     glVertex3i(this->getTextureWidth(), 0, 0);
     
     glTexCoord2d(1, 0);
     glVertex3i(this->getTextureWidth(), this->getTextureHeight(), 0);
     
     glTexCoord2d(0, 0);
     glVertex3i(0, this->getTextureHeight(), 0);
     
  glEnd();
  this->swapBuffers();
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
}


void GLWidget::initDisplayList()
{
     //std::cout<<"[start initDisplayList@GLWidget]" <<std::endl;
     fprintf(stderr, "[GLWidget::initDisplayList] texture width: %d height:%d\n", this->getTextureWidth(), this->getTextureHeight());
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


void GLWidget::deleteTexture()
{
    if (texture >= 0) {
      glDeleteTextures(1, &texture);
      if (this->texture_buffer)
        free(this->texture_buffer);
    }
}

void GLWidget::initBuffer(){
  fprintf(stderr, "buffer initialized\n");
  this->buffer = (unsigned char *) malloc(this->getTextureWidth() * this->getTextureHeight() * bpp);
  this->readyToReceiveNewFrame = true;
}

/**
* renew texture
*/
void GLWidget::renewTexture(){
  int newWidth, newHeight;
  newWidth = this->getTextureWidth(); // getMax2n(texInfo.width);
  newHeight = this->getTextureHeight(); // getMax2n(texInfo.height);

  int maxt;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&maxt);
  if (newWidth > maxt)  {
    fprintf(stderr, "Warning: trying to create texture %d, bigger than maximun %d\n", newWidth, maxt);
    newWidth = maxt;
  }
  if (newHeight > maxt) {
    fprintf(stderr, "Warning: trying to create texture %d, bigger than maximun %d\n", newHeight, maxt);
    newHeight = maxt;
  }

  /*if (newWidth <= this->getTextureWidth() && newHeight <= this->getTextureHeight()) {
    fprintf(stderr,"exit here................\n");
    return;
  }*/

  this->setTextureWidth(newWidth);
  this->setTextureHeight(newHeight);

  deleteTexture();
  GLuint handle;      
  glGenTextures(1, &handle);
  texture = handle;

  // Create GL texture object
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	
  this->texture_buffer = (GLubyte *) malloc(this->getTextureWidth() * this->getTextureHeight() * bpp);
  memset(this->texture_buffer, 0, this->getTextureWidth() * this->getTextureHeight() * bpp);
  
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, handle);

 #if defined(GLSL_YUV)  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
  glTexImage2D(GL_TEXTURE_2D , 0, GL_LUMINANCE_ALPHA, this->getTextureWidth(), this->getTextureHeight(), 
                               0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, this->buffer); 
#else
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, handle);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->getTextureWidth(), this->getTextureHeight(), 0, 
                                  GL_RGB, GL_UNSIGNED_BYTE, this->texture_buffer);
#endif
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

