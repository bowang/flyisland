#ifndef DEFINE_H
#define DEFINE_H

#define CONFIG_FILENAME "./config.ini"
#define BUFFER_SIZE 1000
#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 768
#define Pi 3.1415926f
#define EPSILON 0.00001

//#define DPRINT
#ifdef DPRINT
    #define PRINTF(...) printf(__VA_ARGS__)
#else
    #define PRINTF(...) 
#endif

// #undef _DEBUG
// #if _DEBUG

#define DEBUG
#ifdef DEBUG
#define GL_CHECK(x) {\
  (x);\
  GLenum error = glGetError();\
  if (GL_NO_ERROR != error) {\
      printf("GL_CHECK [%s ln%d]: %s\n", __FILE__, __LINE__ ,gluErrorString(error));\
  }\
}
#else
#define GL_CHECK(x) {\
  (x);\
}
#endif


#endif
