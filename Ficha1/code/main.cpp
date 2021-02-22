#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

float size = 1;
float x = 0;
float y = 0;
float z = 0;
float angleX = 0;
float angleY = 0;
float angleZ = 0;

void changeSize(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you can�t make a window with zero width).
	if (h == 0)
		h = 1;
	// compute window's aspect ratio
	float ratio = w * 1.0f / h;
	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load the identity matrix
	glLoadIdentity();
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// Set the perspective
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);
	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


void renderScene(void)
{
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// set camera
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 5.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 1.0f, 0.0f);
		
	// put drawing instructions here
	
	//size = abs(sin(glutGet(GLUT_ELAPSED_TIME)/1000));
	
	/*
	if(size >= 1.5) size = 0.1;
	else size += 0.0005;
	*/
	
	glTranslated(x, y, z);
	glRotated(angleX, 0, 1, 0);
	glRotated(angleZ, 0, 0, 1);
	glRotated(angleY, 1, 0, 0);
	glutWireTeapot(size);

	// End of frame
	glutSwapBuffers();
}

void action(unsigned char key, int x, int y){
	switch(key){
		case '+':
			size += 0.1;
			break;
		case '-':
			size -= 0.1;
			break;
        case 'w':
            z += 0.1;
            break;
        case 's':
            z -= 0.1;
            break;
        case 'a':
            x += 0.1;
            break;
        case 'd':
            x -= 0.1;
            break;
        case 'z':
            angleY += 0.1;
            break;
        case 'x':
            angleY -= 0.1;
            break;
		case 'q':
            angleX += 1;
            break;
        case 'e':
            angleX -= 1;
            break;
		default:
			break;
	}
}

void printInfo() {

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
}


int main(int argc, char** argv)
{
	// put GLUT init here
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("CG@DI");
	glutFullScreen();
    
	// put callback registry here
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
	
	glutKeyboardFunc(action);
	
	// some OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	// enter GLUT�s main cycle
	glutMainLoop();
	
	return 1;
}