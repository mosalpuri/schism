
#ifndef GL_FONT_FACE_LOADER_H_INCLUDED
#define GL_FONT_FACE_LOADER_H_INCLUDED

#include <scm/gl/font/face.h>
#include <scm/core/font/face_loader.h>

#include <scm/core/platform/platform.h>
#include <scm/core/utilities/platform_warning_disable.h>

namespace scm {
namespace gl {

class __scm_export(ogl) face_loader : protected scm::font::face_loader
{
public:
    face_loader();
    virtual ~face_loader();

    using font::face_loader::set_font_resource_path;

    scm::gl::face_ptr   load(const std::string& /*file_name*/,
                             unsigned           /*size*/     = 12,
                             unsigned           /*disp_res*/ = 72);

protected:
    using font::face_loader::load;

}; // class font_resource_loader

} // namespace gl
} // namespace scm

#include <scm/core/utilities/platform_warning_enable.h>

#endif //GL_FONT_FACE_LOADER_H_INCLUDED