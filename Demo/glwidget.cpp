#include "glwidget.h"

#include <GL/glext.h>
#include <GL/glx.h>
#include <QtGui>
#include <iostream>

PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent), draw_mode(GL_TRIANGLES)
{
}

GLWidget::~GLWidget()
{
    //delete the vertex array and vertex buffer
    GLuint loc = glGetAttribLocation(shader->programId(), "position");
    glDisableVertexAttribArray(loc);
    loc = glGetAttribLocation(shader->programId(), "color");
    glDisableVertexAttribArray(loc);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vertex_buffer);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vertex_array);

    //delete the shaders
    shader->release();
    shader->removeAllShaders();
    delete shader;
}

void GLWidget::initializeGL()
{
    //load opengl functions that are not in QGLFunctions
    //on mac and linux use glXGetProcAddress
    //on windows use wglGetProcAddress and don't cast to const GLubyte*
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glXGetProcAddress((const GLubyte*)"glDeleteVertexArrays");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glXGetProcAddress((const GLubyte*)"glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glXGetProcAddress((const GLubyte*)"glBindVertexArray");

    //load opengl functions
    initializeGLFunctions();

    //set the clear color to black
    glClearColor(0,0,0,1);

    //enable depth testing
    glEnable(GL_DEPTH_TEST);

    //load and initialize shaders
    initializeShaders();

    //setup vertex and color data and send to GPU
    initializeBuffers();
}

void GLWidget::paintGL()
{
    //clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw the 3 points as a triangle or line loop
    glDrawArrays(draw_mode, 0, 3);
}

void GLWidget::resizeGL(int w, int h)
{
    //set the viewing rectangle to be the entire window
    glViewport(0,0,w,h);
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(500, 500);
}

void GLWidget::initializeShaders()
{
    //load the shaders, link the shader, and set the shader to be active
    shader = new QGLShaderProgram();
    shader->addShaderFromSourceFile(QGLShader::Vertex, ":/basic.vert");
    shader->addShaderFromSourceFile(QGLShader::Fragment, ":/basic.frag");
    shader->link();
    shader->bind();
}

void GLWidget::initializeBuffers()
{
    //vertex and color data
    GLfloat vertices[] = {-0.5,-0.5,0, 0.5,-0.5,0, 0,0.5,0};
    GLfloat colors[] = {1,0,0, 1,1,0, 1,0,1};

    //get a unique id for the vertex array
    glGenVertexArrays(1, &vertex_array);
    //set the vertex array to be the active one
    glBindVertexArray(vertex_array);

    //get a unique id for the vertex buffer
    glGenBuffers(1, &vertex_buffer);
    //set the vertex buffer to be active
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    //specify the size and type of the vertex buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);

    //load the vertex and color data into the vertex buffer (vertices in the first half, colors in the second half)
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

    //set the size of the vertex (3 component) and connect it to the correct shader variable
    GLuint loc = glGetAttribLocation(shader->programId(), "position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //set the size of the color (3 component) and connect it to the correct shader variable
    loc = glGetAttribLocation(shader->programId(), "color");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(vertices));
}

void GLWidget::triangles()
{
    //change the draw mode to GL_TRIANGLES and update the screen
    draw_mode = GL_TRIANGLES;
    updateGL();
}

void GLWidget::lineLoop()
{
    //change the draw mode to GL_LINE_LOOP and update the screen
    draw_mode = GL_LINE_LOOP;
    updateGL();
}
