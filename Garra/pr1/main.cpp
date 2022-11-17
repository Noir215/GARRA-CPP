/*
*
*	Practica Final Ángel Dolz
*
*/
#include <GL/glut.h>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <chrono>
#include <cmath>

using namespace std;

int screenHeight = 480;
int screenWidth = 640;

//VARIABLES GLOBALES DEL PROGRAMA

//Booleanos
bool Idle_ext_1 = false;
bool Idle_ext_2 = false;
bool Extendido_a = false;
bool Extendido_b = false;
bool extender_1 = false;
bool mov_cam = false;
bool agarrar = false;
bool colision = false;
bool vista = false;

//Enteros
int yold = 0;
int xold = 0;

//Floats
float tecta = 45;
float phi = 45;
float r = 25;		//Radio

float Rot_0 = 0;
float Rot_a = 0;
float Rot_b = 0;
float Rot_c = 0;
float Rot_d = 0;
float Rot_e = 0;

float Ext_1 = 0;
float Ext_2 = 0;

float dx = 0;		//Dedo x
float dy = 0;		//Dedo y
float dz = 0;		//Dedo z

//Ctes
const float rad = 0.0174533;
const float MAX = 4;

//Estructuras
struct punto {
	float x;
	float y;
	float z;
};

punto camara = { r * sin(tecta * rad) * cos(phi * rad), r * cos(tecta * rad), r * sin(tecta * rad) * sin(phi * rad) };
punto centro = { 0, 0, 0 };
punto up = { 0, 1, 0 };

//Matrices
float matriz_mano[16];
float matriz_tetera[16];
float matriz_distancia[16];

//DEFINICION DE FUNCIONES DE DIBUJADO BASE

//Dibujo base
void dibujar_base() {
	glColor3f(1, 0, 0);		// Color Rojo
	glutWireCube(1);
}

//Dibujo peana
void dibujar_peana() {
	glPushMatrix();
	glColor3f(1, 1, 1);		// Color Blanco
	glScalef(0.5, 3, 0.5);
	glutWireCube(1);
	glPopMatrix();
}

//Dibujo codo
void dibujar_codo() {
	glColor3f(1, 0, 0);		// Color Rojo
	glutWireSphere(0.5, 10, 10);
}

//Dibujo antebrazo
void dibujar_antebrazo() {
	glPushMatrix();
	glColor3f(1, 1, 1);		// Color Blanco
	glScalef(3 + Ext_1, 0.5, 0.5);
	glutWireCube(1);
	glPopMatrix();
}

//Dibujo brazo
void dibujar_brazo() {
	glPushMatrix();
	glColor3f(1, 1, 1);		// Color Blanco
	glScalef(0.5, 2 + Ext_2, 0.5);
	glutWireCube(1);
	glPopMatrix();
}

//Dibujo dedos
void dibujar_dedos() {
	glPushMatrix();
	glColor3f(1, 1, 0);		// Color Amarillo
	glScalef(0.05, 1, 0.05);
	glutWireCube(1);
	glPopMatrix();
}

//Dibujo mano
void dibujar_mano() {
	dibujar_codo();
	for (int i = 0; i < 3; i++) {
		glPushMatrix();
		glRotatef(Rot_d, dx, dy, dz);
		glTranslatef(0.5, -0.5, (i * 0.3) - 0.3);
		dibujar_dedos();
		glPopMatrix();
	}
	
	glPushMatrix();
	glRotatef(Rot_e, 0, 0, 1);
	glTranslatef(-0.5, -0.5, 0);
	dibujar_dedos();
	glPopMatrix();
}

//Dibujo taza
void dibujar_taza() {
	glColor3f(0, 1, 0);		// Color Amarillo
	glutWireTeapot(0.7);
}

//Dibujo escena
void dibujar_escena() {
	// Dibujado de la base
	glPushMatrix();
	dibujar_base();
	glPopMatrix();

	// Dibujado de la peana
	glPushMatrix();
	glTranslatef(0, 2, 0);
	glRotatef(Rot_0, 0, 1, 0);
	dibujar_peana();

	// Dibujado del primer codo
	glPushMatrix();
	glTranslatef(0, 2, 0);
	glRotatef(Rot_a, 0, 0, 1);
	dibujar_codo();

	// Dibujado del antebrazo
	glPushMatrix();
	glTranslatef(2 + Ext_1 / 2, 0, 0);
	dibujar_antebrazo();

	// Dibujado del segundo codo
	glPushMatrix();
	glTranslatef(2 + Ext_1 / 2, 0, 0);
	glRotatef(Rot_b, 0, 0, 1);
	dibujar_codo();

	// Dibujado del brazo
	glPushMatrix();
	glTranslatef(0, -1.5 - Ext_2 / 2, 0);
	dibujar_brazo();

	// Dibujado de la mano
	glPushMatrix();
	glTranslatef(0, -1.5 - Ext_2 / 2, 0);
	glRotatef(-Rot_c, 0, 0, 1);
	glGetFloatv(GL_MODELVIEW_MATRIX, matriz_mano);
	dibujar_mano();

	if (agarrar && colision) {
		glPushMatrix();
		glTranslatef(0, -1, 0);
		dibujar_taza();
		glPopMatrix();
	}

	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

	if (!agarrar) {
		glTranslatef(8, 0, 0);
		glGetFloatv(GL_MODELVIEW_MATRIX, matriz_tetera);
		dibujar_taza();
		glPopMatrix();
	}

	for (int i = 12; i < 15; i++)
	{
		matriz_distancia[i] = matriz_tetera[i] - matriz_mano[i];
	}
	if (abs(matriz_distancia[12]) < 1 && abs(matriz_distancia[13]) < 1 && abs(matriz_distancia[14]) < 1)
		colision = true;
}

//DEFINICION DE FUNCIONES DE CALLBACK

//Control por ratón
void myMouse(int button, int state, int mx, int  my) {
	//glLoadIdentity();

	if (!mov_cam) {
		if (button == GLUT_LEFT_BUTTON) {
			if (!Idle_ext_1)		Idle_ext_1 = true;
			else if (Idle_ext_1)	Idle_ext_1 = false;
		}
		if (button == GLUT_RIGHT_BUTTON) {
			if (!Idle_ext_2)		Idle_ext_2 = true;
			else if (Idle_ext_2)	Idle_ext_2 = false;
		}
	}
	glutPostRedisplay();
}

//Cambia las dimensiones de la pantalla
void myReshape(int w, int h) {
	screenWidth = w;
	screenHeight = h;
}

//Funcion iddle
void myIdle()
{
	static long int  Time = 0;

	// Número de milisegundos que han pasado desde que se inicio el programa
	long int currentTime = glutGet(GLUT_ELAPSED_TIME);

	if (currentTime - Time > 10) {
		Time = currentTime;

		if (Idle_ext_1) {
			if (Ext_1 <= MAX && !Extendido_a)
				Ext_1 += 0.1;
			else if (Ext_1 >= 0 && Extendido_a)
				Ext_1 -= 0.1;

			if (Ext_1 >= MAX)	Extendido_a = true;
			if (Ext_1 <= 0)		Extendido_a = false;
		}
		if (Idle_ext_2) {
			if (Ext_2 <= MAX && !Extendido_b)
				Ext_2 += 0.1;
			else if (Ext_2 >= 0 && Extendido_b)
				Ext_2 -= 0.1;

			if (Ext_2 >= MAX)	Extendido_b = true;
			if (Ext_2 <= 0)		Extendido_b = false;
		}
	}
	glutPostRedisplay();
}

//Control por teclado
void myKeys(unsigned char key, int x, int y) {
	switch (key)
	{
		case 'q': exit(0);
		case 'v': case 'V':
			if (!vista)			vista = true;
			else if (vista)		vista = false;
			break;
		case 'j': case 'J':
			if (!mov_cam)		mov_cam = true;
			else if (mov_cam)	mov_cam = false;
			break;
		case 'p': case 'P':
			Rot_0 += 5;
			break;
		case 'o': case 'O':
			Rot_0 -= 5;
			break;
		case 'a': case 'A':
			if (Rot_a < 90)
				Rot_a += 5;
			break;
		case 's': case 'S':
			if (Rot_a > 0)
				Rot_a -= 5;
			break;
		case 'b': case 'B':
			if (Rot_b < 180)
				Rot_b += 5;
			break;
		case 'n': case 'N':
			if (Rot_b > 0)
				Rot_b -= 5;
			break;
		case 'c': case 'C':
			if (Rot_c < 90)
				Rot_c += 5;
			break;
		case 'x': case 'X':
			if (Rot_c > 0)
				Rot_c -= 5;
			break;
		case 'h': case 'H':
			dx = 1;		
			dy = 0;
			dz = 0;
			Rot_d += 5;
			break;
		case 'i': case 'I':
			dy = 1;
			dx = 0;
			dz = 0;
			Rot_d += 5;
			break;
		case 'r': case 'R':
			dz = 1;
			dx = 0;
			dy = 0;
			Rot_d += 5;
			break;
		case 'g': case 'G':
			dx = 1;
			dy = 0;
			dz = 0;
			Rot_d -= 5;
			break;
		case 'u': case 'U':
			dy = 1;
			dx = 0;
			dz = 0;
			Rot_d -= 5;
			break;
		case 't': case 'T':
			dz = 1;
			dx = 0;
			dy = 0;
			Rot_d -= 5;
			break;
		case 'w': case 'W':
			Rot_e += 5;
			break;
		case 'e': case 'E':
			Rot_e -= 5;
			break;
		case 'k': case 'K':
			if (!agarrar && colision)		agarrar = true;
			else if (agarrar)				agarrar = false;	colision = false;
			break;
	}
	glutPostRedisplay();
}

//Control de la camara
void mov_raton(int x, int y) {
	if (mov_cam) {
		if (yold - y > 0)
			tecta += 3;
		if (yold - y < 0)
			tecta -= 3;

		if (tecta > 359)
			tecta = 0;
		if (tecta < 0)
			tecta = 359;

		if (xold - x > 0)
			phi -= 3;
		if (xold - x < 0)
			phi += 3;

		if (tecta > 0 && tecta < 180)
			up.y = 1;
		else
			up.y = -1;

		xold = x;
		yold = y;
		camara.x = r * sin(tecta * rad) * cos(phi * rad);
		camara.y = r * cos(tecta * rad);
		camara.z = r * sin(tecta * rad) * sin(phi * rad);

		glutPostRedisplay();
	}
}

//Dibuja las 4 perspectivas
static void perspectiva(void) {
	glViewport(0, 0, screenWidth / 2, screenHeight / 2);
	glLoadIdentity();
	gluLookAt(0, 0, -20, 0, 0, 0, 0, 1, 0);
	dibujar_escena();
	glViewport(screenWidth / 2, 0, screenWidth / 2, screenHeight / 2);
	glLoadIdentity();
	gluLookAt(0, 0, 20, 0, 0, 0, 0, 1, 0);
	dibujar_escena();
	glViewport(0, screenHeight / 2, screenWidth / 2, screenHeight / 2);
	glLoadIdentity();
	gluLookAt(0, 20, 0, 0, 0, 0, 1, 0, 0);
	dibujar_escena();
	glViewport(screenWidth / 2, screenHeight / 2, screenWidth / 2, screenHeight / 2);
	glLoadIdentity();
	gluLookAt(10, 10, 10, 0, 0, 0, 0, 1, 0);
	dibujar_escena();
}

//Dibuja el raster
void myDisplay(void) {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (vista) {
		perspectiva();
	}
	else if (!vista){
		glViewport(0, 0, screenWidth, screenHeight);
		gluLookAt(camara.x, camara.y, camara.z, centro.x, centro.y, centro.z, up.x, up.y, up.z);
		dibujar_escena();
	}

	glFlush();
}

//Funcion main del programa
int main(int argc, char ** argv) {
	//CREACION DEL ENTORNO GRAFICO
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(30, 30);
	glutInit(&argc, argv);
	glutCreateWindow("Lab. Inf. Grafica || Ángel Dolz González");
	//--------------------


	//Damos de alta a las funciones de Callback
	glutKeyboardFunc(myKeys);
	glutMouseFunc(myMouse);
	glutMotionFunc(mov_raton);
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);
	glutIdleFunc(myIdle);
	//--------------------

	//Funciones de limpieza de la pantalla
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//---------------------

	glFlush();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Puedes experimentar con dos tipos de vista diferente (Ortografica o Perspectiva) comentando o descomentando
	//una de las dos siguientes funciones (deja activa solo una de ellas)

	//glOrtho(-6.0,6.0,-6.0,6.0,-5,5);  //Vista ortográfica en 3D
	gluPerspective(45, 4.0 / 3.0, 0.1, 100); //Vista perpectiva en 3D

	glMatrixMode(GL_MODELVIEW);
	//x,y,anchura,altura
	glViewport(0, 0, 640, 480);

	//Llamada al bucle principal de recoleccion de eventos
	glutMainLoop();
}