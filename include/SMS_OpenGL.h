//This header includes the OpenGL headers and defines some macros
#ifndef __SMS_OPENGL_H__
#define __SMS_OPENGL_H__

#include "SMS_Config.h"

//TODO: add support for other OSes

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef SMS_DEBUG
#define GLCheck(x) x; GLCheckError(__FILE__, __LINE__)
#else
#define GLCheck(x) x
#endif

#include <string>
#include <iostream>
inline void GLCheckError(const std::string& file, int line) {
    GLenum error;

    while ((error = glGetError()) != GL_NO_ERROR) {

        std::string errorString = "UNKNOWN";
        switch (error) {
        case GL_INVALID_ENUM:
            errorString = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errorString = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            errorString = "GL_INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            errorString = "GL_STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            errorString = "GL_STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            errorString = "GL_OUT_OF_MEMORY";
            break;
        case GL_TABLE_TOO_LARGE:
            errorString = "GL_TABLE_TOO_LARGE";
            break;
        }

        std::cerr << file << "(" << line << ") : " << errorString << std::endl;
    }
}

#endif//__SMS_OPENGL_H__
