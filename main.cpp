#include <iostream>

#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <gd.h>

void changeSize(int w, int h) {
    if (h == 0)
        h = 1;
    float ratio =  w * 1.0 / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
}

void renderScene(void) {
    // Clear all
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset
    glLoadIdentity();

    // Set camera
    gluLookAt(0.0f, 0.0f, 10.0f,
              0.0f, 0.0f,  0.0f,
              0.0f, 1.0f,  0.0f);

    // Swap to show new buffer
    glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {
    if (key == 'q' || key == 27)
        std::exit(0);
}

int main(int argc, char **argv) {
    // Initialize all
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(320,320);
    glutCreateWindow("Lighthouse3D- GLUT Tutorial");

    // Our callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);
    glutKeyboardFunc(processNormalKeys);

    // Give control to GLUT lib.
    glutMainLoop();

    return 0;
}