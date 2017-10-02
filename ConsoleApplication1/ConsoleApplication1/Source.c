#include<stdlib.h>
#include<math.h>
#include <GL/glut.h>
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<WinSock2.h>
#include <ws2tcpip.h>
#include<malloc.h>
#pragma comment(lib, "ws2_32.lib")


//////////// structs
struct Floor_colors {
	float red;
	float green;
	float blue;
};
struct Obstacles {
	int name;
	int floor;
	int type;
	int status;
	int pos_in_floor_options; // 0 = three options, 1= only right and left
	int pos_in_floor; // right, center, left
};
struct Floor {
	struct Floor_colors FloorColor;
	float velocity;
	int type;
	int obstacles_pos[2];
	struct Obstacles * obstacles[2];
};

//----------------------defines---------------------
#define M_PI 3.14159265358979323846
#define PATH_Z -1.5 
// jump defines
#define JUMP_LIMIT 2.5 //jump limit value
#define JUMPING_UP 1 //Jumping up status
#define JUMPING_DOWN 2 //Jumping down status
#define WALKING 0 //walking status

// turns defines
#define RIGHT 0
#define LEFT 1
#define STRAIGHT 2
#define OPPOSITE 3

// start/pause defines
#define GO 1
#define PAUSE 0

// floor defines
#define Floors_Num 3
#define All_FLOORS 4
#define BRIDGE 0
#define SOLID 1
#define PIPE 2
#define OBSTACLES_NUM 5
#define OBSTACLE_IN_RIGHT 0
#define OBSTACLE_IN_LEFT 1
#define OBSTACLE_IN_CENTER 2
#define OBSTACLE_JUMP 0
#define OBSTACLE_BEND 1
#define OBSTACLE_AVOID 2
#define OBSTACLE_RUN 1
#define OBSTACLE_STOP 0
#define OBSTACLE_POSITION_THREE_OPTIONS 0
#define OBSTACLE_POSITION_ONLY_RIGHT_AND_LEFT 1
#define OBSTACLE_POSITION_ONE_OPTION 2

#define NONE_OBSTACLE 0
#define DOWN_BLADE 1
#define UPPER_BLADE 2
#define MISSING_FLOOR 3
#define PUNCH 4

// junction defines
#define R_JUNCTION 0
#define L_JUNCTION 1
#define T_JUNCTION 2

//////// bend defines
#define BEND_DOWN 1
#define BEND_BACK_TO_NORMAL 2
#define STANDING 0
#define BEND_LIMIT 3

//////// player movement defines
#define LEFT_ARM_RIGHT_LEG 0
#define RIGHT_ARM_LEFT_LEG 1

/////// blade defines
#define BLADE_Start 2
#define BLADE_Stop 3
#define BLADE_UP 0
#define BLADE_DOWN 1
#define BLADE_START 0
#define BLADE_STOP 1

//spring defines
#define PUNCH_SQUEEZE 0
#define PUNCH_EXTEND 1
#define PUNCH_LIMIT 2

// macro for random a number
#define randnum(min, max) \
        ((rand() % (int)(((max) + 1) - (min))) + (min))
//----------------------variables declaration---------------------\\
// texture
FILE * file;
GLuint images_arr[10];
unsigned char *ground_picture, *left_picture, *front, *right_picture, *back, *sky, *wall;

//////// floor
struct Floor floors[Floors_Num];
struct Floor_colors FloorColor[Floors_Num];
struct Obstacles obstacles[OBSTACLES_NUM];
int Floor_Num = 0;
int temp_Floor_Num = 0;
float fx = 0.0, fy = 0.5, fz = 0; // floor x,y,z - to save the x,y for the player and draw from the current location
int RFloorFlag = 1, LFloorFlag = 1; // RF=right floor  LF=left floor
int Turnflag = 2; // 0-went right  1- went left  2-defualt, straight  3-opposite z
int Junction_Flag = T_JUNCTION;
int JunctionTurnFlag = 0; // 1=right on the junction,  0= left on the junction
int Jflag = 0; // 0-TJunction, 1-RJunction 2-LJunction

//////// blade obstacle
float Blade_rot = 0;
float Blade_y = 0;
float Blade_x = -1;
int Blade_flag = BLADE_UP;
int Blade_start_flag = BLADE_STOP;

////// bend
float bend_angle = 0;
int bend_status = STANDING;

//spring obstacle
float Punch_x=0;
int  Punch_flag = PUNCH_EXTEND;

//////// player setup
float legAngle[2] = { 0.0f, 0.0f };//left ,right angle
float armAngle[2] = { 0.0f, 0.0f };//left ,right angle
int Player_MovementStatus = LEFT_ARM_RIGHT_LEG; // animation movement flag
float tx = 0.0, ty = 0.5, tz = 0; // player x,y,z
int Jump_status = 0; // movement status 0-walking, 1-jump up, 2- jump down
int startflag = 0; // start flag  'f'=1=start 'p'=0=stop
int rot = 0; // rotation angle
float count = 0; //position in the floor
int Obstacle1_flag= 0; // to generate random number for the first obstacle in the floor
int Obstacle2_flag= 0;// to generate random number for the second obstacle in the floor
int OBSTACLE1_POSITION= 8;
int OBSTACLE2_POSITION= 20;
int Floor_length = 30;
int temp_velocity;
				 //----------------------function declaration---------------------\\
				 /////// player
void initRandom();
void player();
void draw();
void Idle();
void PlayerMovement(float v);
void Jump();
void bend();
void init();
void initTexture();
void initFloor();
void initFloorColor();

/////// floor declaration
void Check_if_crashed();// check if the player crashed on an obstacle
void initFloorColor();
void sidecubes(); // draw bridge side cubes
void sidecubes1(); // draw regular side cubes
void sidecubes2(); // draw pipeline side cubes
void Floor(); // draw bridge floor
void Floor1(); // draw regular floor
void Floor2(); // draw pipeline floor
void Pipe(); // draw pipe
void Select_Floor(int floor_pos);
void Blade(); //draw blade
void Blade_movement();
void Upper_Blade(int obstacle_num); // Pipe floor : obstacle 2
void Down_Blade(int obstacle_num);// Pipe floor : obstacle 1
void Punch_movement();
void Punch(int obstacle_num);// solid floor : obstacle 1
void Missing_floor(int obstacle_num);// Bridge floor : obstacle 1

//////junctions declaration
void JunctionL();
void JunctionT();
void JunctionR();
void randJunc(); // randomize junction

//////////////obstacles: 
void RandomObstacle(int ObstacleNumber);

////////floor
void RandomObstacle(int ObstacleNumber) {
	
	/// draw first obstacle
	if (ObstacleNumber == 0)
	{
		if (Floor_Num == PIPE) {
			switch (Obstacle1_flag)
			{
			case DOWN_BLADE: Down_Blade(0);
				break;
			case UPPER_BLADE: Upper_Blade(0);
				break;
			}
		}
		else if (Floor_Num == BRIDGE) {
			switch (Obstacle1_flag)
			{
			case MISSING_FLOOR: Missing_floor(0);
				break;
			}
		}
		else if (Floor_Num == SOLID) {
			switch (Obstacle1_flag)
			{
			case PUNCH: Punch(0);
				break;
			}
		}
	}
	else if (ObstacleNumber == 1) {
		if (Floor_Num == PIPE) {
			switch (Obstacle2_flag)
			{
			    case DOWN_BLADE: Down_Blade(1);
				     break;
			    case UPPER_BLADE: Upper_Blade(1);
				     break;
			}
		}
		else if (Floor_Num == BRIDGE) {
			switch (Obstacle2_flag)
			{
			case MISSING_FLOOR: Missing_floor(1);
				break;
			}
		}
		else if (Floor_Num == SOLID) {
			switch (Obstacle2_flag)
			{
			case PUNCH: Punch(1);
				break;
			}
		}
	}
	

}
void Select_Floor(int floor_pos) {
	if (floor_pos == 0)
	{
		switch (Floor_Num)
		{
		case BRIDGE: Floor();
			break;
		case SOLID: Floor1();
			break;
		case PIPE: Floor2();
			break;
		}
	}
	else {
		switch (temp_Floor_Num)
		{
		case BRIDGE: Floor();
			break;
		case SOLID: Floor1();
			break;
		case PIPE: Floor2();
			break;
		}
	}
}
void Floor() {
	int i;
	Blade_start_flag = BLADE_STOP;
	for (i = 0;i<(Floor_length / 1.5);i++)
	{
		// Floor
		if (i == (floors[BRIDGE].obstacles_pos[0] / 1.5))
		{
			RandomObstacle(0);
			if (floors[BRIDGE].obstacles[0]->name == MISSING_FLOOR)
			{
				glTranslatef(0.0, 0, PATH_Z);
				continue;
			}
		}
		else if (i == (floors[BRIDGE].obstacles_pos[1]/1.5 ))
		{
			RandomObstacle(1);
			if (floors[BRIDGE].obstacles[1]->name == MISSING_FLOOR)
			{
				glTranslatef(0.0, 0, PATH_Z);
				continue;
			}
		}

		glBegin(GL_QUADS);
		glColor3f(FloorColor[0].red, FloorColor[0].green, FloorColor[0].blue);
		glNormal3f(0, 1, 0);	// normal straight up
		glVertex3f(-2.5, 0, -1.0); //floor left up
		glVertex3f(2.5, 0, -1.0); // floor right up
		glVertex3f(2.5, 0, 0.0); // floor right down
		glVertex3f(-2.5, 0, 0.0); // floor left down
		glEnd();

		glTranslatef(2.4, 0, -0.5);
		sidecubes();
		glTranslatef(-4.8, 0, 0);
		sidecubes();
		glTranslatef(2.4, 0, 0.5);

		glTranslatef(0.0, 0, PATH_Z);
	}
	glTranslatef(0.0, 0, Floor_length);
}
void Pipe() {
	// Floor
	int ACC = 48;
	float a, p, q;
	int i;
	glBegin(GL_QUAD_STRIP);
	glColor3f(FloorColor[2].red, FloorColor[2].green, FloorColor[2].blue);
	for (i = 0;i <= ACC;i++)
	{
		a = i*(M_PI) * 2 / ACC;
		p = cos(a);
		q = sin(a);
		glNormal3f(0.4, q, p);
		glVertex3f(2.5, 0.4*q, 0.4*p);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(-2.5, 0.4*q, 0.4*p);
	}
	glEnd();
}
void Floor1() {
	// Floor
	Blade_start_flag = BLADE_STOP;
	glBegin(GL_QUADS);
	glColor3f(FloorColor[1].red, FloorColor[1].green, FloorColor[1].blue);
	glNormal3f(0, 1, 0);	// normal straight up
	glVertex3f(-2.5, 0, 0); //floor left down
	glVertex3f(2.5, 0, 0); // floor right down
	glVertex3f(2.5, 0, -Floor_length); // floor right up
	glVertex3f(-2.5, 0, -Floor_length); // floor left up
	glEnd();

	glTranslatef(2.25, 0, 0);
	sidecubes1();
	glTranslatef(-4.5, 0, 0);
	sidecubes1();
	glTranslatef(2.25, 0, 0);

	if (Floor_length == 45)
	{
		RandomObstacle(0);
		RandomObstacle(1);
	}
	else if (Floor_length == 30)
		RandomObstacle(0);
	
	
}
void Floor2() {
	int i;
	glTranslatef(0, -0.5, 0);
	for (i = 0;i<Floor_length;i++) {
		// Floor
		
		//first obstacle
		if (i == floors[Floor_Num].obstacles_pos[0]) 
			RandomObstacle(0);

		//seconds obstacle
		if (i == floors[Floor_Num].obstacles_pos[1])
			RandomObstacle(1);
		Pipe();
		glTranslatef(0.0, 0, -1.1);
	}
	glTranslatef(0, 0.5, (Floor_length * (1.1)));
}
void JunctionT()
{
	Select_Floor(0);

	glTranslatef(0, 0, -(Floor_length+2.5));
	glBegin(GL_QUADS);
	glColor3f(FloorColor[Floor_Num].red, FloorColor[Floor_Num].green, FloorColor[Floor_Num].blue);
	glNormal3f(0, 1, 0);	// normal straight up
	glVertex3f(-2.5, 0, -2.5); //floor left down
	glVertex3f(2.5, 0, -2.5); // floor right down
	glVertex3f(2.5, 0, 2.5); // floor right up
	glVertex3f(-2.5, 0, 2.5); // floor left up
	glEnd();

	glRotatef(-90, 0, 1, 0); // right floor
	glTranslatef(0, 0, -2.5);
	Select_Floor(1);
	glTranslatef(0, 0, 5);

	glRotatef(180, 0, 1, 0);// left floor
	Select_Floor(1);
	glTranslatef(0, 0, 2.5);
	glRotatef(-90, 0, 1, 0);
	JunctionTurnFlag = 1;
	Junction_Flag = T_JUNCTION;
}
void JunctionR()
{
	if (JunctionTurnFlag)
	{
		glRotatef(90, 0, 1, 0);
		JunctionTurnFlag = 0;
	}
	glRotatef(-90, 0, 1, 0);
	Select_Floor(0);
	glTranslatef(0, 0, -(Floor_length + 2.5));

	glBegin(GL_QUADS);
	glColor3f(FloorColor[Floor_Num].red, FloorColor[Floor_Num].green, FloorColor[Floor_Num].blue);
	glNormal3f(0, 1, 0);	// normal straight up
	glVertex3f(-2.5, 0, -2.5); //floor left down
	glVertex3f(2.5, 0, -2.5); // floor right down
	glVertex3f(2.5, 0, 2.5); // floor right up
	glVertex3f(-2.5, 0, 2.5); // floor left up
	glEnd();

	glRotatef(-90, 0, 1, 0);
	glTranslatef(0, 0, -2.5);
	Select_Floor(1);
	glTranslatef(0, 0, 2.5);
	glRotatef(90, 0, 1, 0);
	glTranslatef(0, 0, (Floor_length+2.5));
	glRotatef(90, 0, 1, 0);
	Junction_Flag = R_JUNCTION;

}
void JunctionL()
{
	if (JunctionTurnFlag)
	{
		glRotatef(-90, 0, 1, 0);
		//glTranslatef(0, 0, -32.5);
		JunctionTurnFlag = 0;
	}

	glRotatef(90, 0, 1, 0);
	Select_Floor(0);

	glTranslatef(0, 0, -(Floor_length+2.5));

	glBegin(GL_QUADS);
	glColor3f(FloorColor[Floor_Num].red, FloorColor[Floor_Num].green, FloorColor[Floor_Num].blue);
	glNormal3f(0, 1, 0);	// normal straight up
	glVertex3f(-2.5, 0, -2.5); //floor left down
	glVertex3f(2.5, 0, -2.5); // floor right down
	glVertex3f(2.5, 0, 2.5); // floor right up
	glVertex3f(-2.5, 0, 2.5); // floor left up
	glEnd();
	glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, -2.5);
			Select_Floor(1);
		glTranslatef(0, 0, 2.5);
	glRotatef(-90, 0, 1, 0);
	glTranslatef(0, 0, (Floor_length+2.5));
	glRotatef(-90, 0, 1, 0);
	Junction_Flag = L_JUNCTION;
}
void randJunc() {
	switch (Jflag)
	{
	case R_JUNCTION: {
		JunctionR(); // T Junction
		break;
	}
	case L_JUNCTION: {
		JunctionL(); // right turn only junction
		break;
	}
	case T_JUNCTION: {
		JunctionT(); // left turn only junction
		break;
	}
	}
}
void sidecubes()
{
	glBegin(GL_QUADS);
	// Front
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(-0.1, 0, 0.25); // left down
	glVertex3f(-0.1, 1.5, 0.25); // left up
	glVertex3f(0.1, 1.5, 0.25);//right up
	glVertex3f(0.1, 0, 0.25); // right down

							  // Back
	glNormal3f(0, 0, -1);
	glVertex3f(-0.1, 0, -0.25); //left down
	glVertex3f(0.1, 0, -0.25); //right down
	glVertex3f(0.1, 1.5, -0.25); // right up
	glVertex3f(-0.1, 1.5, -0.25); // left up

								  // Left side
	glNormal3f(-1, 0, 0);
	glVertex3f(-0.1, 0, 0.25);//front down
	glVertex3f(-0.1, 1.5, 0.25); //front up
	glVertex3f(-0.1, 1.5, -0.25);//back up
	glVertex3f(-0.1, 0, -0.25); //back down

								// Right side
	glNormal3f(1.0, 0, 0);
	glVertex3f(0.1, 0, 0.25); // front down
	glVertex3f(0.1, 0, -0.25); // back down
	glVertex3f(0.1, 1.5, -0.25); // back up
	glVertex3f(0.1, 1.5, 0.25); // front up
	glEnd();
}
void sidecubes1()
{
	glBegin(GL_QUADS);
	// Front
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(0.25, 0, 0); // left down
	glVertex3f(0.25, 1.5, 0); // left up
	glVertex3f(-0.25, 1.5, 0);//right up
	glVertex3f(-0.25, 0, 0.); // right down

							  // Back
	glNormal3f(0, 0, -1);
	glVertex3f(0.25, 0, 0); //left down
	glVertex3f(-0.25, 0, 0); //right down
	glVertex3f(-0.25, 1.5, 0); // right up
	glVertex3f(0.25, 1.5, 0); // left u

							  // Left side
	glNormal3f(0.25, 0, 0);
	glVertex3f(0.25, 0, -Floor_length);//front down
	glVertex3f(0.25, 1.5, -Floor_length); //front up
	glVertex3f(0.25, 1.5, 0);//back up
	glVertex3f(0.25, 0, 0); //back down

							// Right side
	glNormal3f(-0.25, 0, 0);
	glVertex3f(-0.25, 0, -Floor_length); // front down
	glVertex3f(-0.25, 0, 0); // back down
	glVertex3f(-0.25, 1.5, 0); // back up
	glVertex3f(-0.25, 1.5, -Floor_length); // front up

								 //upper side
	glNormal3f(0, 1, 0);
	glVertex3f(-0.25, 1.5, -Floor_length); // front right
	glVertex3f(0.25, 1.5, -Floor_length); // front left
	glVertex3f(0.25, 1.5, 0);// back right 
	glVertex3f(-0.25, 1.5, 0); // back left

	glEnd();
}
void sidecubes2()
{
	glBegin(GL_QUADS);
	// Front
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(0.25, 0, 0); // left down
	glVertex3f(0.25, 1.5, 0); // left up
	glVertex3f(-0.25, 1.5, 0);//right up
	glVertex3f(-0.25, 0, 0.); // right down

							  // Back
	glNormal3f(0, 0, -1);
	glVertex3f(0.25, 0, 0); //left down
	glVertex3f(-0.25, 0, 0); //right down
	glVertex3f(-0.25, 1.5, 0); // right up
	glVertex3f(0.25, 1.5, 0); // left u

							  // Left side
	glNormal3f(0.25, 0, 0);
	glVertex3f(0.25, 0, -Floor_length);//front down
	glVertex3f(0.25, 1.5, -Floor_length); //front up
	glVertex3f(0.25, 1.5, 0);//back up
	glVertex3f(0.25, 0, 0); //back down

							// Right side
	glNormal3f(-0.25, 0, 0);
	glVertex3f(-0.25, 0, -Floor_length); // front down
	glVertex3f(-0.25, 0, 0); // back down
	glVertex3f(-0.25, 1.5, 0); // back up
	glVertex3f(-0.25, 1.5, -Floor_length); // front up

								 //upper side
	glNormal3f(0, 1, 0);
	glVertex3f(-0.25, 1.5, -Floor_length); // front right
	glVertex3f(0.25, 1.5, -Floor_length); // front left
	glVertex3f(0.25, 1.5, 0);// back right 
	glVertex3f(-0.25, 1.5, 0); // back left

	glEnd();
}
void Blade() {
	glScalef(0.8,0.8,0.8);
	double a, x, y;
	glColor3f(1, 1, 1);
	glBegin(GL_POLYGON);
	for (int i = 0;i <= 720;i++)
	{
		a = M_PI * i;
		x = cos(a / 360);
		y = sin(a / 360);
		glVertex3f(x, y, 0);
	}
	glEnd();
	double Xc = 0, Yc = -0.1, z = 0.2;
	a = 0;
	Xc = cos(a / 360);
	Yc = sin(a / 360);
	glBegin(GL_TRIANGLES);
	for (int i = 20;i <= 720;i += 20)
	{
		a = M_PI * i;
		x = cos(a / 360);
		y = sin(a / 360);
		glVertex3f(x, y, 0);
		glVertex3f(Xc, Yc, 0);

		if (i <= 180) {
			glVertex3f((x + Xc) / 2 + z, (y + Yc) / 2 + 0.2 - z, 0);
			z -= 0.0222222222222222222222;
		}
		else if (i > 180 && i <= 360)
		{
			glVertex3f((x + Xc) / 2 - z, (y + Yc) / 2 + 0.2 - z, 0);
			z += 0.0222222222222222222222;
		}
		else if (i > 360 && i <= 540)
		{
			glVertex3f((x + Xc) / 2 - z, (y + Yc) / 2 - 0.2 + z, 0);
			z -= 0.0222222222222222222222;
		}
		else {
			glVertex3f((x + Xc) / 2 + z, (y + Yc) / 2 - 0.2 + z, 0);
			z += 0.0222222222222222222222;
		}
		Yc = y;
		Xc = x;
	}
	glEnd();
	Blade_start_flag = BLADE_START;
	glScalef(1.25, 1.25, 1.25);
}
void Blade_movement() {
	
	Blade_rot += 0.8;
	if (Blade_rot >= 360)
		Blade_rot = 0;

	////blade up and down
	if (Blade_flag == BLADE_UP)
	{
		Blade_x -= 0.02;
		Blade_y += 0.005;
		if (Blade_y >= 0.5)	Blade_flag = BLADE_DOWN;
	}
	else if(Blade_flag == BLADE_DOWN){
		
		Blade_x += 0.02;
		Blade_y -= 0.005;
		if(Blade_y <= 0) Blade_flag = BLADE_UP;
	}

}
void Upper_Blade(int obstacle_num)
{
	if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_RIGHT ) // translate to right
		glTranslatef(2.5, 0, 0);
	else if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_LEFT)// translate to left
	{
		glTranslatef(-2.5, 0, 0);
		glRotatef(180, 0, 1, 0);
	}
	glScalef(1, 2, 1);
		sidecubes();
	glScalef(1, 0.5, 1);

	glTranslatef(0, (2 * 1.5) , 0);
		glRotatef(90,0,0,1);
			glPushMatrix();
			glScalef(1, 2, 1);
				sidecubes();
			glPopMatrix();
		glRotatef(-90, 0, 0, 1);
	glTranslatef(Blade_x,0,0);
		glRotatef(Blade_rot, 0, 0, 1);
			glPushMatrix();
			glScalef(0.8, 0.8, 0.8);
				Blade();
			glPopMatrix();
		glRotatef(-Blade_rot, 0, 0, 1);
	glTranslatef(-Blade_x, -(2 * 1.5), 0);

	if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_RIGHT) // retranslate to center
		glTranslatef(-2.5, 0, 0);
	else if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_LEFT) //retranslate to center
	{
		glRotatef(-180, 0, 1, 0);
		glTranslatef(2.5, 0, 0);
	}
}
void Down_Blade(int obstacle_num) {
	if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_RIGHT) // translate to right
		glTranslatef(1.2, 0, 0);
	else if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_LEFT)// translate to left
		glTranslatef(-1.2, 0, 0);
	glTranslatef(0, Blade_y, 0);
	glRotatef(Blade_rot, 0, 0, 1);
		Blade();
	glRotatef(-Blade_rot, 0, 0, 1);
	glTranslatef(0, -Blade_y, 0);
	if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_RIGHT) // retranslate to center
		glTranslatef(-1.2, 0, 0);
	else if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_LEFT) //retranslate to center
		glTranslatef(1.2, 0, 0);
}
void Missing_floor(int obstacle_num) {
	if (floors[BRIDGE].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_RIGHT) // translate to right
		glTranslatef(2.5/2, 0, 0);
	else if (floors[BRIDGE].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_LEFT)// translate to left
		glTranslatef(-(2.5/2), 0, 0);

	glBegin(GL_QUADS);
	glColor3f(FloorColor[0].red, FloorColor[0].green, FloorColor[0].blue);
	glNormal3f(0, 1, 0);	// normal straight up
	glVertex3f(-0.6, 0, -1.5); //floor left up
	glVertex3f(0.6, 0, -1.5); // floor right up
	glVertex3f(0.6, 0, 0.5); // floor right down
	glVertex3f(-0.6, 0, 0.5); // floor left down
	glEnd();

	if (floors[BRIDGE].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_RIGHT) // translate to right
		glTranslatef(-2.5 / 2, 0, 0);
	else if (floors[BRIDGE].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_LEFT)// translate to left
		glTranslatef((2.5 / 2), 0, 0);
}
void Punch_movement() {

	////blade up and down
	if (Punch_flag == PUNCH_EXTEND)
	{
		if (Punch_x <= -0.0008)
			Punch_flag = PUNCH_SQUEEZE;
		Punch_x -= 0.000015;
	}
	else if (Punch_flag == PUNCH_SQUEEZE) {
		Punch_x += 0.000015;
		if (Punch_x >= 0)
			Punch_flag = PUNCH_EXTEND;
	}
}
void Punch(int obstacle_num)
{
	if(floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_RIGHT) // translate to right
		glTranslatef(1.85, 1, -(floors[Floor_Num].obstacles_pos[obstacle_num]));
	else if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_LEFT)// translate to left
	{
		glTranslatef(-1.85, 1, -(floors[Floor_Num].obstacles_pos[obstacle_num]));
		glRotatef(180,0,1,0);
	}

	glPushMatrix();
	glScalef(0.4, 0.4, 0.4);
	glBegin(GL_QUADS);
	// Front
	glColor3f(0.5, 0.5, 0.5);
	glNormal3f(0, 1, 0);
	glVertex3f(-1, 1, 1); // left down
	glVertex3f(-1, 1, -1); // left up
	glVertex3f(1, 1, -1); //right up
	glVertex3f(1, 1, 1); // right down

							  // Back
	glNormal3f(0, 0, 1);
	glVertex3f(-1, 1, 1); // left down
	glVertex3f(1, 1, 1); // left up
	glVertex3f(1, -1, 1);//right up
	glVertex3f(-1, -1, 1); // right down

								  // Left side
	glNormal3f(0, 0, -1);
	glVertex3f(-1, 1, -1); // left down
	glVertex3f(1, 1, -1); // left up
	glVertex3f(1, -1, -1);//right up
	glVertex3f(-1, -1, -1); // right down

								// Right side
	glNormal3f(0, -1, 0);
	glVertex3f(-1, -1, 1); // left down
	glVertex3f(-1, -1, -1); // left up
	glVertex3f(1, -1, 1);//right up
	glVertex3f(1, -1, -1); // right down

	glNormal3f(1, 0, 0);
	glVertex3f(1, -1, 1); // left down
	glVertex3f(1, -1, -1); // left up
	glVertex3f(1, 1, -1);//right up
	glVertex3f(1, 1, 1); // right down

	glEnd();
	
	double a, x=0, y, z;
	glColor3f(1, 1, 1);
	for (int j = 0;j < 5;j++)
	{
		glBegin(GL_POLYGON);
			for (int i = 0;i <= 720;i++)
			{
				a = M_PI * i;
				z = cos(a / 360);
				y = sin(a / 360);
				x += Punch_x;
				glVertex3f(x, y, z);
			}
		glEnd();

	}
	glTranslatef((720 * (Punch_x) * 5) -0.8, 0, 0);
	glutSolidSphere(1, 32, 32);
	glTranslatef(-((720 * (Punch_x) * 5) - 0.8), 0, 0);

	glPopMatrix();

	if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_RIGHT) // translate to right
		glTranslatef(-1.85, -1, floors[Floor_Num].obstacles_pos[obstacle_num]);
	else if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_LEFT)// translate to left
	{
		glRotatef(-180, 0, 1, 0);
		glTranslatef(1.85, -1, floors[Floor_Num].obstacles_pos[obstacle_num]);
	}
}
void Check_if_crashed() {
	for (int i = 0 ; i < 2 ; i++)
	{
		switch (floors[Floor_Num].obstacles[i]->name) // check first obstacle
		{
			case DOWN_BLADE: { // down blade
				if ((ty < Blade_y + 1) && ((count >= (floors[Floor_Num].obstacles_pos[i] * 1.1) - 0.1 && count <= (floors[Floor_Num].obstacles_pos[i] * 1.1) + 0.1)))
				{
					if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_RIGHT) && (tx >= 1.1 && tx <= 1.3))
						exit(1);
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_LEFT) && (tx <= -1.1 && tx >= -1.3))
						exit(1);
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_CENTER) && (tx >= -0.5 && tx <= 0.5))
						exit(1);
					}
				break;
				}
			case UPPER_BLADE:  // upper blade
			{
				if (ty >= 0.4 && ((count >= (floors[Floor_Num].obstacles_pos[i] * 1.1) - 0.1 && count <= (floors[Floor_Num].obstacles_pos[i] * 1.1) + 0.1)))
				{
					if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_RIGHT) && (tx >= -1))
						exit(1);
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_LEFT) && (tx <= 1))
						exit(1);
				}
			break;
			}
			case MISSING_FLOOR:  // MISSING_FLOOR
			{
				if (ty <= 0.6 && ((count >= floors[Floor_Num].obstacles_pos[i]-0.1 && count <= floors[Floor_Num].obstacles_pos[i] + 1)))
				{
					if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_RIGHT) && (tx <= 1))
						exit(1);
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_LEFT) && (tx >= -1))
						exit(1);
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_CENTER) && (tx < -0.5 || tx > 0.5))
						exit(1);
				}
				break;
			}
			case PUNCH:  // PUNCH
			{
				if (((count >= floors[Floor_Num].obstacles_pos[i] - 0.5 && count <= floors[Floor_Num].obstacles_pos[i] + 1)))
				{
					if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_RIGHT) && (tx >= 2.25 + ((720 * (Punch_x) * 5)))&& Punch_flag == PUNCH_EXTEND)
						exit(1);
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_LEFT) && (tx <= -2.25 - ((720 * (Punch_x) * 5)))&& Punch_flag == PUNCH_EXTEND)
						exit(1);
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_RIGHT) && (tx >= ((720 * (Punch_x) * 5))) && Punch_flag == PUNCH_SQUEEZE && tx == 0)
						tx -= 1.2;
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_LEFT) && (tx <= (720 * (Punch_x) * 5)) && Punch_flag == PUNCH_SQUEEZE  && tx == 0)
						tx += 1.2;
				}
				break;
			}
		}
	}
}

////// player
void Bridge_robe()
{
	glPushMatrix();
	glColor3f(1, 1, 1);
	double a, x, y;
	glColor3f(1, 1, 1);
	glBegin(GL_POLYGON);
	for (int i = 0;i <= 360;i++)
	{
		a = M_PI * i;
		x = cos(a / 360);
		y = sin(a / 360);
		glVertex3f(x, y, 0);
	}
	for (int i = 360;i > 0;i--)
	{
		a = M_PI * i;
		x = cos(a / 360);
		y = sin(a / 360);
		glVertex3f(x, y, 0);
	}
	//glVertex3f(cos(0), sin(0), 0);
	glEnd();

	glPopMatrix();
}
void bend(){
	if (bend_status == BEND_DOWN)
	{
		if (bend_angle >= 80)
			bend_status = BEND_BACK_TO_NORMAL;
		bend_angle += 1.5;
		ty -= 0.012;
	}
	else if (bend_status == BEND_BACK_TO_NORMAL)
	{
		if (bend_angle <= 0)
		{
			bend_status = STANDING;
			Player_MovementStatus = LEFT_ARM_RIGHT_LEG;
		}
		bend_angle -= 2;
		ty += 0.016;
	}
}
void Jump() {
	if (Jump_status == JUMPING_UP)
	{
		if (ty >= JUMP_LIMIT)
			Jump_status = JUMPING_DOWN;
		//ty += 0.03;
		ty += (floors[Floor_Num].velocity/1.3);
	}
	else if (Jump_status == JUMPING_DOWN)
	{
		if (ty <= 0.5)
		{
			Jump_status = WALKING;
			ty = 0.5;
		}
		//ty -= 0.03;
		ty -= (floors[Floor_Num].velocity/1.8);
	}
}
void DrawArm(float xPos, float yPos, float zPos)
{
	glPushMatrix();

	/*      Sets color to red*/
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(xPos, yPos, zPos);

	glColor3f(1, 1, 1);
	glTranslatef(0, 0, 0);
	glScalef(0.5, 1.65, 0.5);
	glutSolidSphere(1, 32, 32);

	glPopMatrix();
}
void DrawHead(float xPos, float yPos, float zPos)
{
	glPushMatrix();

	/*   Sets color to white*/
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(xPos, yPos, zPos);
	glutSolidSphere(1, 32, 32);

	//hat
	glTranslatef(0, 0.5, 0);
	glScalef(0.75, 1.2, 0.75);
	glColor3f(1.0f, 1.0f, 0.0f);
	glutSolidSphere(1, 32, 32);

	glTranslatef(0, 0.1, 0);
	glScalef(2, 0.1, 2);
	glColor3f(1.0f, 1.0f, 0.0f);
	glutSolidSphere(1, 32, 32);

	glPopMatrix();
}
void DrawTorso(float xPos, float yPos, float zPos)
{
	glPushMatrix();

	/*      Sets color to blue*/
	glColor3f(0.0f, 0.0f, 1.0f);
	glTranslatef(xPos, yPos, zPos);

	glColor3f(1, 1, 1);
	glScalef(1.5, 2.3, 1.5);
	glutSolidSphere(1, 32, 32);

	glPopMatrix();
}
void DrawLeg(float xPos, float yPos, float zPos)
{
	glPushMatrix();

	/*      Sets color to yellow*/
	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(xPos, yPos, zPos);

	glColor3f(1, 1, 1);
	glScalef(0.5, 1.75, 0.5);
	glutSolidSphere(1, 32, 32);


	glPopMatrix();
}
void Bodymovement()
{
	if (armAngle[0] >= 30) // left hand
		Player_MovementStatus = RIGHT_ARM_LEFT_LEG;
	else if (armAngle[1] >= 30) // right hand
		Player_MovementStatus = LEFT_ARM_RIGHT_LEG;
	if (Player_MovementStatus == LEFT_ARM_RIGHT_LEG)
	{
		armAngle[0] += 1; // increase left arm angle 
		armAngle[1] -= 1; // decrease right arm angle

		legAngle[0] += 1;
		legAngle[1] -= 1;
	}
	if (Player_MovementStatus == RIGHT_ARM_LEFT_LEG)
	{
		armAngle[1] += 1; // increase right arm angle 
		armAngle[0] -= 1; // decrease left arm angle

		legAngle[0] -= 1;
		legAngle[1] += 1;
	}
}
void player()
{
	glScalef(0.35, 0.35, 0.35);
	DrawTorso(0, 0, 0);
	DrawHead(0, 3.5, 0);

	glRotatef(armAngle[0], 1, 0, 0); // rotate left arm
	DrawArm(-2, 0, 0);
	DrawLeg(1, -4, 0);
	glRotatef(-armAngle[0], 1, 0, 0); // rotate left arm

	glRotatef(armAngle[1], 1, 0, 0); // rotate left arm
	DrawLeg(-1, -4, 0);
	DrawArm(2, 0, 0);
	glRotatef(-armAngle[1], 1, 0, 0); // rotate right arm
	glScalef(7, 7, 7);
}
void keyboard(unsigned char key, int x, int y)
{
	if (key == 27) exit(1);
	if (key == 'f')
		startflag = 1;
	if (key == 'p')
		startflag = 0;
	if (key == 'r')
	{
		rot += 90;
		rot %= 360;
	}
	if (key == 'l')
	{
		rot -= 90;
		rot %= 360;
	}
	if (key == '8')
		if (Jump_status == WALKING)
			Jump_status = JUMPING_UP;
	if (key == '4') { //turn left key
		if (count < (Floor_length - 0.5)) {
			if (LFloorFlag > 0) {
				tx -= 1.2;
				LFloorFlag--;
				RFloorFlag++;
			}
		}
		else
		{
			if (Jflag == R_JUNCTION)
				exit(1);
			tz -= ((Floor_length + 2.5) - count);
			fz = tz;
			count = 0;
			initRandom();
			rot = -90;
		//	initTexture();
			if (Jflag == R_JUNCTION)
				rot = 90;
			if (Jflag == T_JUNCTION)
				rot = 0;
		}
	}
	if (key == '6') { // turn right key
		if (count < (Floor_length - 2.5)) {
			if (RFloorFlag > 0) {
				tx += 1.2;
				RFloorFlag--;
				LFloorFlag++;
			}
		}
		else
		{
			if (Jflag == L_JUNCTION)
				exit(1);
			tz -= ((Floor_length+2.5) - count);
			fz = tz;
			count = 0;
			initRandom();
			rot = 90;
			//initTexture();
			if (Jflag == L_JUNCTION)
				rot = -90;
			if (Jflag == T_JUNCTION)
				rot = 0;
		}
	}
	if (key == '2') {
		bend_status = BEND_DOWN;
		Player_MovementStatus = STANDING;
	}
	glutPostRedisplay();
}
void draw()
{
//	printf("%f \n", ((Punch_x) * 5));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0, -5, -10); //initial situation
	
	glTranslatef(-tx, -ty, -tz); //movement in the world

	glTranslatef(tx, ty + 0.8, tz - 1);//player control
	glRotatef(bend_angle, 1, 0, 0); // player bend
	player();
	glRotatef(-bend_angle, 1, 0, 0);
	glTranslatef(-tx, (-ty) - 0.8, -tz);

	glTranslatef(0, 0, fz); // new junction by the existing junction x,z
	glRotatef(rot, 0, 1, 0); // junction rotation
	randJunc();
	glRotatef(-rot, 0, 1, 0);
	glTranslatef(0, 0, -fz);

	if (count > (Floor_length+4.7)) // lost
		exit(1);
	glColor3f(1.0,1.0,1.0);
	glutSwapBuffers();			// display the output
}
void PlayerMovement(float v)
{
	Check_if_crashed();
	Jump();
	bend();
	Bodymovement();
	tz -= v;
	count += v;
}
void Idle()
{
	if (startflag == 1)
		if(Floor_length == 15)
			PlayerMovement(floors[Floor_Num].velocity * 0.5);
		else if (Floor_length == 30)
			PlayerMovement(floors[Floor_Num].velocity * 0.75);
		else if (Floor_length == 45)
			PlayerMovement(floors[Floor_Num].velocity);
	
	if(Blade_start_flag == BLADE_START)
		Blade_movement();
	Punch_movement();
	draw();
}

// Set OpenGL parameters
void initTexture() {
	//makeskypic
	//make ground pic
	glGenTextures(1, images_arr);
	glBindTexture(GL_TEXTURE_2D, images_arr);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, images_arr[0]);
	glEnable(GL_TEXTURE_2D);
	sky = (unsigned char*)malloc(256 * 256 * 3);
	file = fopen("sky.bmp", "rb");
	fread(ground_picture, 256 * 256 * 3, 1, file);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 256, 256, GL_BGR_EXT, GL_UNSIGNED_BYTE, ground_picture);
	fclose(file);
	glBegin(GL_QUADS);
	glTexCoord3d(0.0, 0.0, 0.0); glVertex3d(-30.0, 0.0, -30.0);
	glTexCoord3d(1.0, 0.0, 0.0); glVertex3d(30.0, 0.0, -30.0);
	glTexCoord3d(1.0, 1.0, 1.0); glVertex3d(30.0, 0.0, 30.0);
	glTexCoord3d(0.0, 1.0, 1.0); glVertex3d(-30.0, 0.0, 30.0);
	glEnd();

	free(sky);
}
void init()
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1.0, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);

	// Lighting parameters

	GLfloat mat_ambdif[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 0.0, 1.0, 0.0, 0.0 };
	GLfloat mat_shininess[] = { 80.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	glClearColor(0.0, 0.0, 0.0, 0.0);


	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambdif);	// set both amb and diff components
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);		// set specular
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);		// set shininess
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);		// set light "position", in this case direction
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);		// active material changes by glColor3f(..)

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_DEPTH_TEST);
	
	initFloor();
}
void initFloorColor()
{
	FloorColor[0].red = 0.2;
	FloorColor[0].green = 0.7;
	FloorColor[0].blue = 0.3;

	FloorColor[1].red = 1;
	FloorColor[1].green = 1;
	FloorColor[1].blue = 1;

	FloorColor[2].red = 0.804;
	FloorColor[2].green = 0.522;
	FloorColor[2].blue = 0.247;
}
void initRandom()
{
	time_t t;
	srand((unsigned)time(&t));

	////generate floor length
	if (Floor_length != 15)
	{
		switch (randnum(0, 2))
		{
		case 0: Floor_length = 30;
			break;
		case 1: Floor_length = 45;
			break;
		case 2: Floor_length = 15;
			break;
		}
	}
	else
	{
		switch (randnum(0, 1))
		{
		case 0: Floor_length = 30;
			break;
		case 1: Floor_length = 45;
			break;
		}
	}
	
	////generate random floor
	Floor_Num = temp_Floor_Num;
	temp_Floor_Num = randnum(0,2);
	//Floor_Num = SOLID;

	///// Generate random junction
	Jflag = randnum(0,2);
	if (Jflag == R_JUNCTION)
		rot = 90;
	else if (Jflag == L_JUNCTION)
		rot = -90;
	else
		rot = 0;

	////// generate random obstacle
	if (Floor_length == 15)
	{
		Obstacle1_flag = NONE_OBSTACLE;
		Obstacle2_flag = NONE_OBSTACLE;
		floors[Floor_Num].obstacles[0] = &obstacles[Obstacle1_flag];
		floors[Floor_Num].obstacles[1] = &obstacles[Obstacle2_flag];

	}
	else if (Floor_length == 30)
	{
		if (Floor_Num == PIPE)
			Obstacle1_flag = randnum(1, 2);
		
		else if (Floor_Num == BRIDGE)
			Obstacle1_flag = randnum(3, 3);
			
		else if (Floor_Num == SOLID)
			Obstacle1_flag = randnum(4, 4);
		
		
		floors[Floor_Num].obstacles[0] = &obstacles[Obstacle1_flag];
		floors[Floor_Num].obstacles_pos[0] = Floor_length / 2; // place in the half
		Obstacle2_flag = NONE_OBSTACLE;
		floors[Floor_Num].obstacles[1] = &obstacles[Obstacle2_flag];
	}
	else if (Floor_length == 45)
	{
		floors[Floor_Num].obstacles_pos[0] = Floor_length / 3; // place in the third of the floor
		floors[Floor_Num].obstacles_pos[1] = (Floor_length / 3) * 2;// place in the two third of the floor
		if (Floor_Num == PIPE)
		{
			Obstacle1_flag = randnum(1, 2);
			Obstacle2_flag = randnum(1, 2);
		}
		else if (Floor_Num == BRIDGE)
		{
			Obstacle1_flag = randnum(3, 3);
			Obstacle2_flag = randnum(3, 3);
		}
		else if (Floor_Num == SOLID)
		{
			Obstacle1_flag = randnum(4, 4);
			Obstacle2_flag = randnum(4, 4);
		}
		floors[Floor_Num].obstacles[0] = &obstacles[Obstacle1_flag];
		floors[Floor_Num].obstacles[1] = &obstacles[Obstacle2_flag];
	}
	////generate random FIRST obstacle pos in floor
	for (int i = 0; i < 2;i++)
	{
		if (floors[Floor_Num].obstacles[i]->pos_in_floor_options == OBSTACLE_POSITION_ONLY_RIGHT_AND_LEFT)
		{
			switch (randnum(0,1))
			{
			case 0:floors[Floor_Num].obstacles[i]->pos_in_floor = OBSTACLE_IN_RIGHT;
				break;
			case 1: floors[Floor_Num].obstacles[i]->pos_in_floor = OBSTACLE_IN_LEFT;
				break;
			}
		}
		else if (floors[Floor_Num].obstacles[i]->pos_in_floor_options == OBSTACLE_POSITION_THREE_OPTIONS)
		{
			switch (randnum(0, 2))
			{
			case 0:floors[Floor_Num].obstacles[i]->pos_in_floor = OBSTACLE_IN_RIGHT;
				break;
			case 1: floors[Floor_Num].obstacles[i]->pos_in_floor = OBSTACLE_IN_LEFT;
				break;
			case 2: floors[Floor_Num].obstacles[i]->pos_in_floor = OBSTACLE_IN_CENTER;
				break;
			}
		}
		else if (floors[Floor_Num].obstacles[i]->pos_in_floor_options == OBSTACLE_POSITION_ONE_OPTION)
			floors[Floor_Num].obstacles[i]->pos_in_floor = OBSTACLE_IN_CENTER;
	}
	
	floors[BRIDGE].velocity += 0.01;
	floors[SOLID].velocity += 0.01;
	floors[PIPE].velocity += 0.01;
	
	glutPostRedisplay();
} 
void initObstacles() {
	obstacles[0].name = NONE_OBSTACLE;
	obstacles[0].floor = NONE_OBSTACLE;
	obstacles[0].type = NONE_OBSTACLE;
	obstacles[0].status = OBSTACLE_STOP;
	obstacles[0].pos_in_floor_options = OBSTACLE_POSITION_THREE_OPTIONS;

	obstacles[1].name= DOWN_BLADE;
	obstacles[1].floor = PIPE;
	obstacles[1].type = OBSTACLE_JUMP;
	obstacles[1].status = OBSTACLE_STOP;
	obstacles[1].pos_in_floor_options = OBSTACLE_POSITION_THREE_OPTIONS;

	obstacles[2].name = UPPER_BLADE;
	obstacles[2].floor = PIPE;
	obstacles[2].type = OBSTACLE_BEND;
	obstacles[2].status = OBSTACLE_STOP;
	obstacles[2].pos_in_floor_options = OBSTACLE_POSITION_ONLY_RIGHT_AND_LEFT;

	obstacles[3].name = MISSING_FLOOR;
	obstacles[3].floor = BRIDGE;
	obstacles[3].type = OBSTACLE_AVOID;
	obstacles[3].status = OBSTACLE_STOP;
	obstacles[3].pos_in_floor_options = OBSTACLE_POSITION_THREE_OPTIONS;

	obstacles[4].name = PUNCH;
	obstacles[4].floor = SOLID;
	obstacles[4].type = OBSTACLE_AVOID;
	obstacles[4].status = OBSTACLE_STOP;
	obstacles[4].pos_in_floor_options = OBSTACLE_POSITION_ONLY_RIGHT_AND_LEFT;
}
void initFloor()
{
	initFloorColor();
	initObstacles();

	floors[0].FloorColor.red = FloorColor[0].red;
	floors[0].FloorColor.green = FloorColor[0].green;
	floors[0].FloorColor.blue = FloorColor[0].blue;
	floors[0].type = BRIDGE;
	floors[0].velocity = 0.06;
	floors[0].obstacles_pos[0] = OBSTACLE1_POSITION;
	floors[0].obstacles_pos[1] = OBSTACLE2_POSITION;

	floors[1].FloorColor.red = FloorColor[1].red;
	floors[1].FloorColor.green = FloorColor[1].green;
	floors[1].FloorColor.blue = FloorColor[1].blue;
	floors[1].type = SOLID;
	floors[1].velocity = 0.08;
	floors[1].obstacles_pos[0] = OBSTACLE1_POSITION;
	floors[1].obstacles_pos[1] = OBSTACLE2_POSITION;

	floors[2].FloorColor.red = FloorColor[2].red;
	floors[2].FloorColor.green = FloorColor[2].green;
	floors[2].FloorColor.blue = FloorColor[2].blue;
	floors[2].type = PIPE;
	floors[2].velocity = 0.08;
	floors[2].obstacles_pos[0] = OBSTACLE1_POSITION * 1.1;
	floors[2].obstacles_pos[1] = OBSTACLE2_POSITION * 1.1;
}
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);	// RGB display, double-buffered, with Z-Buffer
	glutInitWindowSize(500, 500);					// 500 x 500 pixels
	glutCreateWindow("3D");
	init();
	initRandom();
	glutDisplayFunc(draw);						// Set the display function
	glutKeyboardFunc(keyboard);					// Set the keyboard function
	glutIdleFunc(Idle);

	glutMainLoop();							// Start the main event loop
}