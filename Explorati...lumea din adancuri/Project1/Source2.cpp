#include<gl/freeglut.h>
#include<math.h>
#include <iostream>
#include "SOIL.h"
#include <vector>
using namespace std;

bool createPhoto;

class Point
{
	float x, y, z;
public:
	Point();
	Point(float, float, float);
	Point & operator = (const Point &);
	float getx() const;
	float gety() const;
	float getz() const;

};

Point::Point()
{
	x = 0;
	y = 0;
	z = 0;
}

Point::Point(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Point & Point :: operator = (const Point & p)
{
	this->x = p.x;
	this->y = p.y;
	this->z = p.z;
	return * this;
}

float Point::getx() const  
{
	return x;
}

float Point::gety() const
{
	return y;
}

float Point::getz() const 
{
	return z;
}

void print(string s, float x, float y, float z, int font = 0, int dim = 0)
{
	glRasterPos3f(x, y, z);
	for (int i = 0; i < s.length(); i++)
	{
		if (font == 0 && dim == 0)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, s[i]);
		else if (font == 1 && dim == 1)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
		else glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
	}
}


class Observer
{
	static Observer * ob;
	Point eye;
	float dx, dy, dz;
	Observer() : eye(0.0, 60.0, 80.0), dx(0.0), dy(0.0), dz(-1.0) {};
public:
	static Observer * getInstance();
	Point getEye();
	void setEye(Point &);
	float getdx();
	float getdy();
	float getdz();
	void setdx(float);
	void setdy(float);
	void setdz(float);
};
Observer * Observer::ob = NULL;

Observer * Observer::getInstance()
{
	if (ob == NULL)
		ob = new Observer();
	return ob;
}

Point Observer::getEye()
{
	return eye;
}

void Observer::setEye(Point & point)
{
	eye = point;
}

float Observer::getdx()
{
	return dx;
}

float Observer::getdy()
{
	return dy;
}

float Observer::getdz()
{
	return dz;
}

void Observer::setdx(float dx)
{
	this -> dx = dx;
}

void Observer::setdy(float dy)
{
	this->dx = dy;
}

void Observer::setdz(float dz)
{
	this->dz = dz;
}

enum { X, Y, Z, W };
enum { A, B, C, D };

void shadowmatrix(GLfloat shadowMat[4][4], GLfloat groundplane[4], GLfloat lightpos[4])
{
	GLfloat dot;

	dot = groundplane[X] * lightpos[X] +
		groundplane[Y] * lightpos[Y] +
		groundplane[Z] * lightpos[Z] +
		groundplane[W] * lightpos[W];

	shadowMat[0][0] = dot - lightpos[X] * groundplane[X];
	shadowMat[1][0] = 0.f - lightpos[X] * groundplane[Y];
	shadowMat[2][0] = 0.f - lightpos[X] * groundplane[Z];
	shadowMat[3][0] = 0.f - lightpos[X] * groundplane[W];

	shadowMat[X][1] = 0.f - lightpos[Y] * groundplane[X];
	shadowMat[1][1] = dot - lightpos[Y] * groundplane[Y];
	shadowMat[2][1] = 0.f - lightpos[Y] * groundplane[Z];
	shadowMat[3][1] = 0.f - lightpos[Y] * groundplane[W];

	shadowMat[X][2] = 0.f - lightpos[Z] * groundplane[X];
	shadowMat[1][2] = 0.f - lightpos[Z] * groundplane[Y];
	shadowMat[2][2] = dot - lightpos[Z] * groundplane[Z];
	shadowMat[3][2] = 0.f - lightpos[Z] * groundplane[W];

	shadowMat[X][3] = 0.f - lightpos[W] * groundplane[X];
	shadowMat[1][3] = 0.f - lightpos[W] * groundplane[Y];
	shadowMat[2][3] = 0.f - lightpos[W] * groundplane[Z];
	shadowMat[3][3] = dot - lightpos[W] * groundplane[W];

}

void findplane(GLfloat plane[4], GLfloat v0[3], GLfloat v1[3], GLfloat v2[3])
{
	GLfloat vec0[3], vec1[3];

	vec0[X] = v1[X] - v0[X];
	vec0[Y] = v1[Y] - v0[Y];
	vec0[Z] = v1[Z] - v0[Z];

	vec1[X] = v2[X] - v0[X];
	vec1[Y] = v2[Y] - v0[Y];
	vec1[Z] = v2[Z] - v0[Z];

	plane[A] = vec0[Y] * vec1[Z] - vec0[Z] * vec1[Y];
	plane[B] = -(vec0[X] * vec1[Z] - vec0[Z] * vec1[X]);
	plane[C] = vec0[X] * vec1[Y] - vec0[Y] * vec1[X];

	plane[D] = -(plane[A] * v0[X] + plane[B] * v0[Y] + plane[C] * v0[Z]);
}

GLfloat floorshadow[4][4];
GLfloat lightpos[] = { -50.f, -100.f, -100.f, 1.f };
GLfloat wall_mat[] = { 1.f, 1.f, 1.f, 1.f };

class Texture
{
	unsigned int id;
	const char * path;
public:
	Texture(const char *);
	void loadTexture();
	unsigned int getId();
	const char * getPath();
};

Texture::Texture(const char * path)
{
	this -> path = path;
}

void Texture::loadTexture()
{
	glGenTextures(1, & id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	int width, height;
	unsigned char * image = SOIL_load_image(path, & width, & height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
}

unsigned int Texture::getId()
{
	return id;
}

const char * Texture::getPath()
{
	return path;
}


Texture binocular("C:\\Project1\\Photos\\binocular.png");
Texture ocean("C:\\Project1\\Photos\\jSLNscz.jpg");
Texture oceanpad("C:\\Project1\\Photos\\sea-blue-ocean-turquoise.jpg");
Texture coralsphere("C:\\Project1\\Photos\\06-CoralKingdom-BG.png");
Texture coralcylinder("C:\\Project1\\Photos\\gorgonian-soft-coral.png");
Texture starfish("C:\\Project1\\Photos\\star.png");
Texture frame("C:\\Project1\\Photos\\PNGPIX-COM-Photo-Frame-PNG-Transparent-Image-2.png");
Texture camera("C:\\Project1\\Photos\\camera.png");
Texture photoIcon("C:\\Project1\\Photos\\photos.png");
Texture oceanfloor("C:\\Project1\\Photos\\b3_grounds040.jpg");
Texture sky("C:\\Project1\\Photos\\Wiki-background.jpg");
Texture blowfish("C:\\Project1\\Photos\\blowfish.jpg");
Texture minioceanpad("C:\\Project1\\Photos\\mini-sea-blue-ocean-turquoise.jpg");
Texture logo("C:\\Project1\\Photos\\Logo.jpg");
Texture ending("C:\\Project1\\Photos\\end.jpg");

enum MOVES {SINUS, ZIGZAG, STRAIGHT};

class Creature
{
protected:
	Point center;
	Point centeri;
	float initialAngle;
	float angle;
	float alpha;
	unsigned int moveType;
public:
	Creature(){}
	Creature(const Point & p) : center(p) { centeri = center; moveType = rand() % 3; }
	virtual void setFace();
	void setMove(unsigned int i);
	Point getCenter();
	virtual void draw() = 0;
	virtual void move() = 0;
};

void Creature::setMove(unsigned int i)
{
	moveType = i;
}

Point Creature::getCenter()
{
	return center;
}

void Creature::setFace()
{
	initialAngle = - initialAngle;
	alpha += 180;
}

class Whale : public Creature
{
    static Texture * texture;
	float radius;
public:
	Whale(const Point & p, float radius) : Creature(p), radius(radius)
	{
		initialAngle = (rand() % 4 + 1) / 50.0;
		int face = rand() % 2;
		if (face == 0)
		{
			initialAngle *= -1;
			alpha = 60;
		}
		else
		{
			alpha = -120;
		}
		angle = initialAngle;
	}
	Whale(const Whale &);
    static Texture * getTexture();
	
	void move();
	virtual void draw();

};

Texture * Whale::texture = new Texture("C:\\Project1\\Photos\\balena.jpg");

Whale::Whale(const Whale & creature)
{
	center = creature.center;
	centeri = creature.centeri;
	initialAngle = creature.initialAngle;
	angle = creature.angle;
	alpha = creature.alpha;
	moveType = creature.moveType;
	radius = creature.radius;
}

void Whale::move()
{
	switch (moveType)
	{
	case SINUS:
		angle += initialAngle;
		center = Point(center.getx() + initialAngle, centeri.gety() + sin(angle), center.getz());
		break;
	case ZIGZAG:
		angle += initialAngle;
		center = Point(center.getx() + initialAngle, center.gety(), centeri.getz() + sin(angle));
		break;
	case STRAIGHT:
		center = Point(center.getx() + initialAngle, centeri.gety(), center.getz());
		break;

	}
}

void Whale::draw()
{
	glEnable(GL_DEPTH_TEST);
	glColor4f(0.1, 0.1, 0.2, 0.7);
	glPushMatrix();
	glMultMatrixf((GLfloat *)floorshadow);
	glPushMatrix();
	glTranslatef(center.getx(), center.gety(), center.getz());
	glutSolidSphere(radius, 100, 100);
	glPopMatrix();
	glPopMatrix();

	GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat position[] = { Observer ::getInstance()->getEye().getx(), center.gety(), Observer::getInstance()->getEye().getz(), 1.0 };
	GLfloat mat_diffuse[] = { 0.3, 0.2, 0.4, 1.0 };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	GLfloat sphere_mat[] = { 0.2f, 0.5f, 0.9f, 1.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sphere_mat);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture -> getId());
	glPushMatrix();
	glTranslatef(center.getx(), center.gety(), center.getz());
	glRotatef(alpha, 0, 1, 0);
	GLUquadric * quad;
	quad = gluNewQuadric();
	gluQuadricTexture(quad, 300);
	gluSphere(quad, radius, 100, 100);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
}

Texture * Whale::getTexture()
{
	return texture;
}

class Blowfish : public Creature
{
	static Texture * texture;
	float radius;
public:
	Blowfish(const Point & p, float radius) : Creature(p), radius(radius)
	{
		initialAngle = (rand() % 4 + 1) / 50.0;
		int face = rand() % 2;
		if (face == 0)
		{
			initialAngle *= -1;
			alpha = 90;
		}
		else
		{
			alpha = -130;
		}
		angle = initialAngle;
	}
	Blowfish(const Blowfish &);
	static Texture * getTexture();

	void move();
	virtual void draw();

};

Texture * Blowfish::texture = new Texture("C:\\Project1\\Photos\\blowfish.jpg");

Blowfish::Blowfish(const Blowfish & creature)
{
	center = creature.center;
	centeri = creature.centeri;
	initialAngle = creature.initialAngle;
	angle = creature.angle;
	alpha = creature.alpha;
	moveType = creature.moveType;
	radius = creature.radius;
}

void Blowfish::move()
{
	switch (moveType)
	{
	case SINUS:
		angle += initialAngle;
		center = Point(center.getx() + initialAngle, centeri.gety() + sin(angle), center.getz());
		break;
	case ZIGZAG:
		angle += initialAngle;
		center = Point(center.getx() + initialAngle, center.gety(), centeri.getz() + sin(angle));
		break;
	case STRAIGHT:
		center = Point(center.getx() + initialAngle, centeri.gety(), center.getz());
		break;

	}
}

void Blowfish::draw()
{
	glColor4f(0.1, 0.1, 0.2, 0.7);
	glPushMatrix();
	glMultMatrixf((GLfloat *)floorshadow);
	glPushMatrix();
	glTranslatef(center.getx(), center.gety(), center.getz());
	glutSolidSphere(radius, 100, 100);
	glPopMatrix();
	glPopMatrix();

	GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat position[] = { Observer::getInstance()->getEye().getx(), center.gety(), Observer::getInstance()->getEye().getz(), 1.0 };
	GLfloat mat_diffuse[] = { 0.3, 0.2, 0.4, 1.0 };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	GLfloat sphere_mat[] = { 0.5f, 0.4f, 0.3f, 1.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sphere_mat);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->getId());
	glPushMatrix();
	glTranslatef(center.getx(), center.gety(), center.getz());
	glRotatef(alpha, 0, 1, 0);
	GLUquadric * quad;
	quad = gluNewQuadric();
	gluQuadricTexture(quad, 300);
	gluSphere(quad, radius, 100, 100);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
}

Texture * Blowfish::getTexture()
{
	return texture;
}

class Dasyatis : public Creature
{
	static Texture * texture;
	float radius;
public:
	Dasyatis(const Point & p, float radius) : Creature(p), radius(radius)
	{

		initialAngle = (rand() % 4 + 1) / 50.0;
		alpha = rand() % 2;
		if (alpha == 0)
		{
			initialAngle *= -1;
			
		}
		angle = initialAngle;
	}
	Dasyatis(const Dasyatis &);
	static Texture * getTexture();
	void move();
	void draw();
	void setFace()
	{
		if (alpha == 0)
			alpha = 1;
		else alpha = 0;
		initialAngle = -initialAngle;
	}

};

Texture * Dasyatis::texture = new Texture("C:\\Project1\\Photos\\Yellow_Chaetodon_Butterfly_Fish_PNG_Clipart-458.png");

Dasyatis::Dasyatis(const Dasyatis & creature)
{
	center = creature.center;
	centeri = creature.centeri;
	initialAngle = creature.initialAngle;
	angle = creature.angle;
	alpha = creature.alpha;
	moveType = creature.moveType;
	radius = creature.radius;
}

void Dasyatis::move()
{
	switch (moveType)
	{
	case SINUS:
		angle += initialAngle;
		center = Point(center.getx() + initialAngle, centeri.gety() + sin(angle), center.getz());
		break;
	case ZIGZAG:
		angle += initialAngle;
		center = Point(center.getx() + initialAngle, center.gety(), centeri.getz() + sin(angle));
		break;
	case STRAIGHT:
		center = Point(center.getx() + initialAngle, centeri.gety(), center.getz());
		break;

	}
}

void Dasyatis::draw()
{
	glColor3f(0.6, 0.6, 0.6);
	glPushMatrix();
	glMultMatrixf((GLfloat *)floorshadow);
	glPushMatrix();
	glTranslatef(center.getx(), center.gety(), center.getz());
	glutSolidSphere(radius, 100, 100);
    glPopMatrix();
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->getId());
	glPushMatrix();
	glTranslatef(center.getx(), center.gety(), center.getz());
	if (alpha)
	{
		glBegin(GL_QUADS);
        glTexCoord2f(1.0, 1.0);	glVertex3f(radius + 2, radius - 1.5, 0);
		glTexCoord2f(1.0, 0.0);	glVertex3f(radius + 2, radius + 1.5, 0);
		glTexCoord2f(0.0, 0.0); glVertex3f(radius - 2, radius + 1.5, 0);
		glTexCoord2f(0.0, 1.0); glVertex3f(radius - 2, radius - 1.5, 0);
		glEnd();
	}
	else
	{
		glBegin(GL_QUADS);
		glTexCoord2f(1.0, 1.0);	glVertex3f(radius - 2, radius - 1.5, 0);
		glTexCoord2f(1.0, 0.0);	glVertex3f(radius - 2, radius + 1.5, 0);
		glTexCoord2f(0.0, 0.0); glVertex3f(radius + 2, radius + 1.5, 0);
		glTexCoord2f(0.0, 1.0); glVertex3f(radius + 2, radius - 1.5, 0);
		glEnd();
	}
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

Texture * Dasyatis::getTexture()
{
	return texture;
}

vector<Creature *> creatures;

class Bubble
{
	float radius;
	Point center;
	Point centeri;
	float angle;
	float alpha;
public:
	Bubble(const Point & p, float alpha) : center(p), angle(alpha), alpha(alpha) { radius = (rand() % 5 + 1) / 50.0; centeri = p; }
	Bubble(const Bubble &);
	Point getCenter();
	void draw();
	void move();
};

Bubble::Bubble(const Bubble & b)
{
	radius = b.radius;
	center = b.center;
	centeri = b.centeri;
	angle = b.angle;
	alpha = b.alpha;
}

void Bubble::draw()
{
	GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat position[] = { center.getx() - radius / 2, center.gety() - radius / 2, center.getz() + 1, 1.0 };
	GLfloat mat_diffuse[] = { 0.34, 0.0, 0.8, 0.5 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.5 };
	GLfloat mat_shininess[] = { 30.0 };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glColor4f(0.9, 0.9, 0.9, 0.7);
	glPushMatrix();
	glTranslatef(center.getx(), center.gety(), center.getz());
	GLUquadric * quad;
	quad = gluNewQuadric();
	gluSphere(quad, radius, 100, 100);
	glPopMatrix();

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	GLfloat init_diffuse[] = {0.8, 0.8, 0.8, 0.1};
	GLfloat init_specular[] = { 0.0, 0.0, 0.0, 0.1 };
	GLfloat init_shininess[] = {0};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, init_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, init_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, init_shininess);
}

void Bubble::move()
{
	angle += alpha;
	center = Point(centeri.getx() + sin(angle * 300), center.gety() + angle, center.getz());
}

Point Bubble::getCenter()
{
	return center;
}

vector<Bubble> bubbles;

class Coral
{
protected:
	Point center;
	float radius;
public:
	Coral(){}
	Coral(const Point & p, float radius) : center(p), radius(radius){};
	void draw();
	virtual void shape() = 0;
};

void Coral::draw()
{
	glColor4f(0.3, 0.4, 0.8, 0.85);
	Point ob = Observer::getInstance()->getEye();
	
	glPushMatrix();
	glTranslatef(center.getx(), center.gety(), center.getz());
	shape();
	glPopMatrix();
	GLfloat init_mat[] = { 0.8, 0.8, 0.8, 1.0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, init_mat);
}

class CoralSphere : public Coral
{
public:
	CoralSphere(const Point & p, float radius) : Coral(p, radius) {};
	CoralSphere(const CoralSphere & cs) : Coral(cs.center, cs.radius) {};
	void shape();
};


void CoralSphere::shape()
{
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, coralsphere.getId());
	GLUquadric * quad;
	quad = gluNewQuadric();
	gluQuadricTexture(quad, 300);
	gluSphere(quad, radius, 100, 100);

	glDisable(GL_TEXTURE_2D);
}

class CoralCylinder : public Coral
{
public:
	CoralCylinder(const Point & p, float radius) : Coral(p, radius) {};
	CoralCylinder(const CoralCylinder & cs) : Coral(cs.center, cs.radius) {};
	void shape();
};

void CoralCylinder::shape()
{
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, coralcylinder.getId());
	GLUquadric * quad;
	quad = gluNewQuadric();
	gluQuadricTexture(quad, 300);
	glRotatef(90, 1, 0, 0);
	gluCylinder(quad, radius / 4, radius / 4, 2 * radius, 100, 100);

	glDisable(GL_TEXTURE_2D);
}

class Starfish : public Coral
{
public:
	Starfish(const Point & p, float radius) : Coral(p, radius) {};
	Starfish(const Starfish & cs) : Coral(cs.center, cs.radius) {};
	void shape();

};

void Starfish::shape()
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, starfish.getId());
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0); glVertex3f(radius - 1, 0.0001, radius - 1);
	glTexCoord2f(1, 1); glVertex3f(radius - 1, 0.0001, radius + 1);
	glTexCoord2f(0, 1); glVertex3f(radius + 1, 0.0001, radius + 1);
	glTexCoord2f(0, 0); glVertex3f(radius + 1, 0.0001, radius - 1);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
}

vector<Coral *> corals;


GLfloat ctrlpoints [][4][3] =
{
{ { -30, 0.01, -30 },
{ -30, 0.01, -10 },
{ -30, 0.01, 10 },
{ -30, 0.01, 30 } },
{ { -10, 0.01, -30 },
{ -10, 50, -10 },
{ -10, 50, 10 },
{ -10, 0.01, 30 } },
{ { 10, 0.01, -10 },
{ 10, 7, 10 },
{ 10, 7, 30 } },
{ { 30, 0.01, -30 },
{ 30, 0.01, -10 },
{ 30, 0.01, 10 },
{ 30, 0.01, 30 } }
};


struct Wave
{
	float waveNo, x, z, waveTime;
	Wave()
	{
		waveTime = (rand() % 3 + 2) * 100;
		x = (rand() % 70) - 10;
		z = ((rand() % 14) - 9) * 10;
	}
	
	Wave(float waveNo, float x, float z, float waveTime)
	{
		this->waveNo = waveNo;
		this->x = x;
		this->z = z;
		this->waveTime = waveTime;
	}

	GLfloat wave[21][4][3] =
	{
	{ { -100, 50, 30 },
	{ -50, 50, 30 },
	{ 50, 50, 30 },
	{ 100, 50, 30 },
	},
	{ { -100, 50, 30.5 },
	{ -50, 50, 30.5 },
	{ 50, 50, 30.5 },
	{ 100, 50, 30.5 },
	},
	{ { -100, 50, 31 },
	{ -50, 50, 31 },
	{ 50, 50, 31 },
	{ 100, 50, 31 },
	},
	{ { -100, 50, 31.5 },
	{ -50, 50, 31.5 },
	{ 50, 50, 31.5 },
	{ 100, 50, 31.5 },
	},
	{ { -100, 50, 32 },
	{ -50, 50, 32 },
	{ 50, 50, 32 },
	{ 100, 50, 32 },
	},
	{ { -100, 50, 32.5 },
	{ -50, 50, 32.5 },
	{ 50, 50, 32.5 },
	{ 100, 50, 32.5 },
	},
	{ { -100, 50, 33 },
	{ -50, 50, 33 },
	{ 50, 50, 33 },
	{ 100, 50, 33 },
	},
	{ { -100, 50, 33.5 },
	{ -50, 50, 33.5 },
	{ 50, 50, 33.5 },
	{ 100, 50, 33.5 },
	},
	{ { -100, 50, 34 },
	{ -50, 50, 34 },
	{ 50, 50, 34 },
	{ 100, 50, 34 },
	},
	{ { -100, 50, 34.5 },
	{ -50, 50, 34.5 },
	{ 50, 50, 34.5 },
	{ 100, 50, 34.5 },
	},
	{ { -100, 50, 35 },
	{ -50, 50, 35 },
	{ 50, 50, 35 },
	{ 100, 50, 35 },
	},
	{ { -100, 50, 35.5 },
	{ -50, 50, 35.5 },
	{ 50, 50, 35.5 },
	{ 100, 50, 35.5 },
	},
	{ { -100, 50, 36 },
	{ -50, 50, 36 },
	{ 50, 50, 36 },
	{ 100, 50, 36 },
	},
	{ { -100, 50, 36.5 },
	{ -50, 50, 36.5 },
	{ 50, 50, 36.5 },
	{ 100, 50, 36.5 },
	},
	{ { -100, 50, 37 },
	{ -50, 50, 37 },
	{ 50, 50, 37 },
	{ 100, 50, 37 },
	},
	{ { -100, 50, 37.5 },
	{ -50, 50, 37.5 },
	{ 50, 50, 37.5 },
	{ 100, 50, 37.5 },
	},
	{ { -100, 50, 38 },
	{ -50, 50, 38 },
	{ 50, 50, 38 },
	{ 100, 50, 38 },
	},
	{ { -100, 50, 38.5 },
	{ -50, 50, 38.5 },
	{ 50, 50, 38.5 },
	{ 100, 50, 38.5 },
	},
	{ { -100, 50, 39 },
	{ -50, 50, 39 },
	{ 50, 50, 39 },
	{ 100, 50, 39 },
	},
	{ { -100, 50, 39.5 },
	{ -50, 50, 39.5 },
	{ 50, 50, 39.5 },
	{ 100, 50, 39.5 },
	}
	};

}wvs[7];


void init()
{
	glClearColor(0.2, 0.2, 0.4, 1.0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	binocular.loadTexture();
	Whale :: getTexture() -> loadTexture();
	Blowfish::getTexture()->loadTexture();
	Dasyatis::getTexture()->loadTexture();
	ocean.loadTexture();
	oceanpad.loadTexture();
	oceanfloor.loadTexture();
	coralsphere.loadTexture();
	coralcylinder.loadTexture();
	starfish.loadTexture();
	frame.loadTexture();
	camera.loadTexture();
	photoIcon.loadTexture();
	sky.loadTexture();
	minioceanpad.loadTexture();
	logo.loadTexture();
	ending.loadTexture();

	corals.push_back(new CoralCylinder(Point(-50, 3, 15), 2));
	corals.push_back(new CoralSphere(Point(55, 1.5, 25), 4));
	corals.push_back(new CoralSphere(Point(-30, 1.5, 35), 2));
	corals.push_back(new CoralCylinder(Point(44, 3, 40), 2));
	corals.push_back(new CoralCylinder(Point(-10, 3, 42), 2));
	corals.push_back(new CoralCylinder(Point(23, 3, 44), 2));
	corals.push_back(new CoralSphere(Point(15, 1.5, 45), 2));
	corals.push_back(new CoralSphere(Point(-3, 1.5, 54), 3));
	corals.push_back(new CoralCylinder(Point(10, 3, 66), 2));
	corals.push_back(new CoralCylinder(Point(-8, 3, 72), 2));
	corals.push_back(new CoralCylinder(Point(-2, 3, 75), 2));
	corals.push_back(new CoralCylinder(Point(-55, 3, -15), 2));
	corals.push_back(new CoralSphere(Point(52, 3, -20), 2));
	corals.push_back(new Starfish(Point(30, 0, 46), 2));
	corals.push_back(new Starfish(Point(-21, 0, 55), 2));
	corals.push_back(new Starfish(Point(3, 0, 67), 2));
	corals.push_back(new Starfish(Point(13, 0, 59), 2));

	GLfloat plane[4];
	GLfloat v0[3], v1[3], v2[3];

	v0[X] = -100.f;
	v0[Y] =  0.001f;
	v0[Z] = -100.f;
	v1[X] = 100.f;
	v1[Y] =  0.001f;
	v1[Z] = 100.f;
	v2[X] = -100.f;
	v2[Y] = 0.001f;
	v2[Z] = 100.f;

	findplane(plane, v0, v1, v2);
	shadowmatrix(floorshadow, plane, lightpos);

	glEnable(GL_FOG);
	GLfloat fogColor[4] = { 0.2, 0.2, 0.4, 1.0 };
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.7);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 50.0);
	glFogf(GL_FOG_END, 120.0);
}


void changeSize(int w, int h)
{

	if (h == 0)
		h = 1;
	float ratio = w * 1.0 / h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45.0f, ratio, 0.1f, 200.0f);
	glMatrixMode(GL_MODELVIEW);
}


void drawMountain(int x, int y, GLfloat ctrlpoints [][4][3])
{
	GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat position[] = { x, 3.0, y, 1.0 };
	GLfloat mat_diffuse[] = { 0.1, 0.1, 0.1, 1.0 };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT3);

	glLightfv(GL_LIGHT3, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT3, GL_POSITION, position);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
	glColor3f(0.4f, 0.5f, 0.8f);
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4,
		0, 1, 12, 4, & ctrlpoints[0][0][0]);
	glEnable(GL_MAP2_VERTEX_3);
	glMapGrid2f(10, 0.0, 1.0, 10, 0.0, 1.0);
	glShadeModel(GL_FLAT);
	glEvalMesh2(GL_FILL, 0, 10, 0, 10);
	glDisable(GL_MAP2_VERTEX_3);

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT3);
}

float o2 = 0.00001;

void drawO2Level()
{
	Point ob = Observer::getInstance()->getEye();
	glColor3f(0.8, 0.8, 0.8);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glVertex3f(ob.getx() - 1.55, ob.gety() - 0.65, ob.getz() - 2);
	glVertex3f(ob.getx() - 1.55, ob.gety() - 0.55, ob.getz() - 2);
	glVertex3f(ob.getx() - 0.9 - o2, ob.gety() - 0.55, ob.getz() - 2);
	glVertex3f(ob.getx() - 0.9 - o2, ob.gety() - 0.65, ob.getz() - 2);
	glEnd();

	glLineWidth(5.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
	glVertex3f(ob.getx() - 1.55, ob.gety() - 0.65, ob.getz() - 2);
	glVertex3f(ob.getx() - 1.55, ob.gety() - 0.55, ob.getz() - 2);
	glVertex3f(ob.getx() - 0.9, ob.gety() - 0.55, ob.getz() - 2);
	glVertex3f(ob.getx() - 0.9, ob.gety() - 0.65, ob.getz() - 2);
	glEnd();
	glLineWidth(1);
	
	print("Nivelul de oxigen", ob.getx() - 1.55, ob.gety() - 0.47, ob.getz() - 2);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

GLfloat texpts[2][2][2] = 
{ { { 0.0, 0.0 },{ 0.0, 5.0 } },
{ { 5.0, 0.0 },{ 5.0, 5.0 } } };

void drawWave(int x, int y, int k, GLfloat ctrlpoints[20][4][3])
{
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	glBindTexture(GL_TEXTURE_2D, minioceanpad.getId());
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 5,
		0, 1, 12, 5, & ctrlpoints[0][0][0]);
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2,
		0, 1, 4, 2, & texpts[0][0][0]);
	glEnable(GL_MAP2_TEXTURE_COORD_2);
	glEnable(GL_MAP2_VERTEX_3);

	for (int i = 0; i < 4; i++)
	{
		ctrlpoints[k][i][1] = 50;
		if (k == 19)
			ctrlpoints[0][i][1] = 53;
		else ctrlpoints[k + 1][i][1] = 53;
	}

	glEnable(GL_MAP2_VERTEX_3);
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
	glShadeModel(GL_SMOOTH);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glDisable(GL_MAP2_VERTEX_3);
	glDisable(GL_TEXTURE_2D);

}

class Photo
{
public:
	Point observer;
	Point observerd;
	vector<Creature *> creaturesInPhoto;
	vector<Bubble> bubblesInPhoto;
	vector<Coral *> coralsInPhoto;
	vector<Wave> wavesInPhoto;
	Photo(Point observer, Point observerd, vector<Creature *> creatures, vector<Bubble> bubbles, vector<Coral *> corals, vector<Wave> wavesInPhoto) :
		observerd(observerd), observer(observer), creaturesInPhoto(creatures), bubblesInPhoto(bubbles), coralsInPhoto(corals), wavesInPhoto(wavesInPhoto)
	    {}
	void draw();
};

void numberToString(unsigned int x, string & s)
{
	if (x == 0)
		s.push_back(49);

	while (x)
	{
		s.push_back((x % 10) + 49);
		x /= 10;
	}

	for (int i = 0; i < s.size() / 2; i++)
	{
		unsigned char c;
		c = s.at(i);
		s.at(i) = s.at(s.size() - 1 - i);
		s.at(s.size() - 1 - i) = c;
	}
}

vector<Photo> photos;


enum PAGE { START, SCENE, PHOTO, END };
int pageno;
int currentPhoto;
float blitz, alphaBlitz, forbidden, alphaForbidden;

void Photo::draw()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glLoadIdentity();

	gluLookAt(observer.getx(), observer.gety(), observer.getz(),
		observer.getx() + observerd.getx(), observer.gety() + observerd.gety(), observer.getz() + observerd.getz(),
		0.0f, 1.0f, 0.0f);

	glColor3f(0.6f, 0.7f, 0.9f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, oceanfloor.getId());
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-110.0f, 0.0f, -50.0f);
	glTexCoord2f(0, 21); glVertex3f(-110.0f, 0.0f, 110.0f);
	glTexCoord2f(21, 21); glVertex3f(110.0f, 0.0f, 110.0f);
	glTexCoord2f(10, 0); glVertex3f(110.0f, 0.0f, -50.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glColor4f(0.6f, 0.7f, 0.9f, 0.5f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, oceanpad.getId());
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-110.0f, 50.0f, -51.0f);
	glTexCoord2f(0, 1); glVertex3f(-110.0f, 50.0f, 110.0f);
	glTexCoord2f(1, 1); glVertex3f(110.0f, 50.0f, 110.0f);
	glTexCoord2f(1, 0); glVertex3f(110.0f, 50.0f, -51.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);


	glColor3f(1.0, 1.0, 1.0);
	glDisable(GL_FOG);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sky.getId());
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(-110.0f, 50.0f, -51.0f);
	glTexCoord2f(1, 0); glVertex3f(-110.0f, 140.0f, -51.0f);
	glTexCoord2f(0, 0); glVertex3f(110.0f, 140.0f, -51.0f);
	glTexCoord2f(0, 1); glVertex3f(110.0f, 50.0f, -51.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_FOG);


	drawMountain(observer.getx(), observer.getz(), ctrlpoints);

	for (int i = 0; i < wavesInPhoto.size(); i++)
		drawWave(observer.getx(), observer.gety(), wavesInPhoto.at(i).waveNo, wavesInPhoto.at(i).wave);

	for (int i = 0; i < coralsInPhoto.size(); i++)
	{
		coralsInPhoto.at(i)->draw();
	}

	for (int i = 0; i < creaturesInPhoto.size(); i++)
	{
		creaturesInPhoto.at(i)->draw();
	}
	for (int i = 0; i < bubblesInPhoto.size(); i++)
	{
		bubblesInPhoto.at(i).draw();
	}

	glDisable(GL_DEPTH_TEST);

	glColor4f(0.44f, 0.10f, 1.0f, 0.2f);
	glBegin(GL_QUADS);
	glVertex3f(observer.getx() - 2, observer.gety() - 1, observer.getz() - 2);
	glVertex3f(observer.getx() - 2, observer.gety() + 1, observer.getz() - 2);
	glVertex3f(observer.getx() + 2, observer.gety() + 1, observer.getz() - 2);
	glVertex3f(observer.getx() + 2, observer.gety() - 1, observer.getz() - 2);
	glEnd();

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(observer.getx() - 1.8, observer.gety() - 1, observer.getz() - 2);
	glVertex3f(observer.getx() - 1.8, observer.gety() + 1, observer.getz() - 2);
	glVertex3f(observer.getx() - 200, observer.gety() + 1, observer.getz() - 2);
	glVertex3f(observer.getx() - 200, observer.gety() - 1, observer.getz() - 2);
	glEnd();

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(observer.getx() + 1.8, observer.gety() - 1, observer.getz() - 2);
	glVertex3f(observer.getx() + 1.8, observer.gety() + 1, observer.getz() - 2);
	glVertex3f(observer.getx() + 200, observer.gety() + 1, observer.getz() - 2);
	glVertex3f(observer.getx() + 200, observer.gety() - 1, observer.getz() - 2);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glColor3f(0.9f, 0.9f, 0.9f);
	glBindTexture(GL_TEXTURE_2D, frame.getId());
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0); glVertex3f(observer.getx() - 1.8, observer.gety() - 0.9, observer.getz() - 2);
	glTexCoord2f(1, 1); glVertex3f(observer.getx() - 1.8, observer.gety() + 0.9, observer.getz() - 2);
	glTexCoord2f(0, 1); glVertex3f(observer.getx() + 1.8, observer.gety() + 0.9, observer.getz() - 2);
	glTexCoord2f(0, 0); glVertex3f(observer.getx() + 1.8, observer.gety() - 0.9, observer.getz() - 2);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(observer.getx(), observer.gety() - 1, observer.getz() - 2);
	GLUquadric * quad;
	quad = gluNewQuadric();
	gluQuadricTexture(quad, 300);
	gluDisk(quad, 0, 0.5, 100, 100);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	glColor3f(0.7f, 0.7f, 0.1f);
	print("Pozele mele", observer.getx() - 0.18, observer.gety() - 0.65, observer.getz() - 2, 2, 1);
	string s1, s2, s3;
	s3 = " / ";	
	numberToString(currentPhoto, s1);
	numberToString(photos.size() - 1, s2);
	print(s1.append(s3).append(s2), observer.getx() - 0.05, observer.gety() - 0.75, observer.getz() - 2, 2, 1);
}


void renderScene(void)
{
	if (pageno == SCENE)
	{
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glLoadIdentity();

		float obx = Observer::getInstance()->getEye().getx();
		float oby = Observer::getInstance()->getEye().gety();
		float obz = Observer::getInstance()->getEye().getz();
		float obdx = Observer::getInstance()->getdx();
		float obdy = Observer::getInstance()->getdy();
		float obdz = Observer::getInstance()->getdz();
		gluLookAt(obx, oby, obz, obx + obdx, oby + obdy, obz + obdz, 0.0f, 1.0f, 0.0f);

		glColor3f(0.6f, 0.7f, 0.9f);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, oceanfloor.getId());
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-150.0f, 0.0f, -50.0f);
		glTexCoord2f(0, 21); glVertex3f(-150.0f, 0.0f, 110.0f);
		glTexCoord2f(21, 21); glVertex3f(150.0f, 0.0f, 110.0f);
		glTexCoord2f(10, 0); glVertex3f(150.0f, 0.0f, -50.0f);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glColor4f(0.6f, 0.7f, 0.9f, 0.5f);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, oceanpad.getId());
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-110.0f, 50.0f, -50.0f);
		glTexCoord2f(0, 1); glVertex3f(-110.0f, 50.0f, 110.0f);
		glTexCoord2f(1, 1); glVertex3f(110.0f, 50.0f, 110.0f);
		glTexCoord2f(1, 0); glVertex3f(110.0f, 50.0f, -50.0f);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glColor4f(0.6f, 0.7f, 0.9f, 0.5f);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, oceanpad.getId());
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-110.0f, 49.89f, -51.0f);
		glTexCoord2f(0, 1); glVertex3f(-110.0f, 49.89f, 110.0f);
		glTexCoord2f(1, 1); glVertex3f(110.0f, 49.89f, 110.0f);
		glTexCoord2f(1, 0); glVertex3f(110.0f, 49.89f, -51.0f);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glColor3f(1.0, 1.0, 1.0);
		glDisable(GL_FOG);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, sky.getId());
		glBegin(GL_QUADS);
		glTexCoord2f(1, 1); glVertex3f(-110.0f, 50.0f, -51.0f);
		glTexCoord2f(1, 0); glVertex3f(-110.0f, 140.0f, -51.0f);
		glTexCoord2f(0, 0); glVertex3f(110.0f, 140.0f, -51.0f);
		glTexCoord2f(0, 1); glVertex3f(110.0f, 50.0f, -51.0f);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_FOG);

		drawMountain(obx, obz, ctrlpoints);

		for (int i = 0; i < 6; i++)
		{
			glPushMatrix();
			glTranslatef(wvs[i].x, -0.001, wvs[i].z);
			if (wvs[i].wave[0][0][0] + wvs[i].z >= 0)
			{
				wvs[i].waveTime = (rand() % 3 + 2) * 100;
				wvs[i].x = (rand() % 70) - 10;
				wvs[i].z = ((rand() % 14) - 9) * 10;
			}
			else wvs[i].z += 0.01;
			drawWave(obx, obz, wvs[i].waveNo, wvs[i].wave);
			glPopMatrix();
		}

		
		for (int i = 0; i < creatures.size(); i++)
		{
			if (creatures.at(i)->getCenter().getx() > 50 || creatures.at(i)->getCenter().getx() < -50)
			{
				creatures.erase(creatures.begin() + i);
				i--;
			}
			else
			{
				creatures.at(i)->draw();
				creatures.at(i)->move();
			}
		}

		for (int i = 0; i < corals.size(); i++)
		{
			corals.at(i)->draw();
		}

		for (int i = 0; i < bubbles.size(); i++)
		{
			if (bubbles.at(i).getCenter().gety() > oby + 4 || bubbles.at(i).getCenter().gety() > 49)
			{
				bubbles.erase(bubbles.begin() + i);
				i--;
			}
			else
			{
				bubbles.at(i).draw();
				bubbles.at(i).move();
			}
		}

		glColor3f(1.0, 1.0, 1.0);
		GLfloat ocean_mat[] = { 0.2f, 0.5f, 0.9f, 1.f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ocean_mat);

		glDisable(GL_DEPTH_TEST);

		glColor4f(0.44f, 0.10f, 1.0f, 0.2f);
		glBegin(GL_QUADS);
		glVertex3f(obx - 2, oby - 1, obz - 2);
		glVertex3f(obx - 2, oby + 1, obz - 2);
		glVertex3f(obx + 2, oby + 1, obz - 2);
		glVertex3f(obx + 2, oby - 1, obz - 2);
		glEnd();

		glColor4f(1.0f, 1.0f, 1.0f, blitz);
		if (blitz != 0.0)
		{
			if (blitz >= 1.0)
			{
				alphaBlitz = -0.02;
			}

			blitz += alphaBlitz;
		}

		glBegin(GL_QUADS);
		glVertex3f(obx - 2, oby - 1, obz - 1.9);
		glVertex3f(obx - 2, oby + 1, obz - 1.9);
		glVertex3f(obx + 2, oby + 1, obz - 1.9);
		glVertex3f(obx + 2, oby - 1, obz - 1.9);
		glEnd();

		glColor4f(1.0f, 0.3f, 0.0f, forbidden);
		if (forbidden != 0.0)
		{
			if (forbidden >= 1.0)
			{
				alphaForbidden = -0.1;
			}
			forbidden += alphaForbidden;
		}

		glBegin(GL_QUADS);
		glVertex3f(obx - 2, oby - 1, obz - 1.8);
		glVertex3f(obx - 2, oby + 1, obz - 1.8);
		glVertex3f(obx + 2, oby + 1, obz - 1.8);
		glVertex3f(obx + 2, oby - 1, obz - 1.8);
		glEnd();

		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glVertex3f(obx - 2, oby - 1, obz - 2);
		glVertex3f(obx - 2, oby + 1, obz - 2);
		glVertex3f(obx - 200, oby + 1, obz - 2);
		glVertex3f(obx - 200, oby - 1, obz - 2);
		glEnd();

		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glVertex3f(obx + 2, oby - 1, obz - 2);
		glVertex3f(obx + 2, oby + 1, obz - 2);
		glVertex3f(obx + 200, oby + 1, obz - 2);
		glVertex3f(obx + 200, oby - 1, obz - 2);
		glEnd();

		glEnable(GL_TEXTURE_2D);
		glColor3f(0.9f, 0.9f, 0.9f);
		glBindTexture(GL_TEXTURE_2D, binocular.getId());
		glBegin(GL_QUADS);
		glTexCoord2f(1, 0); glVertex3f(obx - 2, oby - 1, obz - 2);
		glTexCoord2f(1, 1); glVertex3f(obx - 2, oby + 1, obz - 2);
		glTexCoord2f(0, 1); glVertex3f(obx + 2, oby + 1, obz - 2);
		glTexCoord2f(0, 0); glVertex3f(obx + 2, oby - 1, obz - 2);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_TEXTURE_2D);
		glColor3f(0.9f, 0.9f, 0.9f);
		glBindTexture(GL_TEXTURE_2D, camera.getId());
		glBegin(GL_QUADS);
		glTexCoord2f(1, 0); glVertex3f(obx + 1.2, oby - 0.6, obz - 1.8);
		glTexCoord2f(1, 1); glVertex3f(obx + 1.2, oby - 0.4, obz - 1.8);
		glTexCoord2f(0, 1); glVertex3f(obx + 1.4, oby - 0.4, obz - 1.8);
		glTexCoord2f(0, 0); glVertex3f(obx + 1.4, oby - 0.6, obz - 1.8);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_TEXTURE_2D);
		glColor3f(0.9f, 0.9f, 0.9f);
		glBindTexture(GL_TEXTURE_2D, photoIcon.getId());
		glBegin(GL_QUADS);
		glTexCoord2f(0, 1); glVertex3f(obx + 1.15, oby - 0.2, obz - 1.8);
		glTexCoord2f(0, 0); glVertex3f(obx + 1.15, oby + 0.05, obz - 1.8);
		glTexCoord2f(1, 0); glVertex3f(obx + 1.43, oby + 0.05, obz - 1.8);
		glTexCoord2f(1, 1); glVertex3f(obx + 1.43, oby - 0.2, obz - 1.8);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		drawO2Level();

		if (oby < 50)
		{
			if (o2 <= 0.65)
				o2 += 0.0001;
			else
			{
				pageno = END;
				exit(0);
			}
		}
		else if (o2 >= 0.0)
			o2 -= 0.002;

		if (createPhoto)
		{
			vector<Creature *> auxVect;
			for (int i = 0; i < creatures.size(); i++)
			{
				Whale * wh = dynamic_cast<Whale *>(creatures.at(i));
				if (wh)
					auxVect.push_back(new Whale(*wh));
				else
				{
					Blowfish * bf = dynamic_cast<Blowfish *>(creatures.at(i));
					if (bf)
						auxVect.push_back(new Blowfish(* bf));
					else
					{
						Dasyatis * ds = dynamic_cast<Dasyatis *>(creatures.at(i));
						if (ds)
							auxVect.push_back(new Dasyatis(*ds));
					}
				}
			}
			vector<Bubble> auxBubbles;
			for (int i = 0; i < bubbles.size(); i++)
			{
				auxBubbles.push_back(*new Bubble(bubbles.at(i)));
			}
			vector<Coral *> auxCorals;
			for (int i = 0; i < corals.size(); i++)
			{
				CoralSphere * cs = dynamic_cast<CoralSphere *>(corals.at(i));
				if (cs)
					auxCorals.push_back(new CoralSphere(*cs));
				else
				{
					CoralCylinder * cc = dynamic_cast<CoralCylinder *>(corals.at(i));
					if (cc)
						auxCorals.push_back(new CoralCylinder(*cc));
					else
					{
						Starfish * sf = dynamic_cast<Starfish *>(corals.at(i));
						if (sf)
							auxCorals.push_back(new Starfish(*sf));
					}
				}
			}
			vector<Wave> auxwaves;
			for (int i = 0; i < 7; i++)
				auxwaves.push_back(Wave(wvs[i].waveNo, wvs[i].x, wvs[i].z, wvs[i].waveTime));
			photos.push_back(Photo(Point(obx, oby, obz), Point(obdx, obdy, obdz), auxVect, auxBubbles, auxCorals, auxwaves));
			createPhoto = false;
		}
		glutSwapBuffers();
	}
	else if(pageno == PHOTO)
	{
		if (photos.size() > 0)
		{
			photos.at(currentPhoto).draw();
		}
		else pageno = SCENE;
		glutSwapBuffers();
	}
	else if (pageno == START)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glLoadIdentity();

		float obx = Observer::getInstance()->getEye().getx();
		float oby = Observer::getInstance()->getEye().gety();
		float obz = Observer::getInstance()->getEye().getz();
		float obdx = Observer::getInstance()->getdx();
		float obdy = Observer::getInstance()->getdy();
		float obdz = Observer::getInstance()->getdz();
		gluLookAt(obx, oby, obz, obx + obdx, oby + obdy, obz + obdz, 0.0f, 1.0f, 0.0f);

		glColor3f(0.6f, 0.7f, 0.9f);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, oceanfloor.getId());
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-150.0f, 0.0f, -50.0f);
		glTexCoord2f(0, 21); glVertex3f(-150.0f, 0.0f, 110.0f);
		glTexCoord2f(21, 21); glVertex3f(150.0f, 0.0f, 110.0f);
		glTexCoord2f(10, 0); glVertex3f(150.0f, 0.0f, -50.0f);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glColor4f(0.6f, 0.7f, 0.9f, 0.5f);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, oceanpad.getId());
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-110.0f, 50.0f, -50.0f);
		glTexCoord2f(0, 1); glVertex3f(-110.0f, 50.0f, 110.0f);
		glTexCoord2f(1, 1); glVertex3f(110.0f, 50.0f, 110.0f);
		glTexCoord2f(1, 0); glVertex3f(110.0f, 50.0f, -50.0f);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glColor4f(0.6f, 0.7f, 0.9f, 0.5f);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, oceanpad.getId());
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-110.0f, 49.89f, -51.0f);
		glTexCoord2f(0, 1); glVertex3f(-110.0f, 49.89f, 110.0f);
		glTexCoord2f(1, 1); glVertex3f(110.0f, 49.89f, 110.0f);
		glTexCoord2f(1, 0); glVertex3f(110.0f, 49.89f, -51.0f);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glColor3f(1.0, 1.0, 1.0);
		glDisable(GL_FOG);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, sky.getId());
		glBegin(GL_QUADS);
		glTexCoord2f(1, 1); glVertex3f(-110.0f, 50.0f, -51.0f);
		glTexCoord2f(1, 0); glVertex3f(-110.0f, 140.0f, -51.0f);
		glTexCoord2f(0, 0); glVertex3f(110.0f, 140.0f, -51.0f);
		glTexCoord2f(0, 1); glVertex3f(110.0f, 50.0f, -51.0f);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_FOG);

		drawMountain(obx, obz, ctrlpoints);

		for (int i = 0; i < 6; i++)
		{
			glPushMatrix();
			glTranslatef(wvs[i].x, -0.001, wvs[i].z);
			if (wvs[i].wave[0][0][0] + wvs[i].z >= 0)
			{
				wvs[i].waveTime = (rand() % 3 + 2) * 100;
				wvs[i].x = (rand() % 70) - 10;
				wvs[i].z = ((rand() % 14) - 9) * 10;
			}
			else wvs[i].z += 0.01;
			drawWave(obx, obz, wvs[i].waveNo, wvs[i].wave);
			glPopMatrix();
		}


		glColor3f(1.0, 1.0, 1.0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, logo.getId());
		glBegin(GL_QUADS);
		glTexCoord2f(1, 1); 
		glVertex3f(0.6f, 60.1f, 78.0f);
		glTexCoord2f(1, 0); 
		glVertex3f(0.6f, 60.6f, 78.0f);
		glTexCoord2f(0, 0); 
		glVertex3f(-0.6f, 60.6f, 78.0f);
		glTexCoord2f(0, 1); 
		glVertex3f(-0.6f, 60.1f, 78.0f);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glVertex3f(- 1.68, oby - 1, obz - 2);
		glVertex3f(- 1.68, oby + 1, obz - 2);
		glVertex3f(- 200, oby + 1, obz - 2);
		glVertex3f(- 200, oby - 1, obz - 2);
		glEnd();

		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glVertex3f(1.68, oby - 1, obz - 2);
		glVertex3f(1.68, oby + 1, obz - 2);
		glVertex3f(200, oby + 1, obz - 2);
		glVertex3f(200, oby - 1, obz - 2);
		glEnd();

		print("Apasati tasta - i - pentru a incepe!", obx - 0.52, oby, obz - 2);
		glutSwapBuffers();
	}
	else if (pageno == END)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glLoadIdentity();

		float obx = Observer::getInstance()->getEye().getx();
		float oby = Observer::getInstance()->getEye().gety();
		float obz = Observer::getInstance()->getEye().getz();
		float obdx = Observer::getInstance()->getdx();
		float obdy = Observer::getInstance()->getdy();
		float obdz = Observer::getInstance()->getdz();
		gluLookAt(obx, oby, obz, obx + obdx, oby + obdy, obz + obdz, 0.0f, 1.0f, 0.0f);

		glEnable(GL_TEXTURE_2D);
		glColor3f(0.9f, 0.9f, 0.9f);
		glBindTexture(GL_TEXTURE_2D, ending.getId());
		glBegin(GL_QUADS);
		glTexCoord2f(1, 1); glVertex3f(obx + 2, oby - 1, obz - 2);
		glTexCoord2f(0, 1); glVertex3f(obx + 2, oby + 1, obz - 2);
		glTexCoord2f(0, 0); glVertex3f(obx - 2, oby + 1, obz - 2);
		glTexCoord2f(1, 0); glVertex3f(obx - 2, oby - 1, obz - 2);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glVertex3f(obx - 2, oby - 1, obz - 2);
		glVertex3f(obx - 2, oby + 1, obz - 2);
		glVertex3f(obx - 200, oby + 1, obz - 2);
		glVertex3f(obx - 200, oby - 1, obz - 2);
		glEnd();

		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glVertex3f(obx + 2, oby - 1, obz - 2);
		glVertex3f(obx + 2, oby + 1, obz - 2);
		glVertex3f(obx + 200, oby + 1, obz - 2);
		glVertex3f(obx + 200, oby - 1, obz - 2);
		glEnd();

		glutSwapBuffers();
	}
}

void mouseFunc(int button, int state, int a, int b)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		int myx = Observer::getInstance()->getEye().getx();
		int myy = Observer::getInstance()->getEye().gety();
		int myz = Observer::getInstance()->getEye().getz();
		Point pos((a - 0) * 4 / 1024.0 + (myx - 2), ((512 - b) - 0) * 2 / 512.0 + (myy - 1), myz);
		if (pos.getx() >= (myx + 1.6) && pos.getx() <= (myx + 1.88) && pos.gety() >= (myy - 0.8) && pos.gety() <= (myy - 0.6))
		{
			createPhoto = true;
			blitz = 0.01;
			alphaBlitz = 0.02;
		}
		else if (pos.getx() >= (myx + 1.55) && pos.getx() <= (myx + 1.91) && pos.gety() >= (myy - 0.2) && pos.gety() <= (myy + 0.05))
		{
			if (pageno == SCENE)
				pageno = PHOTO;
		}
		glutPostRedisplay();
	}

}

void key(unsigned char key, int x, int y)
{
	Point ob = Observer::getInstance()->getEye();

	
	if (pageno == START)
	{
		if (key == 'i')
			pageno = SCENE;
	}
	else if(pageno == SCENE)
	switch(key)
	{
	case 'p':
		createPhoto = true;
		blitz = 0.01;
		alphaBlitz = 0.02;
		break;
	case 'u':
		if(ob.gety() < 60)
			Observer::getInstance()->setEye(*new Point(ob.getx(), ob.gety() + 0.1, ob.getz()));
		break;
	case 'd':
		if(ob.gety() > 1.0)
			Observer::getInstance()->setEye(*new Point(ob.getx(), ob.gety() - 0.1, ob.getz()));
		break;
	}
	else if (pageno == PHOTO)
	{
		if (key == 27)
			pageno = SCENE;
	}

	glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy)
{
	Point ob = Observer::getInstance()->getEye();

	if (pageno == PHOTO)
	{
		switch (key)
		{
		case GLUT_KEY_LEFT:
			if (currentPhoto > 0 && currentPhoto < photos.size())
			{
				currentPhoto--;
			}
			break;
		case GLUT_KEY_RIGHT:
			if (currentPhoto >= 0 && currentPhoto < photos.size() - 1)
			{
				currentPhoto++;
			}
			break;
		}
	}
	else
	if(pageno == SCENE)
	switch (key) 
	{
	case GLUT_KEY_LEFT:
		if(ob.getx() > -10.0)
		   Observer::getInstance()->setEye(*new Point(ob.getx() - 0.1, ob.gety(), ob.getz()));
		else
		{
			forbidden = 0.1;
			alphaForbidden = 0.01;
		}
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		if(ob.getx() < 10.0)
		   Observer::getInstance()->setEye(*new Point(ob.getx() + 0.1, ob.gety(), ob.getz()));
		else
		{
			forbidden = 0.1;
			alphaForbidden = 0.01;
		}
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		if(ob.getz() > 65.0)
		    Observer::getInstance()->setEye(*new Point(ob.getx(), ob.gety(), ob.getz() - 0.1));
		else
		{
			forbidden = 0.1;
			alphaForbidden = 0.01;
		}
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		if(ob.getz() < 80.0)
			Observer::getInstance()->setEye(*new Point(ob.getx(), ob.gety(), ob.getz() + 0.1));
		else
		{
			forbidden = 0.1;
			alphaForbidden = 0.01;
		}
		glutPostRedisplay();
		break;
	}
}

void addCreature(int val)
{
	vector<Creature *> vectAux;
	for (int i = 0; i < creatures.size(); i++)
	{
		Whale * wh = dynamic_cast<Whale *>(creatures.at(i));
		if (wh)
		{
			vectAux.push_back(new Whale(* wh));
		}
		else
		{
			Blowfish * bl = dynamic_cast<Blowfish *>(creatures.at(i));
			if (bl)
			{
				vectAux.push_back(new Blowfish(*bl));
			}
			else
			{
				Dasyatis * ds = dynamic_cast<Dasyatis *>(creatures.at(i));
				if (ds)
				{
					vectAux.push_back(new Dasyatis(*ds));
				}
			}
		}
	}

	int i;
	int row = (rand() % 3 + 4) * 10;
	for (i = 0; i < vectAux.size(); i++)
	{
		if (creatures.at(i)->getCenter().getz() >= row * 1.0 - 1 && creatures.at(i)->getCenter().getz() <= row * 1.0 + 1)
		{
			break;
		}
	}

	if (i == vectAux.size())
	{
		Point randPoint((rand() % 2 == 0) ? 50 : - 50, (rand() % 2 + 6), row);
		int type = rand() % 3;
		Creature * c;
		switch (type)
		{
		case 0: c = new Whale(randPoint, rand() % 4 + 1);
			creatures.push_back(c);
			break;
		case 1: c = new Blowfish(randPoint, rand() % 2 + 1);
			creatures.push_back(c);
			break;
		case 2: 
			if (row == 40)
			{
				c = new Dasyatis(randPoint, rand() % 2 + 2);
			}
			else c = new Blowfish(randPoint, rand() % 2 + 1);
			creatures.push_back(c);
			break;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(rand() % 700 + 500, addCreature, 0);
}

void changeTrajectoryTimer(int i)
{
	int randomnr;
	if(creatures.size() > 0)
		randomnr = rand() % creatures.size();
	for (int i = 0; i < creatures.size(); i++)
		if (i == randomnr)
		{
			creatures.at(i)->setMove(rand() % 3);
		}
	glutPostRedisplay();
	glutTimerFunc(rand() % 7000 + 5000, changeTrajectoryTimer, 0);
}

void setDirectionTimer(int i)
{
	int randomnr;
	if (creatures.size() > 0)
		randomnr = rand() % creatures.size();
	for(int i = 0; i < creatures.size(); i ++)
		if (i == randomnr)
		{
			if(creatures.at(i) ->getCenter().getx() < 5.0 && creatures.at(i) ->getCenter().getx() > - 5.0)
			creatures.at(i)->setFace();
		}
	glutPostRedisplay();
	glutTimerFunc(rand() % 9000 + 6000, setDirectionTimer, 0);
}

void createBubblesTimer(int i)
{

	if (bubbles.size() < 8 && Observer::getInstance()->getEye().gety() < 45)
	{
		float xpoz;
		xpoz = ((rand() % 200) - 100) / 100.0;
		if (rand() % 2 == 0)
		{
			xpoz *= -1;
		}
		bubbles.push_back(Bubble(Point(Observer::getInstance()->getEye().getx() + xpoz, Observer::getInstance()->getEye().gety() - 2, Observer::getInstance()->getEye().getz() - 2), (rand() % 5 + 1) / 100000.0));

	}

	glutPostRedisplay();
	glutTimerFunc(rand() % 90, createBubblesTimer, 0);
}

void waveFormTimer0(int i)
{
	if (wvs[0].waveNo == 19)
		wvs[0].waveNo = 0;
	else wvs[0].waveNo++;
	glutPostRedisplay();
	glutTimerFunc(wvs[0].waveTime, waveFormTimer0, 0);
}

void waveFormTimer1(int i)
{
	if (wvs[1].waveNo == 19)
		wvs[1].waveNo = 0;
	else wvs[1].waveNo++;
	glutPostRedisplay();
	glutTimerFunc(wvs[1].waveTime, waveFormTimer1, 0);
}

void waveFormTimer2(int i)
{
	if (wvs[2].waveNo == 19)
		wvs[2].waveNo = 0;
	else wvs[2].waveNo++;
	glutPostRedisplay();
	glutTimerFunc(wvs[2].waveTime, waveFormTimer2, 0);
}

void waveFormTimer3(int i)
{
	if (wvs[3].waveNo == 19)
		wvs[3].waveNo = 0;
	else wvs[3].waveNo++;
	glutPostRedisplay();
	glutTimerFunc(wvs[3].waveTime, waveFormTimer3, 0);
}

void waveFormTimer4(int i)
{
	if (wvs[4].waveNo == 19)
		wvs[4].waveNo = 0;
	else wvs[4].waveNo++;
	glutPostRedisplay();
	glutTimerFunc(wvs[4].waveTime, waveFormTimer4, 0);
}

void waveFormTimer5(int i)
{
	if (wvs[5].waveNo == 19)
		wvs[5].waveNo = 0;
	else wvs[5].waveNo++;
	glutPostRedisplay();
	glutTimerFunc(wvs[5].waveTime, waveFormTimer5, 0);
}

void waveFormTimer6(int i)
{
	if (wvs[6].waveNo == 19)
		wvs[6].waveNo = 0;
	else wvs[6].waveNo++;
	glutPostRedisplay();
	glutTimerFunc(wvs[6].waveTime, waveFormTimer6, 0);
}


int main(int argc, char **argv) 
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1024, 512);
	glutCreateWindow("Lumea din adancuri");

	init();

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
	glutMouseFunc(mouseFunc);
	glutKeyboardFunc(key);
	glutSpecialFunc(processSpecialKeys);
	changeTrajectoryTimer(0);
	setDirectionTimer(0);
	addCreature(0);
	setDirectionTimer(0);
	createBubblesTimer(0);
	waveFormTimer0(0);
	waveFormTimer1(0);
	waveFormTimer2(0);
	waveFormTimer3(0);
	waveFormTimer4(0);
	waveFormTimer5(0);
	waveFormTimer6(0);
	glutMainLoop();

	return 1;
}




