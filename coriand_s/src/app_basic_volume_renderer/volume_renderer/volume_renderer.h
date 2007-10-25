
#ifndef VOLUME_RENDERER_H_INCLUDED
#define VOLUME_RENDERER_H_INCLUDED

#include <scm/core/math/math.h>

// includes, project

#include <scm/ogl/utilities/volume_textured_unit_cube.h>
#include <scm/ogl/utilities/volume_crossplanes.h>

namespace gl
{
    class volume_renderer_parameters;

    class volume_renderer
    {
    public:
        volume_renderer();
        virtual ~volume_renderer();

        virtual bool            initialize();
        virtual void            frame(const gl::volume_renderer_parameters&)      = 0;
        virtual bool            shutdown()   = 0;

        void                    draw_outlines(const gl::volume_renderer_parameters&);
        void                    draw_bounding_volume(const gl::volume_renderer_parameters&);

    protected:
        scm::gl::volume_textured_unit_cube   _cube;
        scm::gl::volume_crossplanes          _planes;
        bool                                 _initialized;

    private:
        // declared - never defined
        volume_renderer(const volume_renderer&);
        const volume_renderer& operator=(const volume_renderer&);

    }; // class volume_renderer

} // namespace gl

#endif // VOLUME_RENDERER_H_INCLUDED