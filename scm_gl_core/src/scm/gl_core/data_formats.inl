
#ifndef SCM_GL_CORE_DATA_FORMATS_INL_INCLUDED
#define SCM_GL_CORE_DATA_FORMATS_INL_INCLUDED

#include <cassert>

namespace scm {
namespace gl {

inline
bool is_normalized(data_format d)
{
    assert(FORMAT_NULL <= d && d < FORMAT_COUNT);
    return (FORMAT_R_8 <= d && d <= FORMAT_SRGBA_8);
}

inline
bool is_unnormalized(data_format d)
{
    assert(FORMAT_NULL <= d && d < FORMAT_COUNT);
    return (FORMAT_R_8I <= d && d <= FORMAT_RGBA_32UI);
}

inline
bool is_integer_type(data_format d)
{
    assert(FORMAT_NULL <= d && d < FORMAT_COUNT);
    return (FORMAT_R_8 <= d && d <= FORMAT_RGBA_32UI);
}

inline
bool is_float_type(data_format d)
{
    assert(FORMAT_NULL <= d && d < FORMAT_COUNT);
    return (   FORMAT_R_16F <= d && d <= FORMAT_R11B11G10F
            || FORMAT_D32F == d
            || FORMAT_D32F_S8 == d);
}

inline
bool is_depth_format(data_format d)
{
    assert(FORMAT_NULL <= d && d < FORMAT_COUNT);
    return (FORMAT_D16 <= d && d <= FORMAT_D32F_S8);
}

inline
bool is_stencil_format(data_format d)
{
    assert(FORMAT_NULL <= d && d < FORMAT_COUNT);
    return (FORMAT_D24_S8 <= d && d <= FORMAT_D32F_S8);
}

inline
bool is_packed_format(data_format d)
{
    assert(FORMAT_NULL <= d && d < FORMAT_COUNT);
    return (FORMAT_RGB9_E5 <= d && d <= FORMAT_R11B11G10F);
}

inline
bool is_plain_format(data_format d)
{
    assert(FORMAT_NULL <= d && d < FORMAT_COUNT);
    return (d <= FORMAT_RGBA_32F);
}

inline
bool is_srgb_format(data_format d)
{
    assert(FORMAT_NULL <= d && d < FORMAT_COUNT);
    return (FORMAT_SRGB_8 <= d && d <= FORMAT_SRGBA_8);
}

inline
int channel_count(data_format d)
{
    static int channel_counts[] = {
        0,
        // normalized integer formats (NORM)
        1, 2, 3, 4,
        1, 2, 3, 4,
        1, 2, 3, 4,
        1, 2, 3, 4,
        1, 2, 3, 4,
        1, 2, 3, 4,
        // swizzled integer formats
        3, 4,
        // srgb integer formats
        3, 4,
        // unnormalized integer formats (UNORM)
        1, 2, 3, 4,
        1, 2, 3, 4,
        1, 2, 3, 4,
        1, 2, 3, 4,
        1, 2, 3, 4,
        1, 2, 3, 4,
        // floating point formats
        1, 2, 3, 4,
        1, 2, 3, 4,
        // special packed formats
        3, 3,
        // depth stencil formats
        1, 1, 1, 1, 2, 2
    };
    assert(FORMAT_NULL <= d && d < FORMAT_COUNT);
    return (channel_counts[d]);
}

inline
int size_of_channel(data_format d)
{
    static int channel_sizes[] = {
        1,
        // normalized integer formats (NORM)
        1, 1, 1, 1,
        2, 2, 2, 2,
        4, 4, 4, 4,
        1, 1, 1, 1,
        2, 2, 2, 2,
        4, 4, 4, 4,
        // swizzled integer formats
        1, 1,
        // srgb integer formats
        1, 1,
        // unnormalized integer formats (UNORM)
        1, 1, 1, 1,
        2, 2, 2, 2,
        4, 4, 4, 4,
        1, 1, 1, 1,
        2, 2, 2, 2,
        4, 4, 4, 4,
        // floating point formats
        2, 2, 2, 2,
        4, 4, 4, 4,
        // special packed formats
        0, 0,
        // depth stencil formats
        0, 0, 0, 0, 0, 0
    };
    assert(FORMAT_NULL <= d && d <= FORMAT_RGBA_32F);
    return (channel_sizes[d]);
}

inline
int size_of_format(data_format d)
{
    static int format_sizes[] = {
        1,
        // normalized integer formats (NORM)
        1, 2, 3, 4,
        2, 4, 6, 8,
        4, 8, 12, 16,
        1, 2, 3, 4,
        2, 4, 6, 8,
        4, 8, 12, 16,
        // swizzled integer formats
        3, 4,
        // srgb integer formats
        3, 4,
        // unnormalized integer formats (UNORM)
        1, 2, 3, 4,
        2, 4, 6, 8,
        4, 8, 12, 16,
        1, 2, 3, 4,
        2, 4, 6, 8,
        4, 8, 12, 16,
        // floating point formats
        2, 4, 6, 8,
        4, 8, 12, 16,
        // special packed formats
        4, 4,
        // depth stencil formats
        2, 3, 4, 4, 4, 8
    };
    assert(FORMAT_NULL <= d && d < FORMAT_COUNT);
    return (format_sizes[d]);
}

} // namespace gl
} // namespace scm

#endif // SCM_GL_CORE_DATA_FORMATS_INL_INCLUDED
