#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES // always before the include#include <math.h>
#include <math.h>
#include <iostream>

typedef struct ponto{
	float x;
	float y;
	float z;
} *PONTO;

typedef struct cilindro{
	int slices;
	float altura;
	float raio;
	int nrPontos;
	float* pontos;
} *CILINDRO;

int _size;
float _altura = 10;
float _raio = 5;
int _slices = 20;
GLenum _drawMode = GL_FILL;

float _x = 0.0f;
float _y = 0.0f;
float _z = 0.0f;
float _angle = 0.0f;

GLuint buffers[1];

int timebase;
float frames = 0;
float fps;
float _time;
char showFrames[256];

void geraPontosCilindro(int slices, float altura, float raio){
	_size = 3 * 4 * 3 * _slices;
	int pos = 0;

	float* pontos = (float*) malloc(_size * sizeof(float));

	for(int i = 0; i < slices; i++){
		float alpha1 = i * 2 * M_PI / slices;
		float alpha2 = (i + 1) * 2 * M_PI / slices;

        pontos[pos++] = 0; pontos[pos++] = altura/2; pontos[pos++] = 0; //p1
        pontos[pos++] = raio * sin(alpha1); pontos[pos++] = altura/2; pontos[pos++] = raio * cos(alpha1); //p2
		pontos[pos++] = raio * sin(alpha2); pontos[pos++] = altura/2; pontos[pos++] = raio * cos(alpha2); //p3

        pontos[pos++] = raio * sin(alpha2); pontos[pos++] = altura/2; pontos[pos++] = raio * cos(alpha2); //p3
        pontos[pos++] = raio * sin(alpha1); pontos[pos++] = altura/2; pontos[pos++] = raio * cos(alpha1); //p2
		pontos[pos++] = raio * sin(alpha2); pontos[pos++] = -altura/2; pontos[pos++] = raio * cos(alpha2); //p6

        pontos[pos++] = raio * sin(alpha1); pontos[pos++] = altura/2; pontos[pos++] = raio * cos(alpha1); //p2
        pontos[pos++] = raio * sin(alpha1); pontos[pos++] = -altura/2; pontos[pos++] = raio * cos(alpha1); //p5
		pontos[pos++] = raio * sin(alpha2); pontos[pos++] = -altura/2; pontos[pos++] = raio * cos(alpha2); //p6

        pontos[pos++] = 0; pontos[pos++] = -altura/2; pontos[pos++] = 0; //p4
        pontos[pos++] = raio * sin(alpha2); pontos[pos++] = -altura/2; pontos[pos++] = raio * cos(alpha2); //p6
		pontos[pos++] = raio * sin(alpha1); pontos[pos++] = -altura/2; pontos[pos++] = raio * cos(alpha1); //p5

	}

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, _size * sizeof(float), pontos, GL_STATIC_DRAW);
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


void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//GL_LINES, GL_FILL
    glPolygonMode(GL_FRONT, _drawMode);

	// set the camera
	glLoadIdentity();
	gluLookAt(20.0,20.0,20.0,
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

	frames++;
	_time = glutGet(GLUT_ELAPSED_TIME);
	if (_time - timebase > 1000) {
		fps = frames * 1000.0 / (_time - timebase);
		timebase = _time;
		frames = 0;
	}
	sprintf(showFrames,"Frames: %f", fps);
	glutSetWindowTitle(showFrames);

	// Desenha os eixos
	glBegin(GL_LINES);
		// X axis in red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f( 100.0f, 0.0f, 0.0f);
		// Y Axis in Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);
		// Z Axis in Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();

    // put the geometric transformations here
	glTranslatef(_x,_y,_z);
	glRotatef(_angle,0,1,0);


    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, _size);

	// End of frame
	glutSwapBuffers();
}

// write function to process keyboard events
void keyEvent(unsigned char key, int x, int y){
	switch(key){
		case 'w':
			_x += 0.1;
			glutPostRedisplay();
			break;
		case 's':
			_x -= 0.1;
			glutPostRedisplay();
			break;
		case 'a':
			_z += 0.1;
			glutPostRedisplay();
			break;
		case 'd':
			_z -= 0.1;
			glutPostRedisplay();
			break;
		case 'e':
			_angle += 1;
			glutPostRedisplay();
			break;
		case 'q':
			_angle -= 1;
			glutPostRedisplay();
			break;
		case 'z':
			_y += 0.1;
			glutPostRedisplay();
			break;
		case 'x':
			_y -= 0.1;
			glutPostRedisplay();
			break;
		default:
			break;
		case 'm':
			if(_drawMode == GL_FILL) _drawMode = GL_LINE;
			else if(_drawMode == GL_LINE) _drawMode = GL_FILL;
			glutPostRedisplay();
			break;
	}
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

	glewInit();

	timebase = glutGet(GLUT_ELAPSED_TIME);

	// put here the registration of the keyboard callbacks
	glutKeyboardFunc(keyEvent);

	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
    glEnableClientState(GL_VERTEX_ARRAY);

    glGenBuffers(1, buffers);
    geraPontosCilindro(_slices, _altura, _raio);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
