#pragma once

#include <stdlib.h>

#include "glslprogram.h"
#include "shapes.h"
#include "loader.h"
//#include <Eigen/Dense>

#define dt			0.01

using namespace std;
using namespace glm;
//using namespace Eigen;


float* Array3(float a, float b, float c);
float* MulArray3(float factor, float array0[3]);
void draw_cylinder(GLfloat radius, GLfloat height, float R, float G, float B, float A, vec3* start, vec3* end, float length);

/*****Basic Shape*****/

class Sphere
{
private:
	float radius;
	vec3 position;
	vec3 color;
	GLuint sphereModel;

	GLuint sphereTnum;
	unsigned char* sphereTexture;

	GLuint sphereNnum;
	unsigned char* sphereNormal;

	GLSLProgram* sphereShader;

public:
	Sphere()
	{
		radius = 0;
		position = vec3(0, 0, 0);
		color = vec3(0, 1, 1);
	}

	void Initialize(vec3 Position, float radius)
	{
		position = Position;

		/*****Sphere Part*****/

		//Create model

		int tw, th;

		glGenTextures(1, &sphereTnum);
		sphereTexture = BmpToTexture("./Object/Texture/NewTennisBallColor.bmp", &tw, &th);
		glBindTexture(GL_TEXTURE_2D, sphereTnum);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, tw, th, 0, GL_RGB, GL_UNSIGNED_BYTE, sphereTexture);

		glGenTextures(1, &sphereNnum);
		sphereNormal = BmpToTexture("./Object/Texture/TennisBallBump.bmp", &tw, &th);
		glBindTexture(GL_TEXTURE_2D, sphereNnum);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, tw, th, 0, GL_RGB, GL_UNSIGNED_BYTE, sphereNormal);

		sphereModel = glGenLists(1);
		glNewList(sphereModel, GL_COMPILE);
		{
			MjbSphere(radius, 100, 100);
		}
		glEndList();

		//Create shader 

		sphereShader = new GLSLProgram();
		bool valid = sphereShader->Create("lighting.vert", "lighting.frag");
		if (!valid)
		{
			fprintf(stderr, "Shader cannot be created!\n");
			exit(314);
		}
		else
		{
			fprintf(stderr, "Sphere Shader created.\n");
		}
		sphereShader->SetVerbose(false);
	}

	void Draw()
	{
		// since we are using glScalef( ), be sure normals get unitized:

		glEnable(GL_NORMALIZE);


		// draw the current object:

		/*****Draw the ship*****/

		glActiveTexture(GL_TEXTURE0); // use texture unit 0
		glBindTexture(GL_TEXTURE_2D, sphereTnum);

		glActiveTexture(GL_TEXTURE1); // use texture unit 1
		glBindTexture(GL_TEXTURE_2D, sphereNnum);

		vec3 specColor = vec3(0.5, 0.5, 0.5);
		float uKa = 0.2;
		float uKd = 0.6;
		float uKs = 0.4;
		float uShine = 0.4;

		sphereShader->Use();
		sphereShader->SetUniformVariable("uKa", uKa);
		sphereShader->SetUniformVariable("uKd", uKd);
		sphereShader->SetUniformVariable("uKs", uKs);
		sphereShader->SetUniformVariable("uSpecularColor", specColor);
		sphereShader->SetUniformVariable("uShininess", uShine);
		sphereShader->SetUniformVariable("uTexUnit", 0);
		sphereShader->SetUniformVariable("ifNormalMap", true);
		sphereShader->SetUniformVariable("uNorUnit", 1);
		{
			glPushMatrix();
			glTranslatef(position.x, position.y, position.z);
			glCallList(sphereModel);
			glPopMatrix();
		}
		sphereShader->Use(0);
	}
};

/*****IK Object*****/

struct IKarm
{
	IKarm* last;
	IKarm* next;
	float localArmLength;
	float localArmRotate;
	vec3 localPos;
	vec3 localTip;
	vec3 color;
};

class RoboArm
{
private:
	int numNodes;
	float maxLength;
	vec3 basePos;
	vec3 currentTip;
	IKarm** armList;

	vec3 FindTip();

public:
	int method;
	int ik;

	RoboArm()
	{
		numNodes = 0;
		method = 0;
		ik = 1;
		basePos = vec3(0, 0, 0);
		currentTip = vec3(0, 0, 0);
		armList = NULL;
	}

	void Initialize(int);

	void SetAngle(int, float);

	void Solve(vec3);
	void SVDSolve(vec3);

	void Draw();
};

/*****Game Objects*****/

