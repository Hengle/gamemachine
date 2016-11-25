#include <windows.h>
#include "glut.h"
#include "objreader/objreader.h"
#include "utilities/path.h"
#include "utilities/camera.h"
#include "utilities/assert.h"
#include "gameloop/gameloop.h"
#include "io/keyboard.h"

using namespace fglextlib;

float width = 600;
float height = 300;
GLfloat centerX = 0, centerY = 0, centerZ = 149;
GLfloat eyeX = 0, eyeY = 0, eyeZ = 150;

ObjReader reader(ObjReader::LoadOnly);
Camera camera;

Ffloat fps = 60;
GameLoopSettings s = { fps };
class GameHandler : public IGameHandler
{
public:
	void setGameLoop(GameLoop* l)
	{
		m_gl = l;
	}

	void render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30, 2, 10, 500);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		CameraUtility::fglextlib_gl_LookAt(camera);

		glColor3f(1, 1, 1);
		reader.draw();

		glFlush();
	}

	void mouse()
	{
		int wx = glutGet(GLUT_WINDOW_X),
			wy = glutGet(GLUT_WINDOW_Y);
		camera.mouseReact(wx, wy, width, height);
	}

	void keyboard()
	{
		Ffloat dis = 50;
		Ffloat v = dis / fps;
		if (Keyboard::isKeyDown(VK_ESCAPE) || Keyboard::isKeyDown('Q'))
			m_gl->terminate();
		if (Keyboard::isKeyDown('A'))
			camera.moveRight(-v);
		if (Keyboard::isKeyDown('D'))
			camera.moveRight(v);
		if (Keyboard::isKeyDown('W'))
			camera.moveFront(v);
		if (Keyboard::isKeyDown('S'))
			camera.moveFront(-v);
	}

	GameLoop* m_gl;
};

GameHandler handler;
GameLoop gl(s, &handler);

void init()
{
	int wx = glutGet(GLUT_WINDOW_X),
		wy = glutGet(GLUT_WINDOW_Y);
	camera.mouseInitReaction(wx, wy, width, height);
	handler.setGameLoop(&gl);

	std::string path = Path::getCurrentPath();
	reader.load(path.append("cat.obj").c_str());
	camera.setSensibility(.25f);
	camera.setPosition(eyeX, eyeY, eyeZ);

	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	GLfloat pos[] = { 150, 150, 150, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, pos);

	GLfloat clr[] = { 1, 1, 1, 1 };
	GLfloat b[] = { 1, 1, 1, 1 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, clr);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, clr);
	glLightfv(GL_LIGHT0, GL_SPECULAR, b);
	glEnable(GL_LIGHT0);
}

void render()
{
}

void resharp(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
}

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	char * lpCmdLine,
	int nCmdShow
)
{
	int argc = 1;
	char* l = "";
	char* argv[1];
	argv[0] = l;
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(400, 400);
	glutCreateWindow("Render");

	init();
	glutReshapeFunc(resharp);
	glutDisplayFunc(render);

	GameLoopUtilities::fglextlib_gl_registerGameLoop(gl);

	glutMainLoop();

	return 0;
}