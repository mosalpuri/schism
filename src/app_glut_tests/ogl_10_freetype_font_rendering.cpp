
#include <iostream>

#include <string>

#include <boost/scoped_ptr.hpp>

#include <ogl/gl.h>
#include <GL/glut.h>

#include <ogl/manipulators/trackball_manipulator.h>
#include <ogl/shader_objects/program_object.h>
#include <ogl/shader_objects/shader_object.h>

#include <scm_core/time/timer.h>

#include <image_handling/image_loader.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <scm_core/math/math.h>

boost::scoped_ptr<gl::program_object>  _shader_program;
boost::scoped_ptr<gl::shader_object>   _vertex_shader;
boost::scoped_ptr<gl::shader_object>   _fragment_shader;

gl::trackball_manipulator _trackball_manip;

int winx = 1024;
int winy = 640;

float initx = 0;
float inity = 0;

bool lb_down = false;
bool mb_down = false;
bool rb_down = false;

float dolly_sens = 10.0f;

// texture objects ids
unsigned tex0_id = 0;
unsigned tex1_id = 0;

bool init_font_rendering()
{
    // soon to be parameters
    std::string     _font_file_name         = std::string("C:/WINDOWS/fonts/consola.ttf");//cour.ttf");//
    unsigned        _font_pixel_size        = 9;
    unsigned        _display_dpi_resolution = 96;


    FT_Library      ft_lib;

    // initialize freetype
    if (FT_Init_FreeType(&ft_lib) != 0) {
        std::cout << "error initializing freetype library" << std::endl;
        return (false);
    }

    // load font face
    FT_Face         ft_face;
    if (FT_New_Face(ft_lib, _font_file_name.c_str(), 0, &ft_face) != 0) {
        std::cout << "error loading font file (" << _font_file_name << ")" << std::endl;
        return (false);
    }

    // rendering the font bullshit into a texture
    if (FT_Set_Char_Size(ft_face, 0, _font_pixel_size << 6, 0, _display_dpi_resolution) != 0) {
    //if (FT_Set_Pixel_Sizes(ft_face, 0, _font_pixel_size) != 0) {
        std::cout << "error setting font sizes" << std::endl;
        return (false);
    }

    for (int i = 0; i < 256; ++i) {
        if(FT_Load_Glyph(ft_face, FT_Get_Char_Index(ft_face, i), FT_LOAD_DEFAULT)) {
            std::cout << "error loading glyph at char code: " << i << std::endl;
        }
        else {
            // Move The Face's Glyph Into A Glyph Object.
            FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL);
            // This Reference Will Make Accessing The Bitmap Easier.
            FT_Bitmap& bitmap=ft_face->glyph->bitmap;

            std::cout << i << " w: " << bitmap.width << " h: " << bitmap.rows << " a: " << (ft_face->glyph->advance.x >> 6) << std::endl;
            //std::cout << i << " w: " << (float)(ft_face->bbox.xMax - ft_face->bbox.xMin) / (float)64 << " h: " << (float)(ft_face->bbox.yMax - ft_face->bbox.yMin) / (float)64 << std::endl;


        }
    }




    // shutdown font face
    if (FT_Done_Face(ft_face) != 0) {
        std::cout << "error closing font face" << std::endl;
        return (false);
    }

    // shutfdown freetype
    if (FT_Done_FreeType(ft_lib) != 0) {
        std::cout << "error shutting down freetype library" << std::endl;
    }

    return (true);
}

bool init_gl()
{
    // check for opengl verison 2.0 with
    // opengl shading language support
    if (!gl::is_supported("GL_VERSION_2_0")) {
        std::cout << "GL_VERSION_2_0 not supported" << std::endl;
        return (false);
    }
    else {
        std::cout << "OpenGL 2.0 supported, OpenGL Version: " << (char*)glGetString(GL_VERSION) << std::endl;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glPixelStorei(GL_PACK_ALIGNMENT,1);

    // set clear color, which is used to fill the background on glClear
    glClearColor(0.2f,0.2f,0.2f,1);

    // setup depth testing
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    // set polygonmode to fill front and back faces
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    _trackball_manip.dolly(1);

    _shader_program.reset(new gl::program_object());
    _vertex_shader.reset(new gl::shader_object(GL_VERTEX_SHADER));
    _fragment_shader.reset(new gl::shader_object(GL_FRAGMENT_SHADER));

    // load shader code from files
    if (!_vertex_shader->set_source_code_from_file("./../../../src/app_glut_tests/shader/texture_vertex_program.glsl")) {
        std::cout << "Error loadong vertex shader:" << std::endl;
        return (false);
    }
    if (!_fragment_shader->set_source_code_from_file("./../../../src/app_glut_tests/shader/texture_fragment_program.glsl")) {
        std::cout << "Error loadong frament shader:" << std::endl;
        return (false);
    }

    // compile shaders
    if (!_vertex_shader->compile()) {
        std::cout << "Error compiling vertex shader - compiler output:" << std::endl;
        std::cout << _vertex_shader->get_compiler_output() << std::endl;
        return (false);
    }
    if (!_fragment_shader->compile()) {
        std::cout << "Error compiling fragment shader - compiler output:" << std::endl;
        std::cout << _fragment_shader->get_compiler_output() << std::endl;
        return (false);
    }

    // attatch shaders to program object
    if (!_shader_program->attach_shader(*_vertex_shader)) {
        std::cout << "unable to attach vertex shader to program object:" << std::endl;
        return (false);
    }
    if (!_shader_program->attach_shader(*_fragment_shader)) {
        std::cout << "unable to attach fragment shader to program object:" << std::endl;
        return (false);
    }

    // link program object
    if (!_shader_program->link()) {
        std::cout << "Error linking program - linker output:" << std::endl;
        std::cout << _shader_program->get_linker_output() << std::endl;
       return (false);
    }

    image ps_data;

    // load texture image
    if (!open_image("./../../../res/textures/rock_decal_gloss_512.tga", ps_data)) {
        return (false);
    }
    // generate texture object id
    glGenTextures(1, &tex0_id);
    if (tex0_id == 0) {
        std::cout << "error creating texture object" << std::endl;
        return (false);
    }
    // load image as texture to texture object
    if (!load_2d_texture(tex0_id, ps_data, false)) {
        std::cout << "error uploading texture" << std::endl;
        return (false);
    }
    // delete image
    close_image(ps_data);

    // load texture image
    if (!open_image("./../../../res/textures/rock_normal_512.tga", ps_data)) {
        std::cout << "error open_image " << std::endl;
        return (false);
    }
    // generate texture object id
    glGenTextures(1, &tex1_id);
    if (tex1_id == 0) {
        std::cout << "error creating texture object" << std::endl;
        return (false);
    }
    // load image as texture to texture object
    if (!load_2d_texture(tex1_id, ps_data, false)) {
        std::cout << "error uploading texture" << std::endl;
        return (false);
    }
    // delete image
    close_image(ps_data);

    if (!init_font_rendering()) {
        std::cout << "error initializing font rendering engine" << std::endl;
        return (false);
    }


    float dif[4]    = {0.7, 0.7, 0.7, 1};
    float spc[4]    = {0.2, 0.7, 0.9, 1};
    float amb[4]    = {0.1, 0.1, 0.1, 1};
    float pos[4]    = {1,1,1,0};

    // setup light 0
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_SPECULAR, spc);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    // define material parameters
    glMaterialfv(GL_FRONT, GL_SPECULAR, spc);
    glMaterialf(GL_FRONT, GL_SHININESS, 128.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);

    return (true);
}


void display()
{
    static scm::core::timer _timer;
    static double           _accum_time     = 0.0;
    static unsigned         _accum_count    = 0;

    _timer.start();

    // clear the color and depth buffer
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // push current modelview matrix
    glPushMatrix();

        // apply camera transform
        _trackball_manip.apply_transform();

        // activate texture unit 0
        glActiveTexture(GL_TEXTURE0);
        // enable texturing on unit 0
        glEnable(GL_TEXTURE_2D);
        // bind texture to unit 0
        glBindTexture(GL_TEXTURE_2D, tex0_id);

        // activate texture unit 1
        glActiveTexture(GL_TEXTURE1);
        // enable texturing on unit 1
        glEnable(GL_TEXTURE_2D);
        // bind texture to unit 1
        glBindTexture(GL_TEXTURE_2D, tex1_id);

        // bind shader program to current opengl state
        _shader_program->bind();
        // set program parameters
        // set the sampler parameters to the particular texture unit number
        _shader_program->set_uniform_1i("_diff_gloss", 0);
        _shader_program->set_uniform_1i("_normal", 1);
        // set shininess material parameter directly to unfiform parameter
        _shader_program->set_uniform_1f("_shininess", 128.0f);

        // draw primitive
        glPushMatrix();
            glTranslatef(-0.5, -0.5, 0);
            // draw quad in immediate mode
            // with texture coordinates associated with every vertex
            glBegin(GL_QUADS);
              glMultiTexCoord2f(GL_TEXTURE0, 0.0f, 0.0f);
              glVertex2f(0.0f, 0.0f);

              glMultiTexCoord2f(GL_TEXTURE0, 1.0f, 0.0f);
              glVertex2f(1.0f, 0.0f);

              glMultiTexCoord2f(GL_TEXTURE0, 1.0f, 1.0f);
              glVertex2f(1.0f, 1.0f);

              glMultiTexCoord2f(GL_TEXTURE0, 0.0f, 1.0f);
              glVertex2f(0.0f, 1.0f);
            glEnd();
        glPopMatrix();

        // unbind shader program
        _shader_program->unbind();

        // unbind all texture objects and disable texturing on texture units
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

    // restore previously saved modelview matrix
    glPopMatrix();

    // swap the back and front buffer, so that the drawn stuff can be seen
    glutSwapBuffers();
    _timer.stop();

    _accum_time += _timer.get_time();
    ++_accum_count;

    if (_accum_time > 1000.0) {
        std::cout << "frame_time: " << _accum_time / static_cast<double>(_accum_count) << "msec \tfps: " << static_cast<double>(_accum_count) / (_accum_time / 1000.0) << std::endl;

        _accum_time     = 0.0;
        _accum_count    = 0;
    }
}

void resize(int w, int h)
{
    // retrieve current matrix mode
    GLint  current_matrix_mode;
    glGetIntegerv(GL_MATRIX_MODE, &current_matrix_mode);

    // safe the new dimensions
    winx = w;
    winy = h;

    // set the new viewport into which now will be rendered
    glViewport(0, 0, w, h);
    
    // reset the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.f, float(w)/float(h), 0.1f, 100.f);

    // restore saved matrix mode
    glMatrixMode(current_matrix_mode);
}

void mousefunc(int button, int state, int x, int y)
{
    switch (button) {
        case GLUT_LEFT_BUTTON:
            {
                lb_down = (state == GLUT_DOWN) ? true : false;
            }break;
        case GLUT_MIDDLE_BUTTON:
            {
                mb_down = (state == GLUT_DOWN) ? true : false;
            }break;
        case GLUT_RIGHT_BUTTON:
            {
                rb_down = (state == GLUT_DOWN) ? true : false;
            }break;
    }

    initx = 2.f * float(x - (winx/2))/float(winx);
    inity = 2.f * float(winy - y - (winy/2))/float(winy);
}

void mousemotion(int x, int y)
{
    float nx = 2.f * float(x - (winx/2))/float(winx);
    float ny = 2.f * float(winy - y - (winy/2))/float(winy);

    //std::cout << "nx " << nx << " ny " << ny << std::endl;

    if (lb_down) {
        _trackball_manip.rotation(initx, inity, nx, ny);
    }
    if (rb_down) {
        _trackball_manip.dolly(dolly_sens * (ny - inity));
    }
    if (mb_down) {
        _trackball_manip.translation(nx - initx, ny - inity);
    }

    inity = ny;
    initx = nx;
}

void idle()
{
    // on ilde just trigger a redraw
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        // ESC key
        case 27: exit (0); break;
        default:;
    }
}


int main(int argc, char **argv)
{
    // the stuff that has to be done
    glutInit(&argc, argv);
    // init a double buffered framebuffer with depth buffer and 4 channels
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA);
    // create window with initial dimensions
    glutInitWindowSize(winx, winy);
    glutCreateWindow("simple_glut");

    // init the GL context
    if (!gl::initialize()) {
        std::cout << "error initializing gl library" << std::endl;
        return (-1);
    }
    if (!init_image_loader()) {
        std::cout << "error initializing image library" << std::endl;
        return (-1);
    }
    if (!init_gl()) {
        std::cout << "error initializing gl context" << std::endl;
        return (-1);
    }

    
    // set the callbacks for resize, draw and idle actions
    glutReshapeFunc(resize);
    glutMouseFunc(mousefunc);
    glutMotionFunc(mousemotion);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutIdleFunc(idle);

    // and finally start the event loop
    glutMainLoop();

    return (0);
}