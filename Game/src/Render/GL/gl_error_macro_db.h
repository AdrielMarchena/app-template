#pragma once
#include "Log/Log.h"
#include "Core/Assert.h"
#include "glad/glad.h"

#ifndef GL_DEBUG_STUFF
#define GL_DEBUG_STUFF

#include "Core/Core.h"
/* this function can be found here:
    https://stackoverflow.com/questions/18319338/opengl-es-error-to-string
*/
static char const* gl_error_string(GLenum const err) noexcept
{
    switch (err)
    {
    case GL_NO_ERROR:
        return "GL_NO_ERROR";

    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";

    //case GL_STACK_OVERFLOW:
    //    return "GL_STACK_OVERFLOW";
    //
    //case GL_STACK_UNDERFLOW:
    //    return "GL_STACK_UNDERFLOW";

    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";

   // case GL_TABLE_TOO_LARGE:
   //     return "GL_TABLE_TOO_LARGE";

        // opengl 3 errors (1)
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";

        // gles 2, 3 and gl 4 error are handled by the switch above
    default:
        GAME_LOG_WARN("unknown GL error");
        return nullptr;
    }
}

/* All things below are from 'The Cherno' youtube Channel */

static void GLClearError()
{
    static GLenum err;
    uint8_t wd = 32;
    while ((err = glGetError()) != GL_NO_ERROR && wd--) 
    {
    }
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        GAME_LOG_ERROR("[OpenGL error]: {0}, {1}): {2} {3}: {4}", error, gl_error_string(error), function, file, line);
        return false;
    }
    return true;
}

#if defined(GAME_DEBUG) //|| defined(GAME_RELEASE)
#define GLCall(x) GLClearError();\
     x;\
     GAME_CORE_ASSERT(GLLogCall(#x,__FILE__,__LINE__),"GL_Error")
#else
#define GLCall(x) x
#endif
#endif