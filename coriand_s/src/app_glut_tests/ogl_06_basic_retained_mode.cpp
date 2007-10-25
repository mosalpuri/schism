

#include <iostream>

#include <boost/scoped_array.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

int winx = 1024;
int winy = 640;

float anim_angl = 0.0f;

struct vertex_format
{
    float       tex[3];
    float       pos[3];
};

boost::scoped_array<vertex_format>  vertices;
boost::scoped_array<unsigned short> indices;

// display list id
unsigned cube_display_list = 0;

bool init_gl()
{
    // set clear color, which is used to fill the background on glClear
    glClearColor(0.2f,0.2f,0.2f,1);
    
    // setup depth testing
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    // set polygonmode to fill front and back faces
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // initialize vertex and index array
    vertices.reset(new vertex_format[8]);
    indices.reset(new unsigned short[14]);

    // generate vertices for a unit cube
    // as a interleaved vertex array
    for (unsigned int v = 0; v < 8; v++) {
        vertices[v].pos[0] = vertices[v].tex[0] = (float)(v & 0x01);
        vertices[v].pos[1] = vertices[v].tex[1] = (float)((v & 0x02) >> 1);
        vertices[v].pos[2] = vertices[v].tex[2] = (float)((v & 0x04) >> 2);
    }

    // setup index array as trianglestrip
    indices[0]  = 4;
    indices[1]  = 5;
    indices[2]  = 6;
    indices[3]  = 7;
    indices[4]  = 3;
    indices[5]  = 5;
    indices[6]  = 1;
    indices[7]  = 4;
    indices[8]  = 0;
    indices[9]  = 6;
    indices[10] = 2;
    indices[11] = 3;
    indices[12] = 0;
    indices[13] = 1;

    // generatew display list for drawing of the cube
    cube_display_list = glGenLists(1);
    glNewList(cube_display_list, GL_COMPILE);
        glBegin(GL_TRIANGLE_STRIP);
            for (unsigned i = 0; i < 14; ++i) {
                glTexCoord3fv(vertices[indices[i]].tex);
                glVertex3fv(vertices[indices[i]].pos);
            }
        glEnd();
    glEndList();

    return (true);
}

void draw_animated_cube_immediate_mode()
{
    // retrieve current polygon mode
    GLint current_poly_mode[2];
    glGetIntegerv(GL_POLYGON_MODE, current_poly_mode);
    // set polygon mode to wireframe rendering
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
        glRotatef(anim_angl, 1, 1, 1);
        glScalef(2.0, 2.0, 2.0);
        glTranslatef(-.5f, -.5f, -.5f);
        glBegin(GL_TRIANGLE_STRIP);
            for (unsigned i = 0; i < 14; ++i) {
                glTexCoord3fv(vertices[indices[i]].tex);
                glVertex3fv(vertices[indices[i]].pos);
            }
        glEnd();
    glPopMatrix();

    // restore saved polygon mode
    glPolygonMode(GL_FRONT, current_poly_mode[0]);
    glPolygonMode(GL_BACK,  current_poly_mode[1]);
}

void draw_animated_cube_display_list()
{
    // retrieve current polygon mode
    GLint current_poly_mode[2];
    glGetIntegerv(GL_POLYGON_MODE, current_poly_mode);
    // set polygon mode to wireframe rendering
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
        glRotatef(anim_angl, 1, 1, 1);
        glScalef(2.0, 2.0, 2.0);
        glTranslatef(-.5f, -.5f, -.5f);
        glCallList(cube_display_list);
    glPopMatrix();

    // restore saved polygon mode
    glPolygonMode(GL_FRONT, current_poly_mode[0]);
    glPolygonMode(GL_BACK,  current_poly_mode[1]);
}

void draw_animated_cube_vertex_array()
{
    // retrieve current polygon mode
    GLint current_poly_mode[2];
    glGetIntegerv(GL_POLYGON_MODE, current_poly_mode);
    // set polygon mode to wireframe rendering
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glColor3f(1.0f, 1.0f, 1.0f);

    // declare vertex array pointers
    glVertexPointer(3, GL_FLOAT, 6*sizeof(float), vertices.get());
    // declare texture coord pointer starting with offset of 3 float
    // values from the beginning of the interleaved array
    glTexCoordPointer(3, GL_FLOAT, 6*sizeof(float), (char*)(vertices.get()) + 3*sizeof(float));

    // enable vertex array pointers in client state
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glPushMatrix();
        glRotatef(anim_angl, 1, 1, 1);
        glScalef(2.0, 2.0, 2.0);
        glTranslatef(-.5f, -.5f, -.5f);
        // draw vertex array using the index array
        glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, indices.get());
    glPopMatrix();

    // disable vertex array pointers
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // restore saved polygon mode
    glPolygonMode(GL_FRONT, current_poly_mode[0]);
    glPolygonMode(GL_BACK,  current_poly_mode[1]);
}

void display()
{
    // clear the color and depth buffer
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    // set shade model
    glShadeModel(GL_SMOOTH);
    //glShadeModel(GL_FLAT);

    // push current modelview matrix
    glPushMatrix();

        // translate the next drawn triangle back a litte so it can be seen
        glTranslatef(0,0,-5.0f);

        draw_animated_cube_immediate_mode();
        //draw_animated_cube_display_list();
        //draw_animated_cube_vertex_array();


    // restore previously saved modelview matrix
    glPopMatrix();

    // swap the back and front buffer, so that the drawn stuff can be seen
    glutSwapBuffers();
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

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        // ESC key
        case 27: exit (0); break;
        default:;
    }
}

void idle()
{
    // animate
    anim_angl += 0.5f;

    // on ilde just trigger a redraw
    glutPostRedisplay();
}


int main(int argc, char **argv)
{
    // the stuff that has to be done
    glutInit(&argc, argv);
    // init a double buffered framebuffer with depth buffer and 4 channels
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA);
    // create window with initial dimensions
    glutInitWindowSize(winx, winy);
    glutCreateWindow("OpenGL - red triangle");

    // init the GL context
    if (!init_gl()) {
        std::cout << "error initializing gl context" << std::endl;
        return (-1);
    }

    // set the callbacks for resize, draw and idle actions
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutIdleFunc(idle);

    // and finally start the event loop
    glutMainLoop();
    return (0);
}