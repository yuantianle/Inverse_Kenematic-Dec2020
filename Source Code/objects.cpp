#include "objects.h"

float* Array3(float a, float b, float c)
{
	static float array[4];
	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

float* MulArray3(float factor, float array0[3])
{
	static float array[4];
	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}

void draw_cylinder(GLfloat radius, GLfloat height, float R, float G, float B, float A, vec3* start, vec3* end, float length)
{
	GLfloat x = 0.0;
	GLfloat y = 0.0;
	GLfloat angle = 0.0;
	GLfloat angle_stepsize = 0.1;

	glPushMatrix();
	glTranslatef(start->x, start->y, start->z);

	//Calculate the direction
	vec3 direction = vec3((end->x - start->x) / length, (end->y - start->y) / length, (end->z - start->z) / length);

	direction = normalize(direction);

	//**********Rotate to the direction

	vec3 cur = vec3(0, 0, 1);

	// first rotation
	glRotatef(-atan2(direction.z, direction.x) * 180 / M_PI, 0, 1, 0);

	// second rotation    
	float d = sqrt(pow(direction.x, 2) + pow(direction.z, 2));
	float pitch = atan2(direction.y, d);
	glRotatef(pitch * 180 / M_PI, 0, 0, 1);

	glRotatef(90, 0, 1, 0);

	/** Draw the tube */
	glColor4f(R, G, B, A);
	glBegin(GL_QUAD_STRIP);
	angle = 0.0;
	while (angle < 2 * M_PI) {
		x = radius * cos(angle);
		y = radius * sin(angle);
		glVertex3f(x, y, height);
		glVertex3f(x, y, 0.0);
		angle = angle + angle_stepsize;
	}
	glVertex3f(radius, 0.0, height);
	glVertex3f(radius, 0.0, 0.0);
	glEnd();

	/** Draw the circle on top of cylinder */
	glColor4f(R, G, B, A);
	glBegin(GL_POLYGON);
	angle = 0.0;
	while (angle < 2 * M_PI) {
		x = radius * cos(angle);
		y = radius * sin(angle);
		glVertex3f(x, y, height);
		angle = angle + angle_stepsize;
	}
	glVertex3f(radius, 0.0, height);
	glEnd();
	glPopMatrix();
}


/*****IK Object*****/

/*
* All the arms should be initialy set to lay to x+
*/

void RoboArm::Initialize(int numN)
{
	maxLength = 0;
	numNodes = numN;
	if (armList == NULL)
	{
		armList = new IKarm * [numNodes];
		for (int i = 0; i < numNodes; i++)
		{
			//Create new IKarm in the list
			armList[i] = new IKarm;

			//Initialize the property
			armList[i]->localArmLength = 5.0f;
			armList[i]->localArmRotate = 18.0f;

			//Add into the count of the maxLength
			maxLength += armList[i]->localArmLength;

			//Set the last linker
			if (i == 0)
			{
				armList[i]->last = NULL;

				//Set the base for the very first arm
				armList[i]->localPos = vec3(0, 0, 0);
			}
			else
			{
				armList[i]->last = armList[i - 1];
			}

			//Set the next linker
			if (i == numNodes - 1)
			{
				armList[i]->next = NULL;
			}
			else
			{
				armList[i]->next = armList[i + 1];
			}

			//Calculate the local pos
			if (armList[i]->last != NULL)
			{
				armList[i]->localPos = armList[i]->last->localTip;
			}

			//Calculate the local tip
			armList[i]->localTip = armList[i]->localPos + vec3(armList[i]->localArmLength, 0, 0);

			//Set the color of arm
			float r = 1 - i * 40 / 255.0f;
			float g = i * 40 / 255.0f;
			float b = i * 40 / 255.0f;
			armList[i]->color = vec3(r, g, b);
		}
	}
}

void RoboArm::SetAngle(int node, float angle)
{
	armList[node]->localArmRotate = angle;
	FindTip();
}

vec3 RoboArm::FindTip()
{
	if (armList != NULL)
	{
		float currentAngle = 0.0f;
		vec3 currentPos = vec3(0, 0, 0);

		for (int i = 0; i < numNodes; i++)
		{
			//Find current angle
			currentAngle += armList[i]->localArmRotate;
			while (currentAngle > 360)
				currentAngle -= 360;
			while (currentAngle < 0)
				currentAngle += 360;

			//Calculate the new distance
			float dx = armList[i]->localArmLength * cos(M_PI * currentAngle / 180.0f);
			float dy = armList[i]->localArmLength * sin(M_PI * currentAngle / 180.0f);

			//Update the local pos in case error
			if (armList[i]->last != NULL)
			{
				armList[i]->localPos = armList[i]->last->localTip;
			}

			//Update the local tip
			armList[i]->localTip = armList[i]->localPos + vec3(dx, dy, 0);

			//Count in total
			currentPos.x += dx;
			currentPos.y += dy;
		}
		return currentPos;
	}
	return vec3(0, 0, 0);
}

void RoboArm::Solve(vec3 targetPos)
{
	if (armList != NULL)
	{
		for (int i = 0; i < numNodes; i++)
		{
			//Find the current tip and Update all the arms
			vec3 currentTip = FindTip();

			//1. Calculate the current arm vector
			vec3 currentVec = currentTip - armList[i]->localPos;

			//2. Calculate the current target vector
			vec3 currentTar = targetPos - armList[i]->localPos;

			//3. Find the difference between current and target
			float thetaS = atan2(currentTar.y, currentTar.x);
			float theta = atan2(currentVec.y, currentVec.x);
			float dtheta = thetaS - theta;

			//4. Update the current arm's rotation to match the target
			armList[i]->localArmRotate += (float)dtheta * 180.0f / M_PI;
		}
	}
}

void RoboArm::SVDSolve(vec3 targetPos)
{
	//targetPos *= maxLength / sqrt(pow(targetPos.x, 2) + pow(targetPos.y, 2));
	MatrixXf m = MatrixXf(3, numNodes);
	for (int i = 0; i < numNodes; i++)
	{
		//Find the current tip and Update all the arms
		vec3 currentTip = FindTip();

		//1. Calculate the current arm vector
		vec3 currentVec = currentTip - armList[i]->localPos;

		//2. Find the element of jacobian matrix
		vec3 elemJaco = cross(vec3(0, 0, 1), currentVec);

		//3. Form the Jacobian Matrix
		m(0, i) = elemJaco.x;
		m(1, i) = elemJaco.y;
		m(2, i) = elemJaco.z;
	}

	//4. Find the target vector
	vec3 currentTip = FindTip();
	vec3 targetVec = targetPos - currentTip;

	//Cast the targetVec
	//if (targetVec.length() > maxLength)
		//targetVec *= (maxLength / targetVec.length());

	Vector3f target(targetVec.x, targetVec.y, targetVec.z);

	JacobiSVD<MatrixXf> svd(m, ComputeThinU | ComputeThinV);

	//Apply the result
	for (int i = 0; i < numNodes; i++)
	{
		armList[i]->localArmRotate += svd.solve(target).row(i).col(0).value();
	}
	//cout << svd.solve(target).row << endl;

	//cout << "Solition is:\n" << svd.solve(target) << endl;
}

void RoboArm::Draw()
{
	float currentAngle = 0;
	//Update once
	vec3 tip = FindTip();

	//Draw Origin and Tip
	glPushMatrix();
	glColor3f(1, 1, 1);
	glPointSize(20);
	glBegin(GL_POINTS);
	{
		glVertex3d(tip.x, tip.y, tip.z);
		glVertex3d(0, 0, 0);
	}
	glEnd();
	glPopMatrix();

	for (int i = 0; i < numNodes; i++)
	{
		//Update the total angle
		currentAngle += armList[i]->localArmRotate;

		//Draw the arms
		glPushMatrix();
		{
			//Draw the cylinder
			draw_cylinder(0.25,
				armList[i]->localArmLength,
				armList[i]->color.r, armList[i]->color.g, armList[i]->color.b, 1,
				new vec3(armList[i]->localPos.x, armList[i]->localPos.y, armList[i]->localPos.z),
				new vec3(armList[i]->localTip.x, armList[i]->localTip.y, armList[i]->localTip.z),
				1);

			//Draw a small sphere
			glColor3f(armList[i]->color.r, armList[i]->color.g, armList[i]->color.b);
			glTranslatef(armList[i]->localTip.x, armList[i]->localTip.y, armList[i]->localTip.z);
			MjbSphere(1, 50, 50);
		}
		glPopMatrix();
	}
}