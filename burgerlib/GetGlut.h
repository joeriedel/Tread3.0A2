#ifndef __GETGLUT_H__
#define __GETGLUT_H__

#if __CF_USE_FRAMEWORK_INCLUDES__
#include <OpenGL/glu.h>		/* Open GL for MacOS X */
#else
#if defined(__WIN95__)
#include <windows.h>			/* Needed before gl in win95 */
#endif
#include <glut.h>				/* OpenGL for all other platforms */
#endif
#endif
