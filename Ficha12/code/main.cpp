#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include<stdio.h>

// distance to target
float dist = 700;
// distance between the eyes
float disp = 30;


float angle=0;
float cBlack[] = {0,0,0,0};
float cWhite[] = {1,1,1,1};
float cRed[] = {0.4,0,0};
float cGreen[] = {0,0.4,0.4};
float cRedish[] = {0.2,0,0};
float cGreenish[] = {0,0.2,0.2};

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	float ratio = 1.0* w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45,ratio,400,10000);
	glMatrixMode(GL_MODELVIEW);
}

void calcCross(float* v, float* u, float* res) {
	res[0] = v[1] * u[2] - v[2] * u[1];   
	res[1] = v[2] * u[0] - v[0] * u[2];
	res[2] = v[0] * u[1] - v[1] * u[0];
}

void renderScene(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	float lpos[] = {1,1,0,0};

	// teapot size
	float ts = 150;

	// fill in the code...
	float dir[] = {0, 0, -1};
	float up[] = {0, 1, 0};
	float r[3];
	calcCross(dir, up, r);

	float posr[] = {0 + r[0] * disp, 0 + r[1] * disp, dist + r[2] * disp};
	float posl[] = {0 - r[0] * disp, 0 - r[1] * disp, dist - r[2] * disp};

	glPushMatrix();
	glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
	// set camera for red eye.
	gluLookAt(posr[0], posr[1], posr[2], dir[0], dir[1], dir[2], 0, 1, 0);
	// light
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	// draw scene
	glRotatef(angle, 1, 1, 1);
	glutWireTeapot(ts);
	glPopMatrix();

	glPushMatrix();
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
	// set camera for blue eye.
	gluLookAt(posl[0], posl[1], posl[2], dir[0], dir[1], dir[2], 0, 1, 0);
	// light
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	// draw scene
	glRotatef(angle, 1, 1, 1);
	glutWireTeapot(ts);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glPopMatrix();

	glutSwapBuffers();

	angle+=0.25;
}

void processNormalKeys(unsigned char key, int x, int y) {

	char s[20];
	if (key == 27) 
		exit(0);
	switch(key) {
		case 'a' : dist *=.9;break;
		case 's' : dist *= 1.1; break;
	}
	sprintf(s,"%f", dist);
	glutSetWindowTitle(s);

}


int main(int argc, char **argv) {

	// GLUT and window init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("CG @ DI");

	// GLUT callbacks
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processNormalKeys);
	
	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	//	glEnable(GL_CULL_FACE);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);

	glClearColor(0.0f,0.0f,0.0f,0.0f);


	glutMainLoop();

	return 0;
}

