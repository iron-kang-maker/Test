#include "glwidget.h"
#include <GL/glut.h>
#include <GL/glu.h>

#define SIZE 0.2

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent)
{
}

GLWidget::~GLWidget()
{
}

void GLWidget::initializeGL()
{
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 2, 4, 0, 0.5f, 0, 0, 1, 0);

    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
          // Top face (y = SIZE)
          // Define vertices in counter-clockwise (CCW) order with normal pointing out
          glColor3f(0.0f, SIZE, 0.0f);     // Green
          glVertex3f( SIZE, SIZE, -SIZE);
          glVertex3f(-SIZE, SIZE, -SIZE);
          glVertex3f(-SIZE, SIZE,  SIZE);
          glVertex3f( SIZE, SIZE,  SIZE);

          // Bottom face (y = -SIZE)
          glColor3f(SIZE, 0.5f, 0.0f);     // Orange
          glVertex3f( SIZE, -SIZE,  SIZE);
          glVertex3f(-SIZE, -SIZE,  SIZE);
          glVertex3f(-SIZE, -SIZE, -SIZE);
          glVertex3f( SIZE, -SIZE, -SIZE);

          // Front face  (z = SIZE)
          glColor3f(SIZE, 0.0f, 0.0f);     // Red
          glVertex3f( SIZE,  SIZE, SIZE);
          glVertex3f(-SIZE,  SIZE, SIZE);
          glVertex3f(-SIZE, -SIZE, SIZE);
          glVertex3f( SIZE, -SIZE, SIZE);

          // Back face (z = -SIZE)
          glColor3f(SIZE, SIZE, 0.0f);     // Yellow
          glVertex3f( SIZE, -SIZE, -SIZE);
          glVertex3f(-SIZE, -SIZE, -SIZE);
          glVertex3f(-SIZE,  SIZE, -SIZE);
          glVertex3f( SIZE,  SIZE, -SIZE);

          // Left face (x = -SIZE)
          glColor3f(0.0f, 0.0f, SIZE);     // Blue
          glVertex3f(-SIZE,  SIZE,  SIZE);
          glVertex3f(-SIZE,  SIZE, -SIZE);
          glVertex3f(-SIZE, -SIZE, -SIZE);
          glVertex3f(-SIZE, -SIZE,  SIZE);

          // Right face (x = SIZE)
          glColor3f(SIZE, 0.0f, SIZE);     // Magenta
          glVertex3f(SIZE,  SIZE, -SIZE);
          glVertex3f(SIZE,  SIZE,  SIZE);
          glVertex3f(SIZE, -SIZE,  SIZE);
          glVertex3f(SIZE, -SIZE, -SIZE);
       glEnd();  // End of drawing color-cube
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, w / h, 0.1, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(SIZE, 0.5f, -7.0f);
}
