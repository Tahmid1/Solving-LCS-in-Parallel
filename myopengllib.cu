// A class for openGL animation of CUDA applicatins 
// 
// (newhall, 2011)
//
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include "cuda.h"
#include "cuda_gl_interop.h"


#include "myopengllib.h"
#include <string.h>


GPUDisplayData  *GPUDisplayData::gpu_disp = 0;

PFNGLBINDBUFFERARBPROC    glBindBuffer     = NULL;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffers  = NULL;
PFNGLGENBUFFERSARBPROC    glGenBuffers     = NULL;
PFNGLBUFFERDATAARBPROC    glBufferData     = NULL;


// the constructor takes dimensions of the openGL graphics display 
// object to create, and a pointer to a struct containing ptrs 
// to application-specific CUDA data that the display function
// needs in order to change bitmap values in the openGL object
GPUDisplayData::GPUDisplayData(int w, int h, void *data, 
    const char *winname ="Animation") 
{

  width = w;
  height = h;
  gpu_data = data;
  animate_function = 0;
  exit_function = 0;

  // find a CUDA device and set it to graphic interoperable
  cudaDeviceProp  prop;
  int dev;
  memset( &prop, 0, sizeof( cudaDeviceProp ) );
  prop.major = 1;
  prop.minor = 0;
  HANDLE_ERROR( cudaChooseDevice( &dev, &prop ), "choose cuda dev" );
  cudaGLSetGLDevice( dev );

  // init glut
  int argc = 1;   // bogus args for glutInit 
  char *argv = "";
  glutInit(&argc, &argv);
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA ); 
  glutInitWindowSize( width, height ); 
  glutCreateWindow( winname );

  // glXGetProcAddress - obtain a pointer to an OpenGL or GLX function
  // This is necessary in environments where the OpenGL link library 
  // exports a different set of functions than the runtime library.
  // (I'm not sure we really need to do this, but we are rather than
  // just calling these functions first)
  glBindBuffer    = (PFNGLBINDBUFFERARBPROC)
    glXGetProcAddress((const GLubyte *)"glBindBuffer");
  glDeleteBuffers = (PFNGLDELETEBUFFERSARBPROC)
    glXGetProcAddress((const GLubyte *)"glDeleteBuffers");
  glGenBuffers    = (PFNGLGENBUFFERSARBPROC)
    glXGetProcAddress((const GLubyte *)"glGenBuffers");
  glBufferData    = (PFNGLBUFFERDATAARBPROC)
    glXGetProcAddress((const GLubyte *)"glBufferData");

  // create an OpenGL buffer
  glGenBuffers( 1, &bufferObj );
  glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, bufferObj );
  glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, width * height * 4, 
      NULL, GL_DYNAMIC_DRAW_ARB );

  // register buffers for CUDA: 
  HANDLE_ERROR( cudaGraphicsGLRegisterBuffer( &resource, bufferObj, 
        cudaGraphicsMapFlagsNone ), "GPUDisplayData::cudaGraphicsGLRegister" );
  gpu_disp = this;
}


GPUDisplayData::~GPUDisplayData() {

  HANDLE_ERROR( cudaGraphicsUnregisterResource( resource ), "~GPUDisplayData" );
  glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
  glDeleteBuffers( 1, &bufferObj );
          
}


void GPUDisplayData::AnimateComputation( void (*anim_func)(uchar4 *, void *)) {

  animate_function = anim_func;;

   // add callbacks on openGL events
   glutIdleFunc(animate);
   glutDisplayFunc(animate);
   atexit(clean_up);   // register function to clean up state on exit
   
   // call glut mainloop
   glutMainLoop();

}

void GPUDisplayData::AnimateComputation( void (*anim_func)(uchar4 *, void *), 
    void (*exit_func)(void)) 
{
 
  animate_function = anim_func;;
  exit_function = exit_func;

   // add callbacks on openGL events
   glutIdleFunc(animate);
   glutDisplayFunc(animate);
   atexit(clean_up);   // register function to clean up state on exit
   
   // call glut mainloop
   glutMainLoop();
}

// cleanup function for call to atexit,
// 
void GPUDisplayData::clean_up(void) {

  GPUDisplayData *obj = GPUDisplayData::get_gpu_obj();
  // unregister openGL buffer with cuda and free it
  HANDLE_ERROR( cudaGraphicsUnregisterResource(obj->resource),
      "cudaGraphicsUnregisterResource"); 
  glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 ); 
  glDeleteBuffers( 1, &obj->bufferObj );
  if(obj->exit_function) obj->exit_function();
}

// generic animate function registered with glutDisplayFunc
// it makes call to the application-specific animate function
void GPUDisplayData::animate(void) {
  uchar4 *devPtr;
  size_t size;
  GPUDisplayData *obj = GPUDisplayData::get_gpu_obj();


  HANDLE_ERROR( cudaGraphicsMapResources( 1, &obj->resource, NULL ), 
      "cudaGraphicsMapResources") ;
  HANDLE_ERROR( cudaGraphicsResourceGetMappedPointer( (void**)&devPtr, 
        &size, obj->resource), "cudaGraphicsResourceGetMappedPointer" );

  if(obj->animate_function) { 
    obj->animate_function(devPtr, obj->gpu_data);
  }
  HANDLE_ERROR( cudaGraphicsUnmapResources( 1, &obj->resource, NULL ),
      "cudaGraphicsUnmapResources");

  glClearColor( 0.0, 0.0, 0.0, 1.0 );
  glClear( GL_COLOR_BUFFER_BIT );
  glDrawPixels( obj->height, obj->width, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
  glutSwapBuffers();

}
