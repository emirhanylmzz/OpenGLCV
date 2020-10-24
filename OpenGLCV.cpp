#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types_c.h>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>

using namespace cv;
using namespace std;

VideoCapture capture0;
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall())

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall()
{
    while (GLenum error = glGetError())
    {
        cout << "[OpenGL Error] (" << error << ")" << endl;
        return false;
    }
    return true;
}

int main()
{
    int w = 800, h = 600;

    //Initialisation glfw
    if (!glfwInit()) {
        cerr << "GLFW initialisation failed!";
        glfwTerminate();
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(w, h, "OpenGL", NULL, nullptr);
    if (!window)
    {
        cerr << "Window creation failed!" << endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    //Init Glew
    if (glewInit() != GLEW_OK)
    {
        cerr << "Glew initialisation failed!" << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, w, h); // use a screen size of WIDTH x HEIGHT
    glEnable(GL_TEXTURE_2D);     // Enable 2D texturing

    glMatrixMode(GL_PROJECTION);     // Make a simple 2D projection on the entire window
    glLoadIdentity();
    glOrtho(0.0, w, h, 0.0, 0.0, 100.0);

    glMatrixMode(GL_MODELVIEW);    // Set the matrix mode to object modeling

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window


    /* OpenGL texture binding of the image loaded by DevIL  */
    GLuint texid;
    glGenTextures(1, &texid); /* Texture name generation */
    glBindTexture(GL_TEXTURE_2D, texid); /* Binding of texture name */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear interpolation for magnification filter */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear interpolation for minifying filter */

    capture0.open(0);

    while (!glfwWindowShouldClose(window))
    {
        Mat frame;
        capture0 >> frame;
        putText(frame, "SCALAR VISION", cvPoint(30, 100),
            FONT_HERSHEY_COMPLEX, 1, cvScalar(255, 0, 0), 1);

        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        // Clear color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, texid);
        glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.size().width, frame.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, (void*)frame.data); /* Texture specification */

        /* Draw a quad */
        glBegin(GL_QUADS);
        glTexCoord2i(0, 0); glVertex2i(0, 0);
        glTexCoord2i(0, 1); glVertex2i(0, h);
        glTexCoord2i(1, 1); glVertex2i(w, h);
        glTexCoord2i(1, 0); glVertex2i(w, 0);
        glEnd();
        glFlush();
        glfwSwapBuffers(window);
    }
    capture0.release();
    glfwTerminate();
    return 1;
}