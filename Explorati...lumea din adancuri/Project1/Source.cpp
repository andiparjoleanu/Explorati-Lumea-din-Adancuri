/*
* Copyright (c) 1993-1997, Silicon Graphics, Inc.
* ALL RIGHTS RESERVED
* Permission to use, copy, modify, and distribute this software for
* any purpose and without fee is hereby granted, provided that the above
* copyright notice appear in all copies and that both the copyright notice
* and this permission notice appear in supporting documentation, and that
* the name of Silicon Graphics, Inc. not be used in advertising
* or publicity pertaining to distribution of the software without specific,
* written prior permission.
*
* THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
* AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
* INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
* FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
* GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
* SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
* KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
* LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
* THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
* ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
* POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
*
* US Government Users Restricted Rights
* Use, duplication, or disclosure by the Government is subject to
* restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
* (c)(1)(ii) of the Rights in Technical Data and Computer Software
* clause at DFARS 252.227-7013 and/or in similar or successor
* clauses in the FAR or the DOD or NASA FAR Supplement.
* Unpublished-- rights reserved under the copyright laws of the
* United States.  Contractor/manufacturer is Silicon Graphics,
* Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
*
* OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
*/

/*  bezcurve.c
*  This program uses evaluators to draw a Bezier curve.
*/

#include <GL/glut.h>
#include <stdlib.h>

GLfloat ctrlpoints[4][3] =                     //PUNCTELE INITIALE
{
	{ -4.0, -4.0, 0.0 },
    { -2.0, 4.0, 0.0 },
	{ 2.0, -4.0, 0.0 },
	{ 4.0, 4.0, 0.0 } 
};

GLfloat colctrlpoints[4][4] =					//DE LA GALBEN, LA ALBASTRU
{
	{ 1.0, 1.0, 0.0, 1.0 },
	{ 0.9, 0.5, 0.0, 1.0 },
	{ 0.2, 0.4, 0.3, 1.0 },
	{ 0.0, 0.0, 1.0, 1.0 } 
};

void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glShadeModel(GL_FLAT);
	
	//GL_MAP1_VERTEX_3 - fiecare vertex e un vector cu 3 coordonate
	//u1 = 0.0, u2 = 1.0, stride(distanta dintre 2 puncte de control) = 3, numarul punctelor de control = 4
	//pointer catre primul punct

	glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, ctrlpoints[0]);
	glMap1f(GL_MAP1_COLOR_4, 0.0, 1.0, 4, 4, colctrlpoints[0]);
	glEnable(GL_MAP1_VERTEX_3);
	glEnable(GL_MAP1_COLOR_4);
}

void display(void)
{
	int i;

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(3.0);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i <= 5; i++)
		glEvalCoord1f((GLfloat)i / 5.0);
	glEnd();
	/* The following code displays the control points as dots. */
	glPointSize(10.0);
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_POINTS);
	for (i = 0; i < 4; i++)
		glVertex3fv(& ctrlpoints[i][0]);
	glColor3fv (& colctrlpoints[i][0]);
	glEnd();
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-5.0, 5.0, -5.0*(GLfloat)h / (GLfloat)w,
			5.0*(GLfloat)h / (GLfloat)w, -5.0, 5.0);
	else
		glOrtho(-5.0*(GLfloat)w / (GLfloat)h,
			5.0*(GLfloat)w / (GLfloat)h, -5.0, 5.0, -5.0, 5.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) 
	{
	case 27:
		exit(0);
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}
