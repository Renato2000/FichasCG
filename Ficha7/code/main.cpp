

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

float camLx = 00, camLy = 00, camLz = 00;
float camPx = 00, camPy = 30, camPz = 40;
float camHeight = 2, camSpeed = 1;
int startX, startY, tracking = 0;
int camAlpha = 0, camBeta = 0, camR = 50;

typedef struct ponto{
	float x, y, z;
} *PONTO;

//Campos para a cena:
int rt = 50;
int ri = 35;
int rc = 15;
int seed = 12345;
int numberTrees = 400;
int nrIndios = 16;
int nrCowboys = 8;
float factor = 0;
float velocidade = 1;
std::vector<PONTO> pontosArvores;
unsigned char *imageData;


void normalizeCamera(){
    double fator = sqrt(pow(camLx,2) + pow(camLy,2) + pow(camLz,2));

	camLx = camLx / fator;
	camLy = camLy / fator;
	camLz = camLz / fator;
}

PONTO cross(PONTO v1, PONTO v2){
	PONTO ret = (PONTO) malloc(sizeof(PONTO));

	ret->x = v1->y * v2->z - v1->z * v2->y;
	ret->y = v1->x * v2->z - v1->z * v2->x;
	ret->z = v1->x * v2->y - v1->y * v2->x;

    return ret;
}


float h(int x, int z){

	return imageData[z * imageWidth + x]; 
}

float hf(float px, float pz){
	int x1, x2, z1, z2;

	x1 = floor(px + imageHeight/2); x2 = x1 + 1;
	z1 = floor(pz + imageHeight/2); z2 = z1 + 1;

	float fx = px - x1  + imageHeight/2;
	float fz = pz - z1  + imageHeight/2;

	float h_x1_z  = h(x1,z1) * (1-fz) + h(x1,z2) * fz;
	float h_x2_z  = h(x2,z1) * (1-fz) + h(x2,z2) * fz;

	return h_x1_z * (1 - fx) + h_x2_z * fx;
}

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

void drawTree(PONTO p){
	glPushMatrix();
	glTranslatef(p->x, p->y, p->z);
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
	glPushMatrix();
	glColor3f(1, 0, 1);
	glutSolidTorus(0.75, 2, 10, 10);
	glPopMatrix();
}

void drawCowboys(){
	float alpha = 0;
	glColor3f(0, 0, 1);
	for(int i = 0; i < nrCowboys; i++){
		alpha = i * 2 * M_PI / nrCowboys - factor;
		float x = rc * sin(alpha);
		float z = rc * cos(alpha);

		glPushMatrix();		
    	glTranslatef(x, hf(x, z), z);
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
    	glTranslatef(x, hf(x, z), z);
		glRotatef((alpha / (2 * M_PI)) * 360, 0, 1, 0);
    	glutSolidTeapot(1.5);
    	glPopMatrix();
	}
}

void drawTerrain() {
	
	glColor3f(0, 0.4, 0.4);

    // colocar aqui o código de desnho do terreno usando VBOs com TRIANGLE_STRIPS
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	for(int i = 0; i < imageWidth - 1; i++){
		glDrawArrays(GL_TRIANGLE_STRIP, i * 2 * imageWidth, 2 * imageWidth);
	}
}

void drawScene(){
	drawTerrain();
	drawTrees();
	drawFire();
	drawIndios();
	drawCowboys();
}

void renderScene(void) {

	float pos[4] = {-1.0, 1.0, 1.0, 0.0};

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(camPx, camPy, camPz, 
		      camLx, camLy, camLz,
			  0.0f,1.0f,0.0f);

	drawScene();

// End of frame
	glutSwapBuffers();

	factor += (M_PI / 180) * velocidade;
	if(factor >= 2*M_PI) factor = 0;
}

