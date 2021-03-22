#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES // always before the include#include <math.h>
#include <math.h>
#include <list>
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
	std::list<PONTO> *pontos;
} *CILINDRO;

typedef struct cameraExplorer{
	float angleX;
	float angleY;
	float distance;
} *CAMERAEXPLORER;

typedef struct player{
	PONTO pos;
	CAMERAEXPLORER cameraExplorer;
	PONTO looking;
	bool cameraModeExplorer;
} *PLAYER;

CILINDRO _cilindro = NULL;
float _altura = 10;
float _raio = 5;
int _slices = 20;
PLAYER _player;
GLenum _drawMode = GL_FILL;

float _x = 0.0f;
float _y = 0.0f;
float _z = 0.0f;
float _angle = 0.0f;


std::list<PONTO> geraPontosCilindro(int slices, float altura, float raio){
	std::list<PONTO> ret;

	for(int i = 0; i < slices; i++){
		float alpha1 = i * 2 * M_PI / slices;
		float alpha2 = (i + 1) * 2 * M_PI / slices;

		auto p1 = (PONTO) malloc(sizeof(PONTO));
		p1 -> x = 0;
		p1 -> y = altura/2; 
		p1 -> z = 0;

		auto p2 = (PONTO) malloc(sizeof(PONTO));
		p2 -> x = raio * sin(alpha1);
		p2 -> y = altura/2; 
		p2 -> z = raio * cos(alpha1);

		auto p3 = (PONTO) malloc(sizeof(PONTO));
		p3 -> x = raio * sin(alpha2);
		p3 -> y = altura/2; 
		p3 -> z = raio * cos(alpha2);

		auto p4 = (PONTO) malloc(sizeof(PONTO));
		p4 -> x = 0;
		p4 -> y = -altura/2;  
		p4 -> z = 0;

		auto p5 = (PONTO) malloc(sizeof(PONTO));
		p5 -> x = raio * sin(alpha1);
		p5 -> y = -altura/2; 
		p5 -> z = raio * cos(alpha1);

		auto p6 = (PONTO) malloc(sizeof(PONTO));
		p6 -> x = raio * sin(alpha2);
		p6 -> y = -altura/2; 
		p6 -> z = raio * cos(alpha2);

        ret.push_front(p3);
		ret.push_front(p2);
        ret.push_front(p1);

		ret.push_front(p6);
        ret.push_front(p2);
        ret.push_front(p3);

		ret.push_front(p6);
        ret.push_front(p5);
        ret.push_front(p2);

		ret.push_front(p5);
        ret.push_front(p6);
        ret.push_front(p4);

	}

	return ret;
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
	if(_player -> cameraModeExplorer){
    	gluLookAt(_player -> pos -> x, _player -> pos -> y, _player -> pos -> z,
            0.0,0.0,0.0,
            0.0f,1.0f,0.0f);
	}
	else{
		gluLookAt(_player -> pos -> x, _player -> pos -> y, _player -> pos -> z,
            _player -> looking -> x, _player -> looking -> y, _player -> looking -> z,
            0.0f,1.0f,0.0f);
	}


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

    // put drawing instructions here
	// Desenha a figura
    auto iter = _cilindro -> pontos -> begin();
    while (iter != _cilindro -> pontos -> end()) {
    	PONTO p1 = *iter;
        iter++;
        PONTO p2 = *iter;
        iter++;
        PONTO p3 = *iter;
        iter++;

        glBegin(GL_TRIANGLES);
        	glColor3f(1.0f, 1.0f, 1.0f);
            glVertex3f(p1->x, p1->y, p1->z);
            glVertex3f(p2->x, p2->y, p2->z);
            glVertex3f(p3->x, p3->y, p3->z);
        glEnd();
    }

	// End of frame
	glutSwapBuffers();
}

void recenterPlayer(){
	_player -> pos -> x = _player -> cameraExplorer -> distance * sin(_player -> cameraExplorer -> angleX) * cos(_player -> cameraExplorer -> angleY) ; 
	if((_player -> cameraExplorer -> angleY >= M_PI / 2 && _player -> cameraExplorer -> angleY <= 3 * M_PI / 2) || (_player -> cameraExplorer -> angleY <= - M_PI / 2 && _player -> cameraExplorer -> angleY >= - 3 * M_PI / 2))
		_player -> pos -> y = - _player -> cameraExplorer -> distance * sin(_player -> cameraExplorer -> angleY);
	else 
		_player -> pos -> y = _player -> cameraExplorer -> distance * sin(_player -> cameraExplorer -> angleY);
	_player -> pos -> z = _player -> cameraExplorer -> distance * cos(_player -> cameraExplorer -> angleX) * cos(_player -> cameraExplorer -> angleY); 		

}

