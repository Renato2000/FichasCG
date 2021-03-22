#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#include <vector>
#include <iostream>

float alfa = 0.0f, beta = 0.5f, radius = 100.0f;
float camX, camY, camZ;

typedef struct ponto{
	float x, y, z;
} *PONTO;


//Campos para a cena:
int r = 50;
int ri = 35;
int rc = 15;
int seed = 12345;
int numberTrees = 400;
int nrIndios = 16;
int nrCowboys = 8;
float factor = 0;
float velocidade = 1;
std::vector<PONTO> pontosArvores;

void spherical2Cartesian() {

	camX = radius * cos(beta) * sin(alfa);
	camY = radius * sin(beta);
	camZ = radius * cos(beta) * cos(alfa);
}


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void drawField(){
	glColor3f(0.2f, 0.8f, 0.2f);
	glBegin(GL_TRIANGLES);
		glVertex3f(100.0f, 0, -100.0f);
		glVertex3f(-100.0f, 0, -100.0f);
		glVertex3f(-100.0f, 0, 100.0f);

		glVertex3f(100.0f, 0, -100.0f);
		glVertex3f(-100.0f, 0, 100.0f);
		glVertex3f(100.0f, 0, 100.0f);
	glEnd();
}

void drawTree(PONTO p){
	glPushMatrix();
	glTranslatef(p->x, 0, p->z);
	glRotatef(-90, 1, 0, 0);
	glColor3f(0.4f, 0.2f, 0.1f);
	glutSolidCone(0.5, 8, 10, 10);
	glColor3f(0, 0.4, 0.15);
	glTranslatef(0, 0, 4);
	glutSolidCone(4, 10, 10, 10);
	glPopMatrix();
}

void drawTrees(){
	for (const auto& ponto : pontosArvores){
		drawTree(ponto);
	}
}

void drawFire(){
	glColor3f(1, 0, 1);
	glutSolidTorus(0.75, 2, 10, 10);
	glRotatef(90, 0, 1, 0);
	glutSolidTorus(0.75, 2, 10, 10);
}

void drawCowboys(){
	float alpha = 0;
	glColor3f(0, 0, 1);
	for(int i = 0; i < nrCowboys; i++){
		alpha = i * 2 * M_PI / nrCowboys - factor;
		float x = rc * sin(alpha);
		float z = rc * cos(alpha);

		glPushMatrix();		
    	glTranslatef(x, 1, z);
		glRotatef((alpha / (2 * M_PI)) * 360 - 90, 0, 1, 0);
    	glutSolidTeapot(1.5);
    	glPopMatrix();
	}
}

void drawIndios(){
	float alpha = 0;
	glColor3f(1, 0, 0);
	for(int i = 0; i < nrIndios; i++){
		alpha = i * 2 * M_PI / nrIndios + factor;
		float x = ri * sin(alpha);
		float z = ri * cos(alpha);

		glPushMatrix();		
    	glTranslatef(x, 1, z);
		glRotatef((alpha / (2 * M_PI)) * 360, 0, 1, 0);
    	glutSolidTeapot(1.5);
    	glPopMatrix();
	}
}

void drawScene(){
	drawField();
	drawTrees();
	drawFire();
	drawIndios();
	drawCowboys();
}

void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	// End of frame
	
	
	// put code to draw scene in here
	drawScene();
	
	glutSwapBuffers();
	
	factor += (M_PI / 180) * velocidade;
	if(factor >= 2*M_PI) factor = 0;
}

std::vector<PONTO> generateTreesRand(){
	std::vector<PONTO> pontos;

	for(int i = 0; i < numberTrees; i++){
		PONTO p = (PONTO) malloc(sizeof(PONTO));
		do{
			p->x = rand() % 200 - 100; 
			p->y = 0; 
			p->z = rand() % 200 - 100;
		} while(sqrt(pow(p->x,2) + pow(p->y,2) + pow(p->z,2)) < r);
		pontos.emplace_back(p);
	}

	return pontos;
}


void processKeys(unsigned char c, int xx, int yy) {

// put code to process regular keys in here

}


void processSpecialKeys(int key, int xx, int yy) {

	switch (key) {

	case GLUT_KEY_RIGHT:
		alfa -= 0.1; break;

	case GLUT_KEY_LEFT:
		alfa += 0.1; break;

	case GLUT_KEY_UP:
		beta += 0.1f;
		if (beta > 1.5f)
			beta = 1.5f;
		break;

	case GLUT_KEY_DOWN:
		beta -= 0.1f;
		if (beta < -1.5f)
			beta = -1.5f;
		break;

	case GLUT_KEY_PAGE_DOWN: radius -= 1.0f;
		if (radius < 1.0f)
			radius = 1.0f;
		break;

	case GLUT_KEY_PAGE_UP: radius += 1.0f; break;
	}
	spherical2Cartesian();
	glutPostRedisplay();

}


void printInfo() {

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	printf("\nUse Arrows to move the camera up/down and left/right\n");
	printf("Home and End control the distance from the camera to the origin");
}


int main(int argc, char **argv) {

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("CG@DI-UM");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	
// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	spherical2Cartesian();

	printInfo();

	pontosArvores = generateTreesRand();

	glutIdleFunc(renderScene);

// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
