
#ifndef SYSTEM_EXCEPTION_H_INCLUDED
#define SYSTEM_EXCEPTION_H_INCLUDED

#include <exception>
#include <stdexcept>
#include <string>

#include <scm/core/platform/platform.h>
#include <scm/core/utilities/platform_warning_disable.h>

namespace scm {
namespace core {

class __scm_export(core) system_exception : public std::runtime_error
{
public:
    system_exception(const std::string&);
}; // system_exception

} // namespace core
} // namespace scm

#include <scm/core/utilities/platform_warning_enable.h>

#endif // SYSTEM_EXCEPTION_H_INCLUDED