// write function to process keyboard events
void keyEvent(unsigned char key, int x, int y){
	switch(key){
		case 'w':
			if(_player -> cameraModeExplorer){
				_player -> cameraExplorer -> angleY += M_PI / 90;
				if(_player -> cameraExplorer -> angleY > 2 * M_PI) _player -> cameraExplorer -> angleY -= 2 * M_PI;
				recenterPlayer();
			}
			else{
 				_player -> looking -> x = 1 + _player -> pos -> x;
			}
			glutPostRedisplay();
			break;
		case 's':
			if(_player -> cameraModeExplorer){
				_player -> cameraExplorer -> angleY -= M_PI / 90;
				if(_player -> cameraExplorer -> angleY > 2 * M_PI) _player -> cameraExplorer -> angleY -= 2 * M_PI;
				recenterPlayer();
			}
			else{
				 _player -> looking -> x = 1 - _player -> pos -> x;
			}
			glutPostRedisplay();
			break;
		case 'a':
			if(_player -> cameraModeExplorer){
 				_player -> cameraExplorer -> angleX += M_PI / 90;
				recenterPlayer();
			}
			else  _player -> looking -> z = 1 + _player -> pos -> z;
			glutPostRedisplay();
			break;
		case 'd':
			if(_player -> cameraModeExplorer){
				_player -> cameraExplorer -> angleX -= M_PI / 90;
				recenterPlayer();
			} 
			else _player -> looking -> z = 1 - _player -> pos -> z;
			glutPostRedisplay();
			break;
		case '+':
			if(_player -> cameraModeExplorer){
				_player -> cameraExplorer -> distance += 1;
				recenterPlayer();
			}
			else _player -> looking -> y = 1 + _player -> pos -> y;
			glutPostRedisplay();
			break;
		case '-':
			if(_player -> cameraModeExplorer){
				_player -> cameraExplorer -> distance -= 1;
				recenterPlayer();
			}
			else _player -> looking -> y = 1 - _player -> pos -> y;
			glutPostRedisplay();
			break;
		case 'm':
			if(_drawMode == GL_FILL) _drawMode = GL_LINE;
			else if(_drawMode == GL_LINE) _drawMode = GL_FILL;
			glutPostRedisplay();
			break;
		case 'c':
			_player -> cameraModeExplorer = !(_player -> cameraModeExplorer);
			if(_player -> cameraModeExplorer){
				_player -> looking -> x = _player -> looking -> y = _player -> looking -> z = 0;
				_player -> cameraExplorer -> distance = _player -> pos -> y / sin(_player -> cameraExplorer -> angleY);
			} 
			glutPostRedisplay();
			break;
		default:
			break;
	}

}


void startStructures(){
	_player = (PLAYER) malloc(sizeof(PLAYER));
	PONTO posPlayer = (PONTO) malloc(sizeof(PONTO));
	posPlayer -> x = 20;
	posPlayer -> y = 20;
	posPlayer -> z = 20;
	_player -> pos = posPlayer;
	CAMERAEXPLORER cameraExplorer = (CAMERAEXPLORER) malloc(sizeof(CAMERAEXPLORER));
	cameraExplorer -> angleX = 0;
	cameraExplorer -> angleY = 0;
	cameraExplorer -> distance = 20;
	_player -> cameraExplorer = cameraExplorer;
	PONTO looking = (PONTO) malloc(sizeof(PONTO));
	looking -> x = 0;
	looking -> y = 0;
	looking -> z = 0;
	_player -> looking = looking;
	_player -> cameraModeExplorer = true;
	recenterPlayer();

	static std::list<PONTO> pontos = geraPontosCilindro(_slices, _altura, _raio);
	_cilindro = (CILINDRO) malloc(sizeof(CILINDRO));
	_cilindro -> slices = _slices;
	_cilindro -> altura = _altura;
	_cilindro -> raio = _raio;
	_cilindro -> pontos = &pontos;
}

int main(int argc, char **argv) {
	
	startStructures();

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("CG@DI-UM");

	// Required callback registry
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	// put here the registration of the keyboard callbacks
	glutKeyboardFunc(keyEvent);

	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