/**
 * Projectile class
 * 
 * This is the class for all the projectiles, such as canon and bullet.
 *
 * @para startPos:	This is a vec3 variable store the start position of the object.
 * @para targetPos: This is a vec3 variable store the end position of the object.
 * @para direction: This is a vec3 variable store the direction of object as unit vector.
 * @para velocity:	This is a float variable store the velocity of the object move.
 * @para model:		This is a GLuint variable represent the ID of object's model,
					currently the model is a sphere.
 * @para pos:		This is a vec3 variable store the current live position of the object.
 
 * @func Projectile():	This is the constructor of the class.

 * @func Initialize():	This is the intialize process of the object.
 		#para start:	The start position of object.
 		#para end:		The end position of the object.
 		#func duty:		The function will store the passed-in position into variables,
						then it will calculate the direction vector as a unit vector based
						on the start and end position. The function will then auto-create a 
						model and set the initial velocity.
 * @func Move():		This is the update moving function for projectile.
 *		#func duty:		This function will be called by the update function of the game,
						it will update the position of the object by moving the object toward
						the forward direction with (velocity * dt). After the update the position,
						the function will use a dot product to check if the instance passes the
						destination, then return a boolean for if pass or not.
*		#return bool:	The function will return a boolean represent if the object passes
						the destination.
**/

class Projectile
{
private:

	vec3 startPos, targetPos;
	vec3 direction;
	double velocity;

	GLuint model;

public:

	vec3 pos;

	Projectile()
	{
		model = 0;
		velocity = 0;
	}

	void Initialize(vec3 start, vec3 end)
	{
		//Store the initial and target position
		startPos.x = start.x;
		startPos.y = start.y;
		startPos.z = start.z;
		targetPos.x = end.x;
		targetPos.y = end.y;
		targetPos.z = end.z;

		//Initialize the position
		pos = startPos;

		//Find the direction of moving
		direction.x = targetPos.x - startPos.x;
		direction.y = targetPos.y - startPos.y;
		direction.z = targetPos.z - startPos.z;
		float d = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
		direction.x /= d;
		direction.y /= d;
		direction.z /= d;

		//Create the model
		model = glGenLists(1);
		glNewList(model, GL_COMPILE);
		{
			glColor3f(1, 1, 0);
			MjbSphere(0.1f, 10, 10);
		}
		glEndList();

		velocity = 300.00f;
	}

	bool Move()
	{
		//Move the projectile
		pos.x += direction.x * velocity * dt;
		pos.y += direction.y * velocity * dt;
		pos.z += direction.z * velocity * dt;

		//Check if pass the target
		vec3 check = vec3(targetPos.x - pos.x, targetPos.y - pos.y, targetPos.z - pos.z);
		float dot = check.x * direction.x + check.y * direction.y + check.z * direction.z;

		//Draw the projectile
		glPushMatrix();
		{
			glTranslated(pos.x, pos.y, pos.z);
			glCallList(model);
		}
		glPopMatrix();

		//If pass the target
		if (dot < 0)
			return true;
		else
			return false;
	}
};

/**
 * Laser class
 
 * This is the class for all the laser type
 
 * @para startPos:	vec3 * variable store the start position of the object.
 * @para targetPos: vec3 * variable store the end position of the object.
 * @para direction:	vec3 variable store the direction of object as unit vector.
 * @para qobj:		GLUquadric * variable used to draw the cylinder.
 * @para color:		vec3 variable store the color of the laser.
 * @para duration:	float variable represent how long will the laser exist in the scene.
 * @para dCounter:	float variable used as a timer for laser.
 * @para radius:	float variable represent the maximum size of the laser.
 * @para height:	float variable represent the length of the laser.
 * @para ifOver:	boolean variable indicates if the laser was over or not
 
 * @func Laser():		This is the constructor of the class.

 * @func Initialize():	This is the intialize process of the object.
 		#para []:		The meaning was represented by the name.
		#fun duty:		The function will receive the input of all the variable and store them.		

 * @func Use():			This is the intialize process of the object.
 		#func duty:		The function will initialize all the variables and create a Cylinder
						model based on the stored variable.
 * @func Draw():		This is the draw function used for update the laser's state.
		#func duty:		While the laser is "alive", the function will change the radius of 
						the laser based on it's lifecycle, to create a kind of animation. After
						the lifecycle was finished, the laser will be set 0 and the ifOver flag
						will be set.
**/

class Laser
{
private:

	vec3 * startPos, * targetPos;
	vec3 direction;
	GLUquadric* qobj;
	vec3  color;
	float duration;
	float dCounter;
	float radius;
	float height;

public:

	bool  ifOver;

	Laser()
	{
		startPos = NULL;
		targetPos = NULL;

		ifOver = true;

		height = 0;
	}

	void Initialize(vec3* start, vec3* end, vec3 scolor, float sduration, float sradius)
	{
		startPos = start;
		targetPos = end;
		color = scolor;
		duration = sduration;
		radius = sradius;
		height = 0.0;
	}

	void Use()
	{
		dCounter = 0;
		ifOver = false;
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_FILL);
		gluQuadricNormals(qobj, GLU_SMOOTH);
	}

	void Draw()
	{
		if (!ifOver)
		{
			glPushMatrix();
			{
				//Calculate the length
				float d = sqrt((startPos->x - targetPos->x) * (startPos->x - targetPos->x) +
					(startPos->y - targetPos->y) * (startPos->y - targetPos->y) +
					(startPos->z - targetPos->z) * (startPos->z - targetPos->z));

				//**********Set the height
				height = d;

				//**********Set the radius
				float r = radius * sin(M_PI * (dCounter / duration));

				//**********Draw the laser
				{
					double alpha = 1.0;
					draw_cylinder(r, height, color.x, color.y, color.z, alpha, startPos, targetPos, d);
					alpha = 0.2;
					draw_cylinder(r * 1.5, height, color.x, color.y, color.z, alpha, startPos, targetPos, d);
				}
			}
			glPopMatrix();

			dCounter += dt;
		}

		//Check if over
		if (dCounter >= duration)
		{
			ifOver = true;
			height = 0;
		}
	}
};

/**
 * Explosion class

 * This is the class for all the explosion type

 * @para duration:	float variable to store the duration of the explosion.
   @para color:		vec3 variable represents the color of explosion.
   @para position:	vec3 variable to store the position of the explosion.
   @para radius:	float variable to store the maximum radius of explosion.
   @para dCounter:	float variable act as a timer to calculate the duration.
   @para ifOver:	boolean variable indicates if the animation was over.

 * @func Explosion():	This is the constructor of the class.
		#para x,y,z:	float variable represents the position of explosion.
		#para dd-,rr-:	float variable represents the duration and radius of explosion.
		#para r,g,b:	float variable represents the color parameter of the explosion.
		#func duty:		Receive input from outside and intialize all the parameters.

 * @func Use():			This is the intialize process of the object.
		#func duty:		The function will initialize all the variables and create a Cylinder
						model based on the stored variable.
 * @func Draw():		This is the draw function used for update the laser's state.
		#func duty:		While the laser is "alive", the function will change the radius of
						the laser based on it's lifecycle, to create a kind of animation. After
						the lifecycle was finished, the laser will be set 0 and the ifOver flag
						will be set.
**/

class Explosion
{
private:

	float duration;
	vec3  color;
	vec3  position;
	float radius;

public:

	float dCounter;
	bool  ifOver;

	Explosion(float x, float y, float z, float dduration, float rradius, float r, float g, float b)
	{
		radius = rradius;
		duration = dduration;
		color.x = r;
		color.y = g;
		color.z = b;
		position.x = x;
		position.y = y;
		position.z = z;

		dCounter = 0;
		ifOver = true;
	}

	void Use()
	{
		dCounter = 0;
		ifOver = false;
	}

	void Draw()
	{
		if (!ifOver)
		{
			glPushMatrix();
			{
				glColor3f(color.x, color.y, color.z);
				glTranslatef(position.x, position.y, position.z);

				glutSolidSphere(radius * sin(M_PI * dCounter / duration), 10, 10);

				glColor4f(color.x, color.y, color.z, 0.1);

				glutSolidSphere(radius * 10 * sin((M_PI / 2) * (dCounter / 4) / duration), 10, 10);
			}
			glPopMatrix();

			dCounter += dt;
		}

		//Check if over
		if (dCounter >= duration)
		{
			ifOver = true;
		}
	}
};

class Object
{
protected:
	vec3 forward, left, up;

public:
	vec3 position, rotation;

	Object()
	{
		position = vec3(0, 0, 0);
		rotation = vec3(0, 0, 0);
	}
};

class TDrone : public Object
{
protected:

	GLuint tmodel;

public:

	int type;	//1 for aircraft, 2 for destroyer or lower, 3 for crusier and upper

	TDrone(int ttype)
	{
		type = ttype;

		tmodel = glGenLists(1);
		glNewList(tmodel, GL_COMPILE);
		{
			glPushMatrix();
			glColor3d(0, 1, 1);
			MjbSphere(1, 10, 10);
			glPopMatrix();
		}
		glEndList();
	}

	void SetPosition(float x, float y, float z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
	};

	void Draw()
	{
		glPushMatrix();
		{
			glTranslatef(position.x, position.y, position.z);
			glColor3d(0, 1, 1);
			MjbSphere(1, 10, 10);
		}
		glPopMatrix();
	};
};

class Turret : public Object
{
protected:
	float angularVelocity;
	float coolDown;
	float coolDownCounter;
	bool ifCooling;
	Object* target;

public:
	int type;				//1 for antiair, 2 for medium, 3 for large
	vec3 position;
	vec3 rotation;
	float maxAngle;
	bool isTracking;

protected:
	//Turret model related
	GLuint turretModel;
	GLuint turretTnum;
	unsigned char* turretTexture;
	GLSLProgram* turretShader;

	//The fire point
	GLuint firePoint;
	vec3 firePos;

public:
	Turret(float baseY)
	{
		forward = vec3(1, 0, 0);
		left = vec3(0, 0, -1);
		up = vec3(0, 1, 0);

		maxAngle = baseY + 200.0f;

		target = NULL;

		ifCooling = false;
	};

	void SetPosition(float x, float y, float z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
	};

	void SetRotation(float rx, float ry, float rz)
	{
		//Set the vector

		rotation.x = rx;
		rotation.y = ry;
		rotation.z = rz;

		//Set the direction vector

		mat4 rotationM = mat4(1.0f);
		rotationM = rotate(rotationM, float((rx / 180.0f) * M_PI), vec3(1, 0, 0));
		rotationM = rotate(rotationM, float((ry / 180.0f) * M_PI), vec3(0, 1, 0));
		rotationM = rotate(rotationM, float((rz / 180.0f) * M_PI), vec3(0, 0, 1));

		vec4 f = vec4(forward.x, forward.y, forward.z, 1);
		vec4 l = vec4(left.x, left.y, left.z, 1);
		vec4 u = vec4(up.x, up.y, up.z, 1);

		f = f * rotationM;
		l = l * rotationM;
		u = u * rotationM;

		forward = vec3(f.x, f.y, f.z);
		left = vec3(l.x, l.y, l.z);
		up = vec3(u.x, u.y, u.z);
	};

	void Rotate(double ry)
	{
		//This is for the lower medium turret specifically
		if (rotation.x == 180)
			rotation.y += ry;
		else
			rotation.y -= ry;

		//Set the direction vector

		mat4 rotationM = mat4(1.0f);
		if (ry != 0)
			rotationM = rotate(rotationM, float((ry / 180.0f) * M_PI), vec3(0, 1, 0));

		vec4 f = vec4(forward.x, forward.y, forward.z, 1);
		vec4 l = vec4(left.x, left.y, left.z, 1);

		f = f * rotationM;
		l = l * rotationM;

		forward = vec3(f.x, f.y, f.z);
		left = vec3(l.x, l.y, l.z);
	};

	void Track(Object* nTarget)
	{
		target = nTarget;
	};

	void Tracking()
	{
		if (target != NULL)
		{
			//Find the difference between target position and current forward
			vec3 targetPos = vec3(target->position.x, 0, target->position.z);
			targetPos.x -= position.x;
			targetPos.z -= position.z;

			//Calculate the dot product using atan2, not acos since it will create problem
			double angle = 0;
			angle = atan2(targetPos.z, targetPos.x) - atan2(forward.z, forward.x);

			if (angle > M_PI) { angle -= 2 * M_PI; }
			else if (angle <= -M_PI) { angle += 2 * M_PI; }

			//printf("theta is: %f\n", angle);

			//Rotate the turret
			Rotate(angle);
		}
	};

	bool IfNeedCool()
	{
		return ifCooling;
	}

	void Fire(Projectile **plist, int pCap, Laser **llist, int lCap)
	{
		if (target != NULL && !ifCooling)
		{
			if (type == 1)		//The anti air turret
			{
				//Add some bias
				vec3 des = target->position;
				double chance = rand() % 100;
				if (chance < 90)
				{
					double bx = rand() % 5 - 2.5;
					double by = rand() % 5 - 2.5;
					double bz = rand() % 5 - 2.5;
					des.x += bx;
					des.y += by;
					des.z += bz;
				}

				//Loop through the plist to find the place
				for (int i = 0; i < pCap; i++)
				{
					if (plist[i] == NULL)
					{
						//printf("Fire function called!\n");
						plist[i] = new Projectile();

						plist[i]->Initialize(position, des);

						ifCooling = true;

						break;
					}
				}
			}
			else if (type == 2)		//The medium turret
			{

				//Loop through the llist to find the place
				for (int i = 0; i < lCap; i++)
				{
					if (llist[i] == NULL)
					{
						llist[i] = new Laser();

						llist[i]->Initialize(&firePos,
							&(target->position),
							vec3(1.0, 0.0, 0.0),
							2,
							0.04);

						llist[i]->Use();

						ifCooling = true;

						break;
					}
				}
			}
			else if (type == 3)		//The medium turret
			{
				//Add some bias
				vec3 des = target->position;

				//Loop through the plist to find the place
				for (int i = 0; i < lCap; i++)
				{
					if (llist[i] == NULL)
					{
						llist[i] = new Laser();

						llist[i]->Initialize(&firePos,
							&(target->position),
							vec3(0.5, 0.0, 0.5),
							3,
							0.1);

						llist[i]->Use();

						ifCooling = true;

						break;
					}
				}
			}
		}
	};

	void CoolDown()
	{
		coolDownCounter += dt + (rand() % 10 / 10);
		//Add some random factor
		if (coolDownCounter >= coolDown + rand() % 5)
		{
			ifCooling = false;
			coolDownCounter = 0;
		}
	}

	void CreateModel(int ttype)
	{
		type = ttype;

		coolDownCounter = 0;

		isTracking = false;

		//Set the angular velocity and initialize all the factor

		if (type == 1)
		{
			angularVelocity = 10;
			coolDown = 0.8;
		}
		else if (type == 2)
		{
			angularVelocity = 8;
			coolDown = 5.0;
		}
		else if (type == 3)
		{
			angularVelocity = 5;
			coolDown = 10.0;
		}

		/*****Turret Part*****/

		//Create model

		int tw, th;

		glGenTextures(1, &turretTnum);

		if (type == 1)
			turretTexture = BmpToTexture("./Blender Model/Crusier/Anit turret/Antiair_turret.bmp", &tw, &th);
		else if (type == 2)
			turretTexture = BmpToTexture("./Blender Model/Crusier/Medium turret/Medium_laser_turret.bmp", &tw, &th);
		else if (type == 3)
			turretTexture = BmpToTexture("./Blender Model/Crusier/Large turret/Large_laser_turret.bmp", &tw, &th);

		glBindTexture(GL_TEXTURE_2D, turretTnum);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, tw, th, 0, GL_RGB, GL_UNSIGNED_BYTE, turretTexture);

		//Create the model and transpose to the set position
		turretModel = glGenLists(1);
		glNewList(turretModel, GL_COMPILE);
		{
			if (type == 1)
				LoadObjFile("./Blender Model/Crusier/Anit turret/Anti.obj");
			else if (type == 2)
				LoadObjFile("./Blender Model/Crusier/Medium turret/Medium_turret.obj");
			else if (type == 3)
				LoadObjFile("./Blender Model/Crusier/Large turret/Large_turret.obj");
		}
		glEndList();

		//Create shader 

		turretShader = new GLSLProgram();
		bool valid = turretShader->Create("lighting.vert", "lighting.frag");
		if (!valid)
		{
			fprintf(stderr, "Shader cannot be created!\n");
			exit(314);
		}
		else
		{
			fprintf(stderr, "Turret Shader created.\n");
		}
		turretShader->SetVerbose(false);

		/*****Fire point Part*****/

		//Create the model and transpose to the set position
		firePoint = glGenLists(1);
		glNewList(firePoint, GL_COMPILE);
		{
			glPushMatrix();
			if (type == 1)
			{
				glColor3d(0, 1, 1);
				glTranslated(0.2, 0, 0);
				firePos = vec3(0.2 + position.x, 0 + position.y, 0 + position.z);
				MjbSphere(0.1, 10, 10);
			}
			else if (type == 2)
			{
				glColor3d(1, 0, 1);
				glTranslated(0.6, 0, 0);
				firePos = vec3(0.6 + position.x, 0 + position.y, 0 + position.z);
				MjbSphere(0.1, 10, 10);
			}
			else if (type == 3)
			{
				glColor3d(1, 1, 0);
				glTranslated(0.7, 0, 0);
				firePos = vec3(0.7 + position.x, 0 + position.y, 0 + position.z);
				MjbSphere(0.1, 10, 10);
			}
			glPopMatrix();
		}
		glEndList();
	};

	void Draw()
	{
		//Update the necessary position

		if (type == 1)
		{
			firePos = vec3(forward.x * 0.2 + position.x, forward.y * 0.2 + position.y, forward.z * 0.2 + position.z);
		}
		else if (type == 2)
		{
			firePos = vec3(forward.x * 0.5 + position.x, forward.y * 0.5 + position.y, forward.z * 0.5 + position.z);
		}
		else if (type == 3)
		{
			firePos = vec3(forward.x * 0.7 + position.x, forward.y * 0.7 + position.y, forward.z * 0.7 + position.z);
		}

		//Draw the turret

		glActiveTexture(GL_TEXTURE1); // use texture unit 0
		glBindTexture(GL_TEXTURE_2D, turretTnum);

		vec3 specColor = vec3(1.0, 1.0, 1.0);
		float uKa = 0.3;
		float uKd = 0.2;
		float uKs = 0.6;
		float uShine = 15.0;

		turretShader->Use();
		turretShader->SetUniformVariable("uKa", uKa);
		turretShader->SetUniformVariable("uKd", uKd);
		turretShader->SetUniformVariable("uKs", uKs);
		turretShader->SetUniformVariable("uSpecularColor", specColor);
		turretShader->SetUniformVariable("uShininess", uShine);
		turretShader->SetUniformVariable("uTexUnit", 1);
		{
			glPushMatrix();

			//Rotate the ship based on the rotation parameter
			glTranslatef(position.x, position.y, position.z);
			glRotatef(rotation.x, 1, 0, 0);
			glRotatef(rotation.y, 0, 1, 0);
			glRotatef(rotation.z, 0, 0, 1);

			glCallList(turretModel);

			glPopMatrix();
		}
		turretShader->Use(0);

		//Debug: Draw the 3 direction vectors and target line
		/*

		glBegin(GL_LINES);
		{
			glColor3d(1, 0, 0);
			glVertex3d(position.x, position.y, position.z);
			glVertex3d(position.x + forward.x * 1, position.y + forward.y * 1, position.z + forward.z * 1);

			glColor3d(0, 1, 0);
			glVertex3d(position.x, position.y, position.z);
			glVertex3d(position.x + left.x * 1, position.y + left.y * 1, position.z + left.z * 1);

			glColor3d(0, 0, 1);
			glVertex3d(position.x, position.y, position.z);
			glVertex3d(position.x + up.x * 1, position.y + up.y * 1, position.z + up.z * 1);

			if (target != NULL)
			{
				glColor3d(0, 1, 1);
				glVertex3d(position.x, position.y, position.z);
				glVertex3d(target->position.x, target->position.y, target->position.z);
			}
		}
		glEnd();

		//Draw the fire point

		glPushMatrix();

		//Rotate the ship based on the rotation parameter
		glTranslatef(position.x, position.y, position.z);
		glRotatef(rotation.x, 1, 0, 0);
		glRotatef(rotation.y, 0, 1, 0);
		glRotatef(rotation.z, 0, 0, 1);

		glCallList(firePoint);

		glPopMatrix();
		*/

		//Debug: End here
	};
};

class Ship : public Object
{
protected:
	float hp;
	float shield;
	float velocity;
	float angularVelocity;
	Turret* turrets[22];
	Ship** targetList;
	bool ifMoveToPoint;
	vec3 MovePosition;

	//Ship model related
	GLuint shipModel;
	GLuint shipTnum;
	unsigned char* shipTexture;
	GLSLProgram* shipShader;

	//Ship shield related
	GLuint shieldModel;
	GLSLProgram* shieldShader;

public:
	void Move(vec3 direction)
	{
		float x = direction.x;
		float y = direction.y;
		float z = direction.z;
		float u = sqrt(x * x + y * y + z * z);
		x /= u;
		y /= u;
		z /= u;

		position.x += x * velocity * dt;
		position.y += y * velocity * dt;
		position.z += z * velocity * dt;

		//Move the turret
		for (int i = 0; i < 22; i++)
		{
			turrets[i]->position.x += x * velocity * dt;
			turrets[i]->position.y += y * velocity * dt;
			turrets[i]->position.z += z * velocity * dt;
		}
	};

	//This function is used to set task for each turrets, act like an AI.
	void Track(Ship** target)
	{
		//Store the target
		targetList = target;

		//Set tracking for each turret
		for (int i = 0; i < 22; i++)
		{
			//Set free turret to track target
			if (!turrets[i]->isTracking)
			{
				//Random set target
				int pos = rand() % 10;
				turrets[i]->Track(target[pos]);
				turrets[i]->isTracking = true;
			}
		}
	};

	//This function will be called during each display phase to update the tracking information.
	void Tracking()
	{
		for (int i = 0; i < 22; i++)
		{
			turrets[i]->Tracking();
		}
	};

	void CreateModel(vec3 Position)
	{
		position = Position;

		ifMoveToPoint = false;

		velocity = 2;

		/*****Ship Part*****/

		//Create model

		int tw, th;

		glGenTextures(1, &shipTnum);
		shipTexture = BmpToTexture("./Blender Model/Crusier/Ship/Crusier_ship.bmp", &tw, &th);
		glBindTexture(GL_TEXTURE_2D, shipTnum);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, tw, th, 0, GL_RGB, GL_UNSIGNED_BYTE, shipTexture);

		shipModel = glGenLists(1);
		glNewList(shipModel, GL_COMPILE);
		{
			glPushMatrix();
			glTranslated(0, 0, -1.5);
			LoadObjFile("./Blender Model/Crusier/Ship/Crusier_ship.obj");
			glPopMatrix();
		}
		glEndList();

		//Create shader 

		shipShader = new GLSLProgram();
		bool valid = shipShader->Create("lighting.vert", "lighting.frag");
		if (!valid)
		{
			fprintf(stderr, "Shader cannot be created!\n");
			exit(314);
		}
		else
		{
			fprintf(stderr, "Ship Shader created.\n");
		}
		shipShader->SetVerbose(false);

		/*****Shield Part*****/

		//Create list

		shieldModel = glGenLists(1);
		glNewList(shieldModel, GL_COMPILE);
		{
			MjbSphere(5.0, 150, 150);
		}
		glEndList();

		//Create shader

		shieldShader = new GLSLProgram();
		valid = shieldShader->Create("shield.vert", "shield.frag");
		if (!valid)
		{
			fprintf(stderr, "Shader cannot be created!\n");
			exit(314);
		}
		else
		{
			fprintf(stderr, "Shield Shader created.\n");
		}
		shieldShader->SetVerbose(false);

		/*****Turret Part*****/

		//The antiair turret part

		turrets[0] = new Turret(0);
		turrets[0]->CreateModel(1);
		turrets[0]->SetPosition(0.60 + position.x, 0.37 + position.y, 0.2 + position.z);

		turrets[1] = new Turret(0);
		turrets[1]->CreateModel(1);
		turrets[1]->SetPosition(0.60 + position.x, 0.37 + position.y, 1.4 + position.z);

		turrets[2] = new Turret(0);
		turrets[2]->CreateModel(1);
		turrets[2]->SetPosition(0.60 + position.x, 0.27 + position.y, 3.0 + position.z);

		turrets[3] = new Turret(180);
		turrets[3]->CreateModel(1);
		turrets[3]->SetRotation(0, 180, 180);
		turrets[3]->SetPosition(0.7 + position.x, -0.42 + position.y, 0.2 + position.z);

		turrets[4] = new Turret(180);
		turrets[4]->CreateModel(1);
		turrets[4]->SetRotation(0, 180, 180);
		turrets[4]->SetPosition(0.7 + position.x, -0.23 + position.y, 1.4 + position.z);

		turrets[5] = new Turret(180);
		turrets[5]->CreateModel(1);
		turrets[5]->SetRotation(0, 180, 180);
		turrets[5]->SetPosition(0.7 + position.x, -0.23 + position.y, 3.0 + position.z);

		turrets[6] = new Turret(180);
		turrets[6]->CreateModel(1);
		turrets[6]->SetRotation(0, 180, 180);
		turrets[6]->SetPosition(0.7 + position.x, -0.42 + position.y, -1 + position.z);

		turrets[7] = new Turret(180);
		turrets[7]->CreateModel(1);
		turrets[7]->SetRotation(0, 180, 180);
		turrets[7]->SetPosition(0.7 + position.x, -0.42 + position.y, -2.2 + position.z);

		turrets[8] = new Turret(180);
		turrets[8]->CreateModel(1);
		turrets[8]->SetRotation(0, 180, 180);
		turrets[8]->SetPosition(0.7 + position.x, -0.42 + position.y, -3.2 + position.z);

		for (int i = 9; i < 18; i++)
		{
			turrets[i] = new Turret(turrets[i - 9]->maxAngle);
			turrets[i]->CreateModel(1);
			turrets[i]->SetRotation(turrets[i - 9]->rotation.x, turrets[i - 9]->rotation.y, turrets[i - 9]->rotation.z);
			turrets[i]->SetPosition(-1 * turrets[i - 9]->position.x + 2 * position.x,
				turrets[i - 9]->position.y,
				turrets[i - 9]->position.z);
		}

		//The medium turret part

		turrets[18] = new Turret(0);
		turrets[18]->CreateModel(2);
		turrets[18]->SetPosition(-0.05 + position.x, 0.47 + position.y, 0.75 + position.z);

		turrets[19] = new Turret(0);
		turrets[19]->CreateModel(2);
		turrets[19]->SetPosition(-0.05 + position.x, 0.37 + position.y, 2.35 + position.z);

		turrets[20] = new Turret(0);
		turrets[20]->CreateModel(2);
		turrets[20]->SetRotation(180, 0, 0);
		turrets[20]->SetPosition(-0.05 + position.x, -0.33 + position.y, 2.35 + position.z);

		//The large turret part

		turrets[21] = new Turret(0);
		turrets[21]->CreateModel(3);
		turrets[21]->SetPosition(-0.05 + position.x, 0.75 + position.y, -0.75 + position.z);
	};

	void Draw()
	{
		// since we are using glScalef( ), be sure normals get unitized:

		glEnable(GL_NORMALIZE);


		// draw the current object:

		/*****Draw the ship*****/

		glActiveTexture(GL_TEXTURE0); // use texture unit 0
		glBindTexture(GL_TEXTURE_2D, shipTnum);

		vec3 specColor = vec3(1.0, 1.0, 1.0);
		float uKa = 0.3;
		float uKd = 0.8;
		float uKs = 0.6;
		float uShine = 150.0;

		shipShader->Use();
		shipShader->SetUniformVariable("uKa", uKa);
		shipShader->SetUniformVariable("uKd", uKd);
		shipShader->SetUniformVariable("uKs", uKs);
		shipShader->SetUniformVariable("uSpecularColor", specColor);
		shipShader->SetUniformVariable("uShininess", uShine);
		shipShader->SetUniformVariable("uTexUnit", 0);
		{
			glPushMatrix();
			glTranslatef(position.x, position.y, position.z);
			glCallList(shipModel);
			glPopMatrix();
		}
		shipShader->Use(0);


		/*****Draw the turret*****/

		for (int i = 0; i < 22; i++)
		{
			turrets[i]->Draw();
			//std::cout << turrets[i]->position.x << " " << turrets[i]->position.y << " " << turrets[i]->position.z << " " << std::endl;
		}


		/*****Draw the shield*****/

		vec3 list[6] = { vec3(0.0, 0.0, 5.0),
						vec3(0.0, 5.0, 0.0),
						vec3(5.0, 0.0, 0.0),
						vec3(0.0, 0.0, -5.0),
						vec3(0.0, -5.0, 0.0),
						vec3(-5.0, 0.0, 0.0) };

		int x = rand() % 6;

		vec3 startPos = list[int(x)];

		//Generate the shield factor

		shieldShader->Use();
		shieldShader->SetUniformVariable("angle", 0.0f);
		shieldShader->SetUniformVariable("fangle", 0.0f);
		shieldShader->SetUniformVariable("startPos", startPos);
		{
			glPushMatrix();
			glTranslatef(position.x, position.y, position.z);
			//glCallList(shieldModel);
			glPopMatrix();
		}
		shieldShader->Use(0);
	};

	void Fire(Projectile **plist, int pCap, Laser **llist, int lCap)
	{
		for (int i = 0; i < 22; i++)
		{
			turrets[i]->Fire(plist, pCap, llist, lCap);
		}
	};

	void SMove()
	{
		//Select a place in the space
		if (!ifMoveToPoint)
		{
			double xc = rand() % 500 - 250;
			double yc = rand() % 500 - 250;
			double zc = rand() % 500 - 250;
			MovePosition = vec3(xc, yc, zc);

			ifMoveToPoint = true;
		}

		//Move
		if (ifMoveToPoint)
		{
			vec3 dir = vec3(MovePosition.x - position.x,
				MovePosition.y - position.y,
				MovePosition.z - position.z);
			dir = normalize(dir);
			Move(dir);

			//Check distance
			float d = sqrt(pow(MovePosition.x - position.x, 2) +
				pow(MovePosition.y - position.y, 2) +
				pow(MovePosition.z - position.z, 2));
			if (d < 10)
			{
				ifMoveToPoint = false;
			}
		}
	}

	void CoolDown()
	{
		for (int i = 0; i < 22; i++)
		{
			if (turrets[i]->IfNeedCool())
				turrets[i]->CoolDown();
		}
	}
};

/*****System Object*****/

class Game
{
private:

	Ship** playerShip, ** enemyShip;
	Projectile** plist;
	Explosion** elist;
	Laser** llist;

	int plistCap;
	int elistCap;
	int llistCap;
	int playerCap;
	int enemyCap;

public:

	Game()
	{
		plistCap = 1000;
		elistCap = 1000;
		llistCap = 100;
		playerCap = enemyCap = 10;
	}

	void Initialize()
	{
		plist = new Projectile * [plistCap];
		for (int i = 0; i < plistCap; i++)
		{
			plist[i] = NULL;
		}

		elist = new Explosion * [elistCap];
		for (int i = 0; i < elistCap; i++)
		{
			elist[i] = NULL;
		}

		llist = new Laser * [llistCap];
		for (int i = 0; i < llistCap; i++)
		{
			llist[i] = NULL;
		}

		playerShip = new Ship * [playerCap];
		enemyShip = new Ship * [enemyCap];

		for (int i = 0; i < 10; i++)
		{
			//Get player ship's position
			vec3 pos = vec3(0, -10 + i * 5, -50 + i * 20);
			playerShip[i] = new Ship;
			playerShip[i]->CreateModel(pos);

			pos = vec3(-500, -10 + i * 5, -50 + i * 20);
			enemyShip[i] = new Ship;
			enemyShip[i]->CreateModel(pos);
		}
	}

	void UpdateEffect()
	{
		//Update the object list
		for (int i = 0; i < plistCap; i++)
		{
			if (plist[i] != NULL)
			{
				//If the projectile reaches the destination
				if (plist[i]->Move())
				{
					//Generate the explosion
					for (int j = 0; j < elistCap; j++)
					{
						if (elist[j] == NULL)
						{
							elist[j] = new Explosion(
								plist[i]->pos.x,
								plist[i]->pos.y,
								plist[i]->pos.z,
								0.4,
								0.3,
								1.0,
								1.0,
								0.0);

							elist[j]->Use();

							break;
						}
					}

					//Clear the slot
					free(plist[i]);
					plist[i] = NULL;
				}
			}
		}

		//Update the explosion list
		for (int i = 0; i < elistCap; i++)
		{
			if (elist[i] != NULL)
			{
				//Draw the explosion
				elist[i]->Draw();

				if (elist[i]->ifOver)
				{
					//Clear the slot
					free(elist[i]);
					elist[i] = NULL;
				}
			}
		}


		//Update the laser list
		for (int i = 0; i < llistCap; i++)
		{
			if (llist[i] != NULL)
			{
				//Draw the explosion
				llist[i]->Draw();

				if (llist[i]->ifOver)
				{
					//Clear the slot
					free(llist[i]);
					llist[i] = NULL;
				}
			}
		}
	}

	void UpdateShip()
	{
		for (int i = 0; i < 10; i++)
		{
			playerShip[i]->SMove();
			playerShip[i]->Draw();

			enemyShip[i]->SMove();
			enemyShip[i]->Draw();
		}
	}

	void TrackShip()
	{
		for (int i = 0; i < 10; i++)
		{
			playerShip[i]->Track(enemyShip);
			playerShip[i]->Tracking();
			playerShip[i]->Fire(plist, plistCap, llist, llistCap);

			enemyShip[i]->Track(playerShip);
			enemyShip[i]->Tracking();
			enemyShip[i]->Fire(plist, plistCap, llist, llistCap);
		}
	}

	void PrepareShip()
	{
		//Cooling down
		for (int i = 0; i < 5; i++)
		{
			playerShip[i]->CoolDown();
			enemyShip[i]->CoolDown();
		}
	}
};

class Camera
{
private:
	vec3 eyePos;
	vec3 lookAtPos;
	vec3 focusPos;
	vec3 upVec;

public:
	Camera()
	{
		eyePos = vec3(0., 0., 10.);
		lookAtPos = vec3(0., 0., 0.);
		focusPos = vec3(0., 0., 0.);
		upVec = vec3(0., 1., 0.);
	}

	void MoveCamera(float x, float y, float z)
	{
		eyePos += vec3(x, y, z);
		lookAtPos += vec3(x, y, z);
	}

	void RotateCamera(float x, float y)
	{
		
	}
};


