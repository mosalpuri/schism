

#ifndef OPENGL_SYSTEM_H_INCLUDED
#define OPENGL_SYSTEM_H_INCLUDED

#include <string>

#include <scm/core/utilities/boost_warning_disable.h>
#include <scm/core/utilities/boost_warning_enable.h>

#include <scm/core/sys_interfaces.h>

#include <scm/core/platform/platform.h>
#include <scm/core/utilities/platform_warning_disable.h>

namespace scm {
namespace gl {

class __scm_export(ogl) opengl_system : public scm::core::system
{
public:

public:
    opengl_system();
    virtual ~opengl_system();
    
    // core::system interface
    bool                    initialize();
    bool                    shutdown();

    bool                    is_supported(const std::string& /*ext*/) const;

protected:

private:

}; // class opengl_system

} // namespace gl
} // namespace scm

#include <scm/core/utilities/platform_warning_enable.h>

#endif // OPENGL_SYSTEM_H_INCLUDED