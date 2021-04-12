

#include<stdio.h>
#include<stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include <IL/il.h>
#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

GLuint buffers[1];
int imageWidth, imageHeight;

float camX = 00, camY = 30, camZ = 40;
int startX, startY, tracking = 0;

int alpha = 0, beta = 45, r = 50;

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(45,ratio,1,1000);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}



void drawTerrain() {

    // colocar aqui o código de desnho do terreno usando VBOs com TRIANGLE_STRIPS
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	for(int i = 0; i < imageWidth - 1; i++){
		glDrawArrays(GL_TRIANGLE_STRIP, i * 2 * imageWidth, 2 * imageWidth);
	}
}



void renderScene(void) {

	float pos[4] = {-1.0, 1.0, 1.0, 0.0};

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(camX, camY, camZ, 
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

	drawTerrain();

	// just so that it renders something before the terrain is built
	// to erase when the terrain is ready
	glutWireTeapot(2.0);

// End of frame
	glutSwapBuffers();
}



void processKeys(unsigned char key, int xx, int yy) {

// put code to process regular keys in here
	switch (key) {
	case '+':
		r += 1;

		camX = r * sin(alpha * 3.14 / 180.0) * cos(beta * 3.14 / 180.0);
		camZ = r * cos(alpha * 3.14 / 180.0) * cos(beta * 3.14 / 180.0);
		camY = r * 							   sin(beta * 3.14 / 180.0);

		break;
	case '-':
		r = r == 0 ? 0 : r - 1;

		camX = r * sin(alpha * 3.14 / 180.0) * cos(beta * 3.14 / 180.0);
		camZ = r * cos(alpha * 3.14 / 180.0) * cos(beta * 3.14 / 180.0);
		camY = r * 							   sin(beta * 3.14 / 180.0);
		
		break;
	default:
		break;
	}
}



void processMouseButtons(int button, int state, int xx, int yy) {
	
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	}
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha += (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			
			r -= yy - startY;
			if (r < 3)
				r = 3.0;
		}
		tracking = 0;
	}
}


void processMouseMotion(int xx, int yy) {

	int deltaX, deltaY;
	int alphaAux, betaAux;
	int rAux;

	if (!tracking)
		return;

	deltaX = xx - startX;
	deltaY = yy - startY;

	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0)
			betaAux = 85.0;
		else if (betaAux < -85.0)
			betaAux = -85.0;

		rAux = r;
	}
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r - deltaY;
		if (rAux < 3)
			rAux = 3;
	}
	camX = rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camZ = rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camY = rAux * 							     sin(betaAux * 3.14 / 180.0);
}


void init() {

// 	Load the height map "terreno.jpg"
	
	unsigned int t;
	unsigned char *imageData;

	ilGenImages(1,&t);
	ilBindImage(t);

	ilLoadImage((ILstring)"terreno.jpg");

	ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);

	imageWidth = ilGetInteger(IL_IMAGE_WIDTH);
	imageHeight = ilGetInteger(IL_IMAGE_HEIGHT);

	imageData = ilGetData();

// 	Build the vertex arrays

	std::vector<float> pontos;

	for(int height = 0; height < imageHeight; height++){
		for(int width = 0; width < imageWidth; width++){
			//printf("x:%d y:%d z:%d\n", j, 0, i);
			pontos.push_back(width - (imageWidth / 2)); //x1
			pontos.push_back(imageData[height * imageWidth + width]); //y1
			pontos.push_back(height - (imageHeight / 2)); //z1

			pontos.push_back(width - (imageWidth / 2)); //x2
			pontos.push_back(imageData[(height + 1) * imageWidth + width]); //y2
			pontos.push_back(height + 1 - (imageHeight / 2)); //z2
		}
	}

	glGenBuffers(1, buffers);
	glBindBuffer(GL_ARRAY_BUFFER,buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * pontos.size(), pontos.data(), GL_STATIC_DRAW);

// 	OpenGL settings
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
}


int main(int argc, char **argv) {

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("CG@DI-UM");
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glPolygonMode(GL_FRONT, GL_LINE);

// Required callback registry 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

    glewInit();
    ilInit();
	init();	

// enter GLUT's main cycle
	glutMainLoop();
	
	return 0;
}

