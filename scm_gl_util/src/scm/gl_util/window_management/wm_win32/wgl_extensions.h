
#ifndef SCM_GL_UTIL_OPENGL_WGL_H_INCLUDED
#define SCM_GL_UTIL_OPENGL_WGL_H_INCLUDED

#include <scm/core/platform/platform.h>

#if SCM_PLATFORM == SCM_PLATFORM_WINDOWS

#include <set>
#include <string>

#include <scm/core/platform/windows.h>
#include <GL/GL.h>
#include <scm/gl_util/window_management/GL/wglext.h>

namespace scm {
namespace gl {
namespace wm {
namespace util {

class wgl_extensions
{
private:
    typedef std::set<std::string> string_set;

public:
    // WGL_ARB_create_context
    PFNWGLCREATECONTEXTATTRIBSARBPROC       wglCreateContextAttribsARB;
    
    // WGL_ARB_pixel_format
    PFNWGLGETPIXELFORMATATTRIBIVARBPROC     wglGetPixelFormatAttribivARB;
    PFNWGLGETPIXELFORMATATTRIBFVARBPROC     wglGetPixelFormatAttribfvARB;
    PFNWGLCHOOSEPIXELFORMATARBPROC          wglChoosePixelFormatARB;

    // WGL_ARB_extensions_string
    PFNWGLGETEXTENSIONSSTRINGARBPROC        wglGetExtensionsStringARB;

    // ARB_framebuffer_sRGB

    // WGL_ARB_multisample

    // WGL_EXT_swap_control
    PFNWGLSWAPINTERVALEXTPROC               wglSwapIntervalEXT;
    PFNWGLGETSWAPINTERVALEXTPROC            wglGetSwapIntervalEXT;

    // WGL_ARB_pbuffer
    PFNWGLCREATEPBUFFERARBPROC              wglCreatePbufferARB;
    PFNWGLGETPBUFFERDCARBPROC               wglGetPbufferDCARB;
    PFNWGLRELEASEPBUFFERDCARBPROC           wglReleasePbufferDCARB;
    PFNWGLDESTROYPBUFFERARBPROC             wglDestroyPbufferARB;
    PFNWGLQUERYPBUFFERARBPROC               wglQueryPbufferARB;

public:
    wgl_extensions();

    bool            initialize();
    bool            is_initialized() const;
    bool            is_supported(const std::string& ext) const;

private:
    string_set      _wgl_extensions;
    bool            _initialized;

}; // class wgl_extensions

} // namespace util
} // namespace wm
} // namepspace gl
} // namepspace scm


#endif // SCM_PLATFORM == SCM_PLATFORM_WINDOWS
#endif // SCM_GL_UTIL_OPENGL_WGL_H_INCLUDED