void processKeys(unsigned char key, int xx, int yy) {
	float dx, dy, dz;
	float rx, ry, rz;
	float alturaAnterior;

// put code to process regular keys in here
	switch(key) {
		case 'w':
			dx = camLx - camPx;
			dy = 0;
			dz = camLz - camPz;

			alturaAnterior = camPy;

			camPx = camPx + camSpeed * dx;
			camPz = camPz + camSpeed * dz;
			camPy = camHeight + hf(camPx, camPz);

			camLx = camLx + camSpeed * dx;
			camLy = camLy + camPy - alturaAnterior;
			camLz = camLz + camSpeed * dz;

			//normalizeCamera();

			break;
		case 'a':
			dx = camLx - camPx;
			dy = 0;
			dz = camLz - camPz;

			alturaAnterior = camPy;

			rx = 1.0f * dz - 0.0f * dy;
			ry = 0.0f * dz - 0.0f * dx;
			rz = 0.0f * dy - 1.0f * dx;

			camPx = camPx + camSpeed * rx;
			camPz = camPz + camSpeed * rz;
			camPy = camHeight + hf(camPx, camPz);

			camLx = camLx + camSpeed * rx;
			camLy = camLy + camPy - alturaAnterior;
			camLz = camLz + camSpeed * rz;

			//normalizeCamera();

			break;
		case 's':
			dx = camPx - camLx;
			dy = 0;
			dz = camPz - camLz;

			alturaAnterior = camPy;

			camPx = camPx + camSpeed * dx;
			camPz = camPz + camSpeed * dz;
			camPy = camHeight + hf(camPx, camPz);

			camLx = camLx + camSpeed * dx;
			camLy = camLy + camPy - alturaAnterior;
			camLz = camLz + camSpeed * dz;

			//normalizeCamera();

			break;
		case 'd':
			dx = camLx - camPx;
			dy = 0;
			dz = camLz - camPz;

			rx = dy * 0.0f - dz * 1.0f;
			ry = dx * 0.0f - dz * 0.0f;
			rz = dx * 1.0f - dy * 0.0f;

			alturaAnterior = camPy;

			camPx = camPx + camSpeed * rx;
			camPz = camPz + camSpeed * rz;
			camPy = camHeight + hf(camPx, camPz);

			camLx = camLx + camSpeed * rx;
			camLy = camLy + camPy - alturaAnterior;
			camLz = camLz + camSpeed * rz;

			//normalizeCamera();

			break;
		default:
			int i = 0;
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
			camAlpha += (xx - startX);
			camBeta += (yy - startY);
		}
		else if (tracking == 2) {
			
			camR -= yy - startY;
			if (camR < 3)
				camR = 3.0;
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


		alphaAux = camAlpha + deltaX;
		betaAux = camBeta + deltaY;

		if (betaAux > 85.0)
			betaAux = 85.0;
		else if (betaAux <	-85.0)
			betaAux = -85.0;

		rAux = camR;
	}
	else if (tracking == 2) {

		alphaAux = camAlpha;
		betaAux = camBeta;
		rAux = camR - deltaY;
		if (rAux < 3)
			rAux = 3;
	}
	float lx = cos(alphaAux * 3.14 / 180.0);
	float ly = -sin(betaAux * 3.14 / 180.0);
	float lz = sin(alphaAux * 3.14 / 180.0);

	camLx = camPx + lx;
	camLy = camPy + ly;
	camLz = camPz + lz;

	printf("%f %f %f\n", camLx, camLy, camLz);
}

std::vector<PONTO> generateTreesRand(){
	std::vector<PONTO> pontos;

	for(int i = 0; i < numberTrees; i++){
		PONTO p = (PONTO) malloc(sizeof(PONTO));
		do{
			p->x = rand() % imageWidth - (imageWidth/2); 
			p->z = rand() % imageHeight - (imageHeight/2);
		} while(sqrt(pow(p->x,2) + pow(p->z,2)) < rt);
		p->y = hf(p->x, p->z);
		pontos.emplace_back(p);
	}

	return pontos;
}

void init() {

// 	Load the height map "terreno.jpg"
	
	unsigned int t;

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

	pontosArvores = generateTreesRand();

// 	OpenGL settings
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	camPx = 0;
	camPz = 0;
	camPy = camHeight + hf(camPx, camPz);
	
	camLx = 0;
	camLz = 1;
	camLy = camHeight + hf(camPx, camPz);
		
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

