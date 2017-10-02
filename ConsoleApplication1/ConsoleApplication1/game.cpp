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
	int pos_in_floor_options; // 0 = three options, 1= only right and left
	int pos_in_floor; // right, center, left
	int Obstacle_Hit; // if there was hit, the player loses
	int GameOver;
};
struct Floor {
	struct Floor_colors FloorColor;
	float velocity;
	int type;
	int obstacles_pos[2];
	struct Obstacles * obstacles[2];
};

struct Coins {
	int name;
	int status;
	int coin_pos[2];
	int quantity;
	int *pos_in_floor;
};

struct Player {
	int name;
	int status;
	float x;
	float y;
	float z;
};
//----------------------defines---------------------

#define M_PI 3.14159265358979323846
#define PATH_Z -1.5 

//menu defines
#define PLAY 0
#define MENU 1
#define HIGHSCORE 2
#define OPTIONS 3

//option defines
#define SELECT_PLAYER 0
#define SELECT_COINS 1
#define DEFAULT_OPTIONS 2

//players
#define JACK 0
#define ROBOT 1
#define PLAYER_ON 1
#define PLAYER_OFF 0

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
#define HIT 2 
#define GAME_FINISHED_NO_MENU 3

#define COIN_ON 0
#define COIN_OFF 1

// floor defines
#define Floors_Num 3
#define All_FLOORS 4
#define BRIDGE 0
#define SOLID 1
#define PIPE 2
#define OBSTACLES_NUM 6
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
#define OBSTACLE_HIT 0
#define OBSTACLE_MISS 1
#define GAME_OVER_ON 0
#define GAME_OVER_OFF 1

#define NONE_OBSTACLE 0
#define DOWN_BLADE 1
#define UPPER_BLADE 2
#define MISSING_FLOOR 3
#define AIRPLANE_CRASH 4
#define PUNCH 5

#define COINS_IN_RIGHT 0
#define COINS_IN_LEFT 1
#define COINS_IN_CENTER 2
#define RED_COINS 0
#define GREEN_COINS 1
#define BLUE_COINS 2
#define YELLOW_COINS 3

#define BONUS_COINS 0
#define SKATEBOARD_COIN 1
#define SKIP_OBSTACLE_COIN 2

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

//baloon defines
#define BALOON_SQUEEZE 0
#define BALOON_EXTEND 1
#define BALOON_LIMIT 2

//skateboard defines
#define SKATEBOARD_UP 0
#define SKATEBOARD_DOWN 1

//SKIP OBSTACLE COIN DEFINES
#define SKIPOBSTACLE_OFF 0
#define SKIPOBSTACLE_ON 1

//Skateboard COIN DEFINES
#define SKATEBOARD_OFF 0
#define SKATEBOARD_ON 1

// macro for random a number
#define randnum(min, max) \
        ((rand() % (int)(((max) + 1) - (min))) + (min))
//----------------------variables declaration---------------------//

int menu_flag = MENU;
int options_flag = DEFAULT_OPTIONS;
int select_player_flag = JACK;
int select_coins_flag = YELLOW_COINS;
int playing_menu_flag = GO;
int game_started_flag = 0;

//timer
time_t t1;
time_t t2;
int temp_time = 0;
char timer_str[20] =" ";
int min;
int sec;

//score
int score=0;
char score_str[100];

int new_score_position = 0;///????
int life = 2; 
char life_str[10];
//files
FILE *highscore_file;

// texture
FILE *file;
GLuint images_arr[10];
unsigned char *brick, *sky;

//////// floor
struct Floor floors[Floors_Num];
struct Floor_colors FloorColor[Floors_Num];
struct Obstacles obstacles[OBSTACLES_NUM];
struct Coins coins[3];
struct Player players[2];

int Floor_Num = 0;
int temp_Floor_Num = 0;
float fx = 0.0, fy = 0.5, fz = 0; // floor x,y,z - to save the x,y for the player and draw from the current location
int RFloorFlag = 1, LFloorFlag = 1; // RF=right floor  LF=left floor
int Turnflag = 2; // 0-went right  1- went left  2-defualt, straight  3-opposite z
int Junction_Flag = T_JUNCTION;
int JunctionTurnFlag = 0; // 1=right on the junction,  0= left on the junction
int Jflag = 0; // 0-TJunction, 1-RJunction 2-LJunction
int check_junction_flag = OBSTACLE_MISS;
int junction_game_over = GAME_OVER_OFF;
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
float Punch_x = 0;
int  Punch_flag = PUNCH_EXTEND;

int  AirPlaneCrash_flag = BALOON_EXTEND;

//////// player setup

float legAngle[2] = { 0.0f, 0.0f };//left ,right angle
float armAngle[2] = { 0.0f, 0.0f };//left ,right angle
int Player_MovementStatus = LEFT_ARM_RIGHT_LEG; // animation movement flag
int selected_player = JACK;
int Jump_status = 0; // movement status 0-walking, 1-jump up, 2- jump down
int startflag = 0; // start flag  'f'=1=start 'p'=0=stop
int rot = 0; // rotation angle
float floor_count = 0; //position in the floor
int Obstacle1_flag = 0; // to generate random number for the first obstacle in the floor
int Obstacle2_flag = 0;// to generate random number for the second obstacle in the floor
int OBSTACLE1_POSITION = 8;
int OBSTACLE2_POSITION = 20;
int Floor_length = 30;
int temp_velocity;

//bouns setups
int SkateBoard_flag = SKATEBOARD_OFF; // skateboard status flag
float SkateBoard_y = 3; // skateboard height
int skateboard_status = SKATEBOARD_UP; // up and down
int bonus_duration = 2;
int skipObstacle_flag = SKIPOBSTACLE_OFF; // skateboard status flag
int skipObstacle_angle = 0;
float BonusCoin_height = 1;

//----------------------function declaration---------------------\\

//file
void check_highscore(char *score_str);
//menu
void menu();
void high_score();
void options();
void options_selectCoins();
void options_selectPlayer();
void Pause_game_button();
void Pause_game();

float Triangle_area(int x1, int y1, int x2, int y2, int x3, int y3);
bool isInside(int x1, int y1, int x2, int y2, int x3, int y3, int x, int y);
void drawBitmapText(char *string, float x, float y, float z);

////// coins
void SkateBoard_coin();
void BonusCoins();
void drawCoins();
void skipObstacles_coin();

/////// player
void initRandom();
void draw_player();
void draw();
void Idle();
void PlayerMovement(float v);
void Jump();
void bend();
void SkateBoard();
void skipObstacles();
void Jack();
void Robot();

//// initiates
void init();
void initTexture();
void loadTexture();
void loadBricks(); // floor1 bricks
void initFloor();
void initFloorColor();
void initObstacles();
void initCoins();
void initPlayer();
void ResetGame();
void setting_new_game();
/////// floor declaration
void initFloorColor();
void sidecubes(); // draw bridge side cubes
void sidecubes1(); // draw regular side cubes
void sidecubes2(); // draw pipeline side cubes
void Floor(int before_after_junction); // draw bridge floor
void Floor1(int before_after_junction); // draw regular floor
void Floor2(int before_after_junction); // draw pipeline floor
void Pipe(); // draw pipe
void Select_Floor(int floor_pos);

//////junctions declaration
void JunctionL();
void JunctionT();
void JunctionR();
void randJunc(); // randomize junction

//////////////obstacles
void Check_if_crashed();// check if the player crashed on an obstacle
void Game_Over(); // game over
void RandomObstacle(int ObstacleNumber);
void Blade(); //draw blade
void Blade_movement();
void Upper_Blade(int obstacle_num); // Pipe floor : obstacle 2
void Down_Blade(int obstacle_num);// Pipe floor : obstacle 1
void Punch_movement();
void Punch(int obstacle_num);// solid floor : obstacle 1
void Missing_floor(int obstacle_num);// Bridge floor : obstacle 1
void AirPlaneCrash(int obstacle_num);// Bridge floor : obstacle 1

//////////// game /////////////

//coins and bonuses
void SkateBoard_coin() {
	switch (coins[SKATEBOARD_COIN].coin_pos[0])
	{
	case COINS_IN_CENTER:
		glTranslatef(0, 2, -(*coins[SKATEBOARD_COIN].pos_in_floor));
		break;
	case COINS_IN_RIGHT:
		glTranslatef(1.2, 2, -(*coins[SKATEBOARD_COIN].pos_in_floor));
		break;
	case COINS_IN_LEFT:
		glTranslatef(-1.2, 2, -(*coins[SKATEBOARD_COIN].pos_in_floor));
		break;
	}
	
	glColor3f(0, 0.8, 0.8);
	glScalef(0.4, 0.4, 0.1);
	glutSolidSphere(1, 32, 32);
	glScalef(2.5, 2.5, 10);
	
	switch (coins[SKATEBOARD_COIN].coin_pos[0])
	{
	case COINS_IN_CENTER:
		glTranslatef(0, -2, (*coins[SKATEBOARD_COIN].pos_in_floor));
		break;
	case COINS_IN_RIGHT:
		glTranslatef(-1.2, -2, (*coins[SKATEBOARD_COIN].pos_in_floor));
		break;
	case COINS_IN_LEFT:
		glTranslatef(1.2, -2, (*coins[SKATEBOARD_COIN].pos_in_floor));
		break;
	}

	if (floor_count >= (*coins[SKATEBOARD_COIN].pos_in_floor) - 1 && floor_count <= (*coins[SKATEBOARD_COIN].pos_in_floor) + 1)
		switch (coins[SKATEBOARD_COIN].coin_pos[0])
		{
		case COINS_IN_CENTER:
			if (players[selected_player].x > -0.5 && players[selected_player].x < 0.5)
			{
				if (players[selected_player].y >= 1.5 && players[selected_player].y <= 2)
				{
					SkateBoard_flag = SKATEBOARD_ON;
					coins[SKATEBOARD_COIN].status = COIN_OFF;
				}
			}
			break;
		case COINS_IN_RIGHT:
			if (players[selected_player].x > 1)
			{
				if (players[selected_player].y >= 1.5 && players[selected_player].y <= 2)
				{
					SkateBoard_flag = SKATEBOARD_ON;
					coins[SKATEBOARD_COIN].status = COIN_OFF;
				}
			}
			break;
		case COINS_IN_LEFT:
			if (players[selected_player].x < -1)
			{
				if (players[selected_player].y >= 1.5 && players[selected_player].y <= 2)
				{
					SkateBoard_flag = SKATEBOARD_ON;
					coins[SKATEBOARD_COIN].status = COIN_OFF;
				}
			}
			break;
		}
}
void BonusCoins() {
	if(select_coins_flag == YELLOW_COINS)
		glColor3f(1, 1, 0);
	else if (select_coins_flag == RED_COINS)
		glColor3f(1, 0.1, 0.1);
	else if (select_coins_flag == GREEN_COINS)
		glColor3f(0.1, 1, 0.1);
	else if (select_coins_flag == BLUE_COINS)
		glColor3f(0.1, 0.1, 1);

	glScalef(0.4, 0.4, 0.1);
	glutSolidSphere(1, 32, 32);
	glScalef(2.5, 2.5, 10);
}
void skipObstacles_coin() {
	switch (coins[SKIP_OBSTACLE_COIN].coin_pos[0])
	{
	case COINS_IN_CENTER:
		glTranslatef(0, 2, -(*coins[SKIP_OBSTACLE_COIN].pos_in_floor));
		break;
	case COINS_IN_RIGHT:
		glTranslatef(1.2, 2, -(*coins[SKIP_OBSTACLE_COIN].pos_in_floor));
		break;
	case COINS_IN_LEFT:
		glTranslatef(-1.2, 2, -(*coins[SKIP_OBSTACLE_COIN].pos_in_floor));
		break;
	}
	glColor3f(0.8, 0.1, 0.3);
	glScalef(0.4, 0.4, 0.1);
	glutSolidSphere(1, 32, 32);
	glScalef(2.5, 2.5, 10);

	switch (coins[SKIP_OBSTACLE_COIN].coin_pos[0])
	{
	case COINS_IN_CENTER:
		glTranslatef(0, -2, (*coins[SKIP_OBSTACLE_COIN].pos_in_floor));
		break;
	case COINS_IN_RIGHT:
		glTranslatef(-1.2, -2, (*coins[SKIP_OBSTACLE_COIN].pos_in_floor));
		break;
	case COINS_IN_LEFT:
		glTranslatef(1.2, -2, (*coins[SKIP_OBSTACLE_COIN].pos_in_floor));
		break;
	}

	if (floor_count >= (*coins[SKIP_OBSTACLE_COIN].pos_in_floor) - 1 && floor_count <= (*coins[SKIP_OBSTACLE_COIN].pos_in_floor) + 1)
		switch (coins[SKIP_OBSTACLE_COIN].coin_pos[0])
		{
		case COINS_IN_CENTER:
			if (players[selected_player].x > -0.5 && players[selected_player].x < 0.5)
			{
				if (players[selected_player].y >= 1.5 && players[selected_player].y <= 2)
				{
					skipObstacle_flag = SKIPOBSTACLE_ON;
					coins[SKIP_OBSTACLE_COIN].status = COIN_OFF;
				}
			}
			break;
		case COINS_IN_RIGHT:
			if (players[selected_player].x > 1)
			{
				if (players[selected_player].y >= 1.5 && players[selected_player].y <= 2)
				{
					skipObstacle_flag = SKIPOBSTACLE_ON;
					coins[SKIP_OBSTACLE_COIN].status = COIN_OFF;
				}
			}
			break;
		case COINS_IN_LEFT:
			if (players[selected_player].x < -1)
			{
				if (players[selected_player].y >= 1.5 && players[selected_player].y <= 2)
				{
					skipObstacle_flag = SKIPOBSTACLE_ON;
					coins[SKIP_OBSTACLE_COIN].status = COIN_OFF;
				}
			}
			break;
		}
}

////////floor
void loadTexture() {

	//settings of the sky texture so we can load it.
	glBindTexture(GL_TEXTURE_2D, images_arr[0]);
	glEnable(GL_TEXTURE_2D);
	sky = (unsigned char*)malloc(256 * 256 * 3);
	file = fopen("sky.bmp", "rb");
	fread(sky, 256 * 256 * 3, 1, file);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 256, 256, GL_BGR_EXT, GL_UNSIGNED_BYTE, sky);

	fclose(file);
	free(sky);
	glRotatef(180, 0, 0, 1);
	glBegin(GL_QUADS);
	glTexCoord3d(0.0, 0.0, 0.0); glVertex3d(-35.0, 30.0, -50.0);
	glTexCoord3d(1.0, 0.0, 0.0); glVertex3d(35.0, 30.0, -50.0);
	glTexCoord3d(1.0, 1.0, 1.0); glVertex3d(35.0, -30.0, -50.0);
	glTexCoord3d(0.0, 1.0, 1.0); glVertex3d(-35.0, -30.0, -50.0);
	glEnd();

	glRotatef(-180, 0, 0, 1);
}
void loadBricks() {
	
	//settings of the bricks texture, it was used in the solid floor
	glBindTexture(GL_TEXTURE_2D, images_arr[1]);
	glEnable(GL_TEXTURE_2D);
	brick = (unsigned char*)malloc(256 * 256 * 3);
	file = fopen("bricks1.bmp", "rb");
	fread(brick, 256 * 256 * 3, 1, file);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 256, 256, GL_BGR_EXT, GL_UNSIGNED_BYTE, brick);

	for (int i = 0;i < 5;i++) {
		glBegin(GL_QUADS);
		glColor3f(FloorColor[1].red, FloorColor[1].green, FloorColor[1].blue);
		glNormal3f(0, 1, 0);	// normal straight up
		glTexCoord3d(0.0, 0.0, 0.0); glVertex3f(-2.5, 0, 0); //floor left down
		glTexCoord3d(1.0, 0.0, 0.0); glVertex3f(2.5, 0, 0); // floor right down
		glTexCoord3d(1.0, 1.0, 1.0); glVertex3f(2.5, 0, -Floor_length / 5); // floor right up
		glTexCoord3d(0.0, 1.0, 1.0); glVertex3f(-2.5, 0, -Floor_length / 5); // floor left up
		glEnd();
		glTranslatef(0, 0, -Floor_length / 5);
	}

	glTranslatef(0, 0, Floor_length);
	fclose(file);
	free(brick);
}
void RandomObstacle(int ObstacleNumber) {

	/// draw first obstacle
	if (ObstacleNumber == 0)
	{
		//each floor has its own obstacle:
		//pipe floor: down blade, upper blade
		//bridge floor: missing floor, airplane crash
		//solid floor: punch
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
			case AIRPLANE_CRASH: AirPlaneCrash(0);
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

	//draw the second obstacle
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
			case AIRPLANE_CRASH: AirPlaneCrash(1);
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
	//the meanning of the floor_pos is whether we actually want to draw the floor before the junction
	//or the floor after the junction
	if (floor_pos == 0)
	{
		switch (Floor_Num)
		{
		case BRIDGE: Floor(0);
			break;
		case SOLID: Floor1(0);
			break;
		case PIPE: Floor2(0);
			break;
		}
		//allow the draw of bonus coins only in the floor before the junction
		drawCoins();
		if (coins[SKATEBOARD_COIN].status == COIN_ON)
			SkateBoard_coin();	
		else if (coins[SKIP_OBSTACLE_COIN].status == COIN_ON)
			skipObstacles_coin();
	}
	//draw the second floor, without obtacles and bonus coins (theres no need of it)
	else {
		switch (temp_Floor_Num)
		{
		case BRIDGE: Floor(1);
			break;
		case SOLID: Floor1(1);
			break;
		case PIPE: Floor2(1);
			break;
		}
	}
}
void Floor(int before_after_junction) {
	int i;
	
	for (i = 0;i<(Floor_length / 1.5);i++)
	{
		// Floor
		if (before_after_junction == 0) // draw obstacles for the floor before junction
		{
			// draw the obstacle postion if we get to its position in the floor
			if (i == (floors[BRIDGE].obstacles_pos[0] / 1.5))
			{
				RandomObstacle(0);
				if (floors[BRIDGE].obstacles[0]->name == MISSING_FLOOR)
				{
					glTranslatef(0.0, 0, PATH_Z);
					continue;
				}
			}
			else if (i == (floors[BRIDGE].obstacles_pos[1] / 1.5))
			{
				RandomObstacle(1);
				if (floors[BRIDGE].obstacles[1]->name == MISSING_FLOOR)
				{
					glTranslatef(0.0, 0, PATH_Z);
					continue;
				}
			}
		}
		//draw the bridge logs

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
	
	//draw a pipe to be used in the pipes floor
	//the pipe was drawn by drawing 24 squares in a synchronic circle
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
void Floor1(int before_after_junction) {
	// draw a solid floor with bricks texture
	loadBricks();
	glFlush();

	glTranslatef(2.25, 0, 0);
	sidecubes1();
	glTranslatef(-4.5, 0, 0);
	sidecubes1();
	glTranslatef(2.25, 0, 0);
	if (before_after_junction == 0) // draw obstacles for the floor before junction
	{
		if (Floor_length == 45)
		{
			RandomObstacle(0);
			RandomObstacle(1);
		}
		else if (Floor_length == 30)
			RandomObstacle(0);
	}

}

void Floor2(int before_after_junction) {
	//draw the pipes floor
	int i;
	glTranslatef(0, -0.5, 0);
	for (i = 0;i<Floor_length;i++) {
		// Floor

		//first obstacle
		if (before_after_junction == 0) // draw obstacles for the floor before junction
		{
			if (i == floors[Floor_Num].obstacles_pos[0])
				RandomObstacle(0);

			//seconds obstacle
			if (i == floors[Floor_Num].obstacles_pos[1] && floors[Floor_Num].obstacles_pos[1] != 0)
				RandomObstacle(1);
		}
		Pipe();
		glTranslatef(0.0, 0, -1.1);
	}
	glTranslatef(0, 0.5, (Floor_length * (1.1)));
}
void JunctionT()
{
	//draw T junction ahead of the player in each time its been called, the player can choose to go left or right
	//when he gets to the relative position in the floor
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
	//draw right junction ahead of the player in each time its been called, the player has to go right
	//when he gets to the relevant position in the floor

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
	glTranslatef(0, 0, (Floor_length + 2.5));
	glRotatef(90, 0, 1, 0);
	Junction_Flag = R_JUNCTION;
}
void JunctionL()
{
	//draw left junction ahead of the player in each time its been called, the player has to go left
	//when he gets to the relative position in the floor

	glRotatef(90, 0, 1, 0);
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
	glRotatef(90, 0, 1, 0);
	glTranslatef(0, 0, -2.5);
	Select_Floor(1);
	glTranslatef(0, 0, 2.5);
	glRotatef(-90, 0, 1, 0);
	glTranslatef(0, 0, (Floor_length + 2.5));
	glRotatef(-90, 0, 1, 0);
	Junction_Flag = L_JUNCTION;
}
void randJunc() {
	//function that is called in the draw function and calls the desired junction
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
	//the sides of the bridge floor

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
	//the sides of the solid floor
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
	//a function to draw a single blade, that was used in the upper and down blades obstacle

	glScalef(0.8, 0.8, 0.8);
	double a, x, y;
	glColor3f(1, 1, 1);
	glBegin(GL_POLYGON);
	//a for loop that draws a 2D circle
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
	//a for loop that draws triangles on the frame of the circle
	//by drawing between two points that 20 pixles seperates them in a circular motion
	for (int i = 20;i <= 720;i += 20)
	{
		a = M_PI * i;
		x = cos(a / 360);
		y = sin(a / 360);
		glVertex3f(x, y, 0);
		glVertex3f(Xc, Yc, 0);

		//conditions to draw the upper vertex of the triangle by the relevant quatter of the circle
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

	//blade controller, that blade moves up and down in the down blade obsacle by movement in the y axe
	//the blade moves left and right by movement in the x axe in the upper blade obstacle
	//both obstacles rotates in the z axe
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
	else if (Blade_flag == BLADE_DOWN) {

		Blade_x += 0.02;
		Blade_y -= 0.005;
		if (Blade_y <= 0) Blade_flag = BLADE_UP;
	}

}
void Upper_Blade(int obstacle_num)
{
	//draw of the upper blade. a blade is drawn on a standard 

	if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_RIGHT) // translate to right
		glTranslatef(2.5, 0, 0);
	else if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_LEFT)// translate to left
	{
		glTranslatef(-2.5, 0, 0);
		glRotatef(180, 0, 1, 0);
	}
	glScalef(1, 2, 1);
	  sidecubes();
	glScalef(1, 0.5, 1);

	glTranslatef(0, (2 * 1.5), 0);
		glRotatef(90, 0, 0, 1);
			glPushMatrix();
				glScalef(1, 2, 1);
					sidecubes();
		    glPopMatrix();
	    glRotatef(-90, 0, 0, 1);

		//rotation in z axe, and movement in the x axe
    glTranslatef(Blade_x, 0, 0);
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
	//draw a down blade
	if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_RIGHT) // translate to right
		glTranslatef(1.2, 0, 0);
	else if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_LEFT)// translate to left
		glTranslatef(-1.2, 0, 0);
	//movement in y axe, rotation in z axe
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
	//draw escape path when thers a missing floor in the bridge

	if (floors[BRIDGE].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_RIGHT) // translate to right
		glTranslatef(2.5 / 2, 0, 0);
	else if (floors[BRIDGE].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_LEFT)// translate to left
		glTranslatef(-(2.5 / 2), 0, 0);

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

	////movent of the punch by extend and squeeze
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
	if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_RIGHT) // translate to right
		glTranslatef(1.85, 1, -(floors[Floor_Num].obstacles_pos[obstacle_num]));
	else if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_LEFT)// translate to left
	{
		glTranslatef(-1.85, 1, -(floors[Floor_Num].obstacles_pos[obstacle_num]));
		glRotatef(180, 0, 1, 0);
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

	double a, x = 0, y, z;
	glColor3f(1, 1, 1);
	//draw of the spring, we draw a polygon but with movement in x axe
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
	//draw a ball by the end of the spring
	glTranslatef((720 * (Punch_x) * 5) - 0.8, 0, 0);
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

void AirPlaneCrash(int obstacle_num)
{
	//the draw of the airplane
	if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_RIGHT) // translate to right
	{
		glTranslatef(3.5, 1.5, 0);
		glRotatef(20, 0, 0, 1);
	}
	else if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_LEFT)// translate to left
	{
		glTranslatef(-3.5, 1.5, 0);
		glRotatef(-20, 0, 0, 1);
		glRotatef(180, 0, 1, 0);
	}
	glPushMatrix();
	//airplane body, 24 squares in a circular motion and its vertices are not similar
	int ACC = 48;
	float a, p, q;
	int i;
	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 1, 1);
	for (i = 0;i <= ACC;i++)
	{
		a = i*(M_PI) * 2 / ACC;
		p = cos(a);
		q = sin(a);
		glNormal3f(0, q, p);
		glVertex3f(2.5, q * 0.6, p * 0.6);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(-2.5, q, p);
	}
	glEnd();
	
	//airplane front
	glPushMatrix();
		glTranslatef(-2.5, 0, 0);
			glScalef(1.5, 1.1, 1);
				glutSolidSphere(0.9, 32, 32);

	glPopMatrix();
		glTranslatef(2.4, 0, 0);
			glutSolidSphere(0.6, 32, 32);
	glPopMatrix();

	//airplane wings
	
	glPushMatrix();
	glColor3f(0.5, 0.4, 0.1);
		glTranslatef(-1, 0, 1.8);
		glRotatef(20, 0, 1, 0);
			glScalef(0.5, 0.1, 3);
				glutSolidSphere(1, 32, 32);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.5, 0.4, 0.1);
		glTranslatef(-1, 0, -1.8);
		glRotatef(-20, 0, 1, 0);
			glScalef(0.5, 0.1, 2.8);
				glutSolidSphere(1, 32, 32);
	glPopMatrix();

	//airplane tail
	glPushMatrix();
		glColor3f(0.5, 0.4, 0.1);
			glTranslatef(2.5, 0.2, 0);
			glBegin(GL_QUADS);
			
			glVertex3f(0, 0, 0.5);
			glVertex3f(0.4, 1.5, 0.2);
			glVertex3f(0.4, 1.5, -0.2);
			glVertex3f(0, 0, -0.5);

			glVertex3f(0.2, 1.5, 0.2);
			glVertex3f(0.2, 1.5, -0.2);
			glVertex3f(-0.2, 0, -0.5);
			glVertex3f(-0.2, 0, 0.5);

			glVertex3f(0.2, 1.5, 0.2);
			glVertex3f(0.2, 1.5, -0.2);
			glVertex3f(0.4, 1.5, -0.2);
			glVertex3f(0.4, 1.5, 0.2);

			glVertex3f(0, 0, 0.5);
			glVertex3f(-0.2, 0, 0.5);
			glVertex3f(0.2, 1.5, -0.2);
			glVertex3f(0.4, 1.5, -0.2);

			glVertex3f(0, 0, 0.5);
			glVertex3f(-0.2, 0, 0.5);
			glVertex3f(0.2, 1.5, 0.2);
			glVertex3f(0.4, 1.5, 0.2);

			glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.5, 0.4, 0.1);
			glTranslatef(2.5, 0, 1);
			glRotatef(15, 0, 1, 0);
			glScalef(0.3, 0.1, 1);
					glutSolidSphere(1, 32, 32);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.5, 0.4, 0.1);
		glTranslatef(2.5, 0, -1);
		glRotatef(-15, 0, 1, 0);
		glScalef(0.3, 0.1, 1);
			glutSolidSphere(1, 32, 32);
	glPopMatrix();

	if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_RIGHT) // translate to right
	{
		glRotatef(-20, 0, 0, 1);
		glTranslatef(-3.5, -1.5, 0);
	}
	else if (floors[Floor_Num].obstacles[obstacle_num]->pos_in_floor == OBSTACLE_IN_LEFT)// translate to left
	{
		glRotatef(-180, 0, 1, 0);
		glRotatef(20, 0, 0, 1);
		glTranslatef(3.5, -1.5, 0);
	}
}

void Check_if_crashed() {
	//the function that checks if there was a hit on one of the obstacles
	if (SkateBoard_flag == SKATEBOARD_OFF && skipObstacle_flag == SKIPOBSTACLE_OFF)
	{
		for (int i = 0; i < 2; i++)
		{
			switch (floors[Floor_Num].obstacles[i]->name) // check first obstacle
			{
			case DOWN_BLADE: { // down blade
				if ((players[selected_player].y < Blade_y + 1) && ((floor_count >= (floors[Floor_Num].obstacles_pos[i] * 1.1) - 0.5 && floor_count <= (floors[Floor_Num].obstacles_pos[i] * 1.1) + 0.8)))
				{
					if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_RIGHT) && (players[selected_player].x >= 1.1 && players[selected_player].x <= 1.3))
						obstacles[DOWN_BLADE].Obstacle_Hit = OBSTACLE_HIT;
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_LEFT) && (players[selected_player].x <= -1.1 && players[selected_player].x >= -1.3))
						obstacles[DOWN_BLADE].Obstacle_Hit = OBSTACLE_HIT;
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_CENTER) && (players[selected_player].x >= -0.5 && players[selected_player].x <= 0.5))
						obstacles[DOWN_BLADE].Obstacle_Hit = OBSTACLE_HIT;
				}
				break;
			}
			case UPPER_BLADE:  // upper blade
			{
				if (players[selected_player].y >= 0.4 && ((floor_count >= (floors[Floor_Num].obstacles_pos[i] * 1.1) - 0.5 && floor_count <= (floors[Floor_Num].obstacles_pos[i] * 1.1) + 0.8)))
				{
					if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_RIGHT) && (players[selected_player].x >= -1))
						obstacles[UPPER_BLADE].Obstacle_Hit = OBSTACLE_HIT;
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_LEFT) && (players[selected_player].x <= 1))
						obstacles[UPPER_BLADE].Obstacle_Hit = OBSTACLE_HIT;
				}
				break;
			}
			case MISSING_FLOOR:  // MISSING_FLOOR
			{
				if (players[selected_player].y <= 0.6 && ((floor_count >= floors[Floor_Num].obstacles_pos[i] - 0.1 && floor_count <= floors[Floor_Num].obstacles_pos[i] + 1)))
				{
					if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_RIGHT) && (players[selected_player].x <= 1))
						obstacles[MISSING_FLOOR].Obstacle_Hit = OBSTACLE_HIT;
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_LEFT) && (players[selected_player].x >= -1))
						obstacles[MISSING_FLOOR].Obstacle_Hit = OBSTACLE_HIT;
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_CENTER) && (players[selected_player].x < -0.5 || players[selected_player].x > 0.5))
						obstacles[MISSING_FLOOR].Obstacle_Hit = OBSTACLE_HIT;
				}
				break;
			}
			case PUNCH:  // PUNCH
			{
				if (((floor_count >= floors[Floor_Num].obstacles_pos[i] - 0.5 && floor_count <= floors[Floor_Num].obstacles_pos[i] + 1)))
				{
					if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_RIGHT) && ((players[selected_player].x >= 2.25 + ((720 * (Punch_x) * 5))) && Punch_flag == PUNCH_EXTEND || players[selected_player].x >= 1))
						obstacles[PUNCH].Obstacle_Hit = OBSTACLE_HIT;
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_LEFT) && ((players[selected_player].x <= -2.25 - ((720 * (Punch_x) * 5))) && Punch_flag == PUNCH_EXTEND || players[selected_player].x <= -1))
						obstacles[PUNCH].Obstacle_Hit = OBSTACLE_HIT;
				}
				break;
			}
			case AIRPLANE_CRASH:  // PUNCH
			{
				if (((floor_count >= floors[Floor_Num].obstacles_pos[i] - 0.5 && floor_count <= floors[Floor_Num].obstacles_pos[i] + 0.5)))
				{
					if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_RIGHT) && (players[selected_player].x >= -0.5))
						obstacles[AIRPLANE_CRASH].Obstacle_Hit = OBSTACLE_HIT;
					else if ((floors[Floor_Num].obstacles[i]->pos_in_floor == OBSTACLE_IN_LEFT) && (players[selected_player].x <= 0.5))
						obstacles[AIRPLANE_CRASH].Obstacle_Hit = OBSTACLE_HIT;
				}
				break;
			}
			}
		}
	}
	Game_Over();
}

void Game_Over() {
	//function to check if there was a hit in any obstacle in the relevant floor

	if (obstacles[DOWN_BLADE].Obstacle_Hit == OBSTACLE_HIT)
	{
		startflag = PAUSE;
		playing_menu_flag = GAME_FINISHED_NO_MENU;

		//if there was hit, do the unique losing animation of the down blade and decrease life by 1
		if (obstacles[DOWN_BLADE].GameOver == GAME_OVER_OFF)
		{
			players[selected_player].y -= 0.005;
			if (players[selected_player].y <= -0.5)
				obstacles[DOWN_BLADE].GameOver = GAME_OVER_ON;
		}
		else
		{
			if (life > 0)
			{
				life--;
				setting_new_game();
				obstacles[DOWN_BLADE].Obstacle_Hit = OBSTACLE_MISS;
				obstacles[DOWN_BLADE].GameOver = GAME_OVER_OFF;
				playing_menu_flag = HIT;
			}
			else
			{
				check_highscore(score_str);
				ResetGame();
				menu_flag = HIGHSCORE;
			}

		}

	}
	else if (obstacles[UPPER_BLADE].Obstacle_Hit == OBSTACLE_HIT)
	{
		startflag = PAUSE;
		playing_menu_flag = GAME_FINISHED_NO_MENU;

		//if there was hit, do the unique losing animation of the upper blade and decrease life by 1
		if (obstacles[UPPER_BLADE].GameOver == GAME_OVER_OFF)
		{
			bend_angle += 1.5;
			players[selected_player].y -= 0.012;

			if (bend_angle >= 80)
				obstacles[UPPER_BLADE].GameOver = GAME_OVER_ON;
		}
		else {
			if (life > 0)
			{
				life--;
				setting_new_game();
				obstacles[UPPER_BLADE].Obstacle_Hit = OBSTACLE_MISS;
				obstacles[UPPER_BLADE].GameOver = GAME_OVER_OFF;
				playing_menu_flag = HIT;
			}
			else
			{
				check_highscore(score_str);
				ResetGame();
				menu_flag = HIGHSCORE;
			}

		}
	}
	else if (obstacles[MISSING_FLOOR].Obstacle_Hit == OBSTACLE_HIT)
	{
		startflag = PAUSE;
		playing_menu_flag = GAME_FINISHED_NO_MENU;

		//if there was hit, do the unique losing animation of the missing floor and decrease life by 1
		//missing floor is used also if the player gets to the end of the floor
		if (obstacles[MISSING_FLOOR].GameOver == GAME_OVER_OFF)
		{
			players[selected_player].y -= 0.05;
			if (players[selected_player].y <= -4)
				obstacles[MISSING_FLOOR].GameOver = GAME_OVER_ON;
		}
		else {
			if (life > 0)
			{
				life--;
				setting_new_game();
				obstacles[MISSING_FLOOR].Obstacle_Hit = OBSTACLE_MISS;
				obstacles[MISSING_FLOOR].GameOver = GAME_OVER_OFF;
				playing_menu_flag = HIT;
			}
			else
			{
				check_highscore(score_str);
				setting_new_game();
				menu_flag = HIGHSCORE;
			}

		}
	}
	else if (obstacles[AIRPLANE_CRASH].Obstacle_Hit == OBSTACLE_HIT)
	{
		startflag = PAUSE;
		playing_menu_flag = GAME_FINISHED_NO_MENU;

		//if there was hit, do the unique losing animation of the airplane crash and decrease life by 1
		if (obstacles[AIRPLANE_CRASH].GameOver == GAME_OVER_OFF)
		{

			players[selected_player].y += 0.05;
			players[selected_player].z -= 0.05;
			bend_angle -= 3.0;
			if (players[selected_player].y >= 3)
				obstacles[AIRPLANE_CRASH].GameOver = GAME_OVER_ON;
		}
		else {
			if (life > 0)
			{
				life--;
				setting_new_game();
				obstacles[AIRPLANE_CRASH].Obstacle_Hit = OBSTACLE_MISS;
				obstacles[AIRPLANE_CRASH].GameOver = GAME_OVER_OFF;
				playing_menu_flag = HIT;
			}
			else
			{
				check_highscore(score_str);
				ResetGame();
				menu_flag = HIGHSCORE;
			}

		}
	}
	else if (obstacles[PUNCH].Obstacle_Hit == OBSTACLE_HIT)
	{
		startflag = PAUSE;
		playing_menu_flag = GAME_FINISHED_NO_MENU;

		//if there was hit, do the unique losing animation of the punch and decrease life by 1
		if (obstacles[PUNCH].GameOver == GAME_OVER_OFF)
		{
			int distance1, distance2; // so we can check to which obstacle we are closer
			distance1 = abs(floors[Floor_Num].obstacles_pos[0] - (int)floor_count); // position of the first punch
			distance2 = abs(floors[Floor_Num].obstacles_pos[1] - (int)floor_count); // position of the second punch
			if (distance1 < distance2) // hit by the first punch
				if (floors[Floor_Num].obstacles[0]->pos_in_floor == OBSTACLE_IN_RIGHT)
					players[selected_player].x -= 0.05;
				else players[selected_player].x += 0.05;
			else // hit by the second punch
				if (floors[Floor_Num].obstacles[1]->pos_in_floor == OBSTACLE_IN_RIGHT)
					players[selected_player].x -= 0.05;
				else players[selected_player].x += 0.05;

				players[selected_player].y += 0.05;
				bend_angle += 1.5;

				if (players[selected_player].x <= -6 || players[selected_player].x >= 6)
					obstacles[PUNCH].GameOver = GAME_OVER_ON;
		}
		else {
			if (life > 0) {
				life--;
				setting_new_game();
				obstacles[PUNCH].Obstacle_Hit = OBSTACLE_MISS;
				obstacles[PUNCH].GameOver = GAME_OVER_OFF;
				playing_menu_flag = HIT;
			}
			else
			{
				check_highscore(score_str);
				ResetGame();
				menu_flag = HIGHSCORE;
			}

		}
	}
	else if (check_junction_flag == OBSTACLE_HIT)
	{
		startflag = PAUSE;
		playing_menu_flag = GAME_FINISHED_NO_MENU;
		if (junction_game_over == GAME_OVER_OFF)
		{
			players[selected_player].y -= 0.05;
			if (Jflag == R_JUNCTION)
				players[selected_player].x = -3;
			
			else if (Jflag == L_JUNCTION)
				players[selected_player].x = 3;

			if (players[selected_player].y <= -4)
				junction_game_over = GAME_OVER_ON;
		}
		else {
			if (life > 0) {
				life--;
				setting_new_game();
				check_junction_flag = OBSTACLE_MISS;
				junction_game_over = GAME_OVER_OFF;
				playing_menu_flag = HIT;
			}
			else
			{
				check_highscore(score_str);
				ResetGame();
				menu_flag = HIGHSCORE;
			}

		}
	}
}
void check_highscore(char *highscore) {
	//function that reads from existing file, and rewrite if there was a new highscore
	char read_line[20]="";
	int score_array[10] = {0};
	FILE *highscore_file = fopen("highscore.txt", "r");
	if (highscore_file == NULL)
		exit(1);
	int i = 0;
	//exchange the file to an array
	while (fgets(read_line, 20, highscore_file) != NULL)
		score_array[i++] = atoi(read_line);
	
	//check if theres new highscore, if there was such a new one, put it in the relevant place and sort the array
	int temp = 0, temp1 =0;
	for (i = 0;i < 10;i++)
	{
		if (score_array[i] < atoi(highscore))
		{
			new_score_position = i;
			temp = score_array[i];
			score_array[i] = atoi(highscore);
			for (;i < 9;i++)
			{
				temp1 = score_array[i + 1];
				score_array[i + 1] = temp;
				temp = temp1;
			}
			break;
		}
	}
		fclose(highscore_file);
		//rewrite to the file
		highscore_file = fopen("highscore.txt", "w");
		for (i = 0;i < 10;i++)
		{
			fprintf(highscore_file, "%d\n", score_array[i]);
		}
	fclose(highscore_file);
}

void bend() {
	//if there was press on '2', the player bends by changing the angle of the playing and going down in y axe
	if (bend_status == BEND_DOWN)
	{
		if (bend_angle >= 80)
			bend_status = BEND_BACK_TO_NORMAL;
		bend_angle += 1.5;
		players[selected_player].y -= 0.012;
	}
	else if (bend_status == BEND_BACK_TO_NORMAL)
	{
		if (bend_angle <= 0)
		{
			bend_status = STANDING;
			Player_MovementStatus = LEFT_ARM_RIGHT_LEG;
		}
		bend_angle -= 2;
		players[selected_player].y += 0.016;
	}
}
void Jump() {
	//if there was press on '8', the player jumps by increasing the y, and back to normal when it reachs the hight limit
	if (Jump_status == JUMPING_UP)
	{
		if (players[selected_player].y >= JUMP_LIMIT)
			Jump_status = JUMPING_DOWN;
		//ty += 0.03;
		players[selected_player].y += (floors[Floor_Num].velocity / 1.3);
	}
	else if (Jump_status == JUMPING_DOWN)
	{
		if (players[selected_player].y <= 0.5)
		{
			Jump_status = WALKING;
			players[selected_player].y = 0.5;
		}
		//ty -= 0.03;
		players[selected_player].y -= (floors[Floor_Num].velocity / 1.8);
	}
}

////// jack
void DrawArm(float xPos, float yPos, float zPos)
{
	//draw the arm of the player by given x,y,z position
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
	//draw the head of the player by given x,y,z position
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
	//draw the torso of the player by given x,y,z position

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
	//draw the legs of the player by given x,y,z position
	glPushMatrix();

	/*      Sets color to yellow*/
	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(xPos, yPos, zPos);

	glColor3f(1, 1, 1);
	glScalef(0.5, 1.75, 0.5);
	glutSolidSphere(1, 32, 32);


	glPopMatrix();
}
void Jack()
{
	//combine the body by calling the functions of arms, legs, head and torso to get full body of jack
	//when skateboard flag is on, put the player in a new hight, rotate him and stop his movement
	if (SkateBoard_flag == SKATEBOARD_ON)
	{
		glTranslatef(0, SkateBoard_y, 0);
		SkateBoard();
		glRotatef(90, 0, 1, 0);
		armAngle[0] = 15;
		armAngle[1] = -15;
	}
	//when skip obstacle flag is on, rotate the player in x axe and stop his movement

	else if (skipObstacle_flag == SKIPOBSTACLE_ON)
	{
		glRotatef(-skipObstacle_angle, 1, 0, 0);
		skipObstacles();
		armAngle[0] = 0;
		armAngle[1] = 0;
	}
	
	glScalef(0.35, 0.35, 0.35);
	DrawTorso(0, 0, 0);
	DrawHead(0, 3.5, 0);

	glRotatef(armAngle[0], 1, 0, 0); // rotate left arm
	DrawArm(-2, 0, 0);
	if (obstacles[DOWN_BLADE].Obstacle_Hit == OBSTACLE_MISS)
		DrawLeg(1, -4, 0);
	glRotatef(-armAngle[0], 1, 0, 0); // rotate left arm

	glRotatef(armAngle[1], 1, 0, 0); // rotate left arm
	if (obstacles[DOWN_BLADE].Obstacle_Hit == OBSTACLE_MISS)
		DrawLeg(-1, -4, 0);
	DrawArm(2, 0, 0);

	glRotatef(-armAngle[1], 1, 0, 0); // rotate right arm
	glScalef(7, 7, 7);

	if (skipObstacle_flag == SKIPOBSTACLE_ON)
		glRotatef(skipObstacle_angle, 1, 0, 0);
	else if (SkateBoard_flag == SKATEBOARD_ON)
	{
		glRotatef(-90, 0, 1, 0);
		glTranslatef(0, -SkateBoard_y, 0);
	}

}

//robot
void DrawArm_Robot(float xPos, float yPos, float zPos)
{
	//draw the arm of the robot by given x,y,z position
	glPushMatrix();

	/*      Sets color to red*/
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(xPos, yPos, zPos);

	glColor3f(1, 1, 1);
	glTranslatef(0, 0, 0);
	glScalef(0.5, 1.65, 0.5);
	glutSolidCube(1.8);

	glPopMatrix();
}
void DrawHead_Robot(float xPos, float yPos, float zPos)
{
	//draw the head of the robot by given x,y,z position
	glPushMatrix();

	/*   Sets color to white*/
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(xPos, yPos, zPos);
	glutSolidCube(1.8);

	glPopMatrix();
}
void DrawTorso_Robot(float xPos, float yPos, float zPos)
{
	//draw the torso of the robot by given x,y,z position
	glPushMatrix();

	/*      Sets color to blue*/
	glColor3f(0.0f, 0.0f, 1.0f);
	glTranslatef(xPos, yPos, zPos);

	glColor3f(1, 1, 1);
	glScalef(1.5, 2.3, 1.5);
	glutSolidCube(1.8);

	glPopMatrix();
}
void DrawLeg_Robot(float xPos, float yPos, float zPos)
{
	//draw the legs of the robot by given x,y,z position
	glPushMatrix();

	/*      Sets color to yellow*/
	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(xPos, yPos, zPos);

	glColor3f(1, 1, 1);
	glScalef(0.5, 1.75, 0.5);
	glutSolidCube(1.8);

	glPopMatrix();
}
void Robot()
{
	//combine the body by calling the functions of arms, legs, head and torso to get full body of the robot
	//when skateboard flag is on, put the player in a new hight, rotate him and stop his movement
	if (SkateBoard_flag == SKATEBOARD_ON)
	{
		glTranslatef(0, SkateBoard_y, 0);
		SkateBoard();
		glRotatef(90, 0, 1, 0);
		armAngle[0] = 15;
		armAngle[1] = -15;
	}
	else if (skipObstacle_flag == SKIPOBSTACLE_ON)
	{
		glRotatef(-skipObstacle_angle, 1, 0, 0);
		skipObstacles();
		armAngle[0] = 0;
		armAngle[1] = 0;
	}
	

	glScalef(0.35, 0.35, 0.35);
	DrawTorso_Robot(0, 0, 0);
	DrawHead_Robot(0, 3.5, 0);

	glRotatef(armAngle[0], 1, 0, 0); // rotate left arm
	DrawArm_Robot(-2, 0, 0);
	if (obstacles[DOWN_BLADE].Obstacle_Hit == OBSTACLE_MISS)
		DrawLeg_Robot(1, -4, 0);
	glRotatef(-armAngle[0], 1, 0, 0); // rotate left arm

	glRotatef(armAngle[1], 1, 0, 0); // rotate left arm
	if (obstacles[DOWN_BLADE].Obstacle_Hit == OBSTACLE_MISS)
		DrawLeg_Robot(-1, -4, 0);
	DrawArm_Robot(2, 0, 0);

	glRotatef(-armAngle[1], 1, 0, 0); // rotate right arm
	glScalef(7, 7, 7);

	if (skipObstacle_flag == SKIPOBSTACLE_ON)
		glRotatef(skipObstacle_angle, 1, 0, 0);
	else if (SkateBoard_flag == SKATEBOARD_ON)
	{
		glRotatef(-90, 0, 1, 0);
		glTranslatef(0, -SkateBoard_y, 0);
	}
}

void SkateBoard()
{
	//draw the skateboard
	glPushMatrix();

	glTranslatef(0, -2, 0);
	glColor3f(0, 0.8, 0.8);
	glScalef(1, 0.2, 3);
	glutSolidSphere(1, 32, 32);

	glPopMatrix();

}
void skipObstacles() {
	//draw the skip obstacle motion
	glColor3f(0, 0, 1);
	glutWireSphere(2, 16, 16);
	
}
void drawCoins() {
	
	switch (coins[BONUS_COINS].coin_pos[0])
	{
	case COINS_IN_RIGHT:
		glTranslatef(1.2, 0, 0);
		break;
	case COINS_IN_LEFT:
		glTranslatef(-1.2, 0, 0);
		break;
	}

	glTranslatef(0, 1, -(Floor_length / 3));
	
	//draw the coins in the floor by its position that was set in the array
	for (int i = 0;i < coins[BONUS_COINS].quantity/2 ;i++)
	{
		if (coins[BONUS_COINS].pos_in_floor[i] != 0)
		{
			if ((floor_count >= (coins[BONUS_COINS].pos_in_floor[i] - 0.4) && floor_count <= (coins[BONUS_COINS].pos_in_floor[i] + 0.4)))
			{
				//the conditions of coins hit, player gets extra 50 points to his score
				if (coins[BONUS_COINS].coin_pos[0] == COINS_IN_RIGHT)
				{
					if (players[selected_player].x > 1)
					{
						score += 80;
						coins[BONUS_COINS].pos_in_floor[i] = 0;
						glTranslatef(0, 0, -1);
						continue;
					}

				}
				else if (coins[BONUS_COINS].coin_pos[0] == COINS_IN_LEFT)
				{
					if (players[selected_player].x < -1)
					{
						score += 50;
						coins[BONUS_COINS].pos_in_floor[i] = 0;
						glTranslatef(0, 0, -1);
						continue;
					}
				}
				else if (coins[BONUS_COINS].coin_pos[0] == COINS_IN_CENTER)
				{
					if (players[selected_player].x > -1 && players[selected_player].x < 1)
					{
						score += 50;
						coins[BONUS_COINS].pos_in_floor[i] = 0;
						glTranslatef(0, 0, -1);
						continue;
					}
				}
			}
			if (SkateBoard_flag == SKATEBOARD_ON)
					glTranslatef(0, SkateBoard_y, 0);
			BonusCoins();
			if (SkateBoard_flag == SKATEBOARD_ON)
					glTranslatef(0, -SkateBoard_y, 0);
		}
		glTranslatef(0, 0, -1);
	}

	switch (coins[BONUS_COINS].coin_pos[0])
	{
	case COINS_IN_RIGHT:
		glTranslatef(-1.2, 0, 0);
		break;
	case COINS_IN_LEFT:
		glTranslatef(1.2, 0, 0);
		break;
	}

	switch (coins[BONUS_COINS].coin_pos[1])
	{
	case COINS_IN_RIGHT:
		glTranslatef(1.2, 0, 0);
		break;
	case COINS_IN_LEFT:
		glTranslatef(-1.2, 0, 0);
		break;
	}

	for (int i = coins[BONUS_COINS].quantity/2 ; i < coins[BONUS_COINS].quantity ;i++)
	{
		{
			if (coins[BONUS_COINS].pos_in_floor[i] != 0)
			{
				if ((floor_count >= (coins[BONUS_COINS].pos_in_floor[i] - 0.4) && floor_count <= (coins[BONUS_COINS].pos_in_floor[i] + 0.4)))
				{
					if (coins[BONUS_COINS].coin_pos[1] == COINS_IN_RIGHT)
					{
						if (players[selected_player].x > 1)
						{
							score += 50;
							coins[BONUS_COINS].pos_in_floor[i] = 0;
							glTranslatef(0, 0, -1);
							continue;
						}

					}
					else if (coins[BONUS_COINS].coin_pos[1] == COINS_IN_LEFT)
					{
						if (players[selected_player].x < -1)
						{
							score += 50;
							coins[BONUS_COINS].pos_in_floor[i] = 0;
							glTranslatef(0, 0, -1);
							continue;
						}
					}
					else if (coins[BONUS_COINS].coin_pos[1] == COINS_IN_CENTER)
					{
						if (players[selected_player].x > -1 && players[selected_player].x < 1)
						{
							score += 50;
							coins[BONUS_COINS].pos_in_floor[i] = 0;
							glTranslatef(0, 0, -1);
							continue;
						}
					}
				}
				if (SkateBoard_flag == SKATEBOARD_ON)
					glTranslatef(0, SkateBoard_y, 0);
				BonusCoins();
				if (SkateBoard_flag == SKATEBOARD_ON)
					glTranslatef(0, -SkateBoard_y, 0);
			}
			glTranslatef(0, 0, -1);
		}
	}

	switch (coins[BONUS_COINS].coin_pos[1])
	{
	case COINS_IN_RIGHT:
		glTranslatef(-1.2, 0, 0);
		break;
	case COINS_IN_LEFT:
		glTranslatef(1.2, 0, 0);
		break;
	}
		glTranslatef(0, -1, Floor_length);
}

void Bodymovement()
{
	if(SkateBoard_flag == SKATEBOARD_OFF && skipObstacle_flag == SKIPOBSTACLE_OFF)
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
	//movement of the player when skateboard is set, that player moves up and down with a surfer
	else if(SkateBoard_flag == SKATEBOARD_ON){
		if (skateboard_status == SKATEBOARD_UP)
		{
			if (SkateBoard_y >= 3.5)
				skateboard_status = SKATEBOARD_DOWN;
			else SkateBoard_y += 0.01;
		}
		else {
			if (SkateBoard_y <= 2.5)
				skateboard_status = SKATEBOARD_UP;
			else SkateBoard_y -= 0.01;
		}
	}
	//movement of the player when skip obstacle is set, the player rotates in x axe
	else if (skipObstacle_flag == SKIPOBSTACLE_ON)
	{
		skipObstacle_angle += 1;
		skipObstacle_angle %= 360;
	}
}

//menu 
void menu() {
	glPushMatrix();
	glScalef(3, 1, 1);
	glTranslatef(0, 4, 0);
	glBegin(GL_QUADS);
	
	//play button
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(-1, 0, 1);
	glVertex3f(-1, 2, 1);
	glVertex3f(1, 2, 1);
	glVertex3f(1, 0, 1);
	glEnd(); 
	//high score button
	glTranslatef(0, -2.2, 0);
	glBegin(GL_QUADS);
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(-1, 0, 1);
	glVertex3f(-1, 2, 1);
	glVertex3f(1, 2, 1);
	glVertex3f(1, 0, 1);
	glEnd();
	//options button
	glTranslatef(0, -2.2, 0);
	glBegin(GL_QUADS);
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(-1, 0, 1);
	glVertex3f(-1, 2, 1);
	glVertex3f(1, 2, 1);
	glVertex3f(1, 0, 1);
	glEnd();
	glPopMatrix();

	//text position and set
        if(game_started_flag == 0)
			drawBitmapText("Play", -0.7, 4.2, 5);
		else drawBitmapText("Resume", -1, 4.2, 5);
		drawBitmapText("HighScore", -1.4, 2.5, 5);
		drawBitmapText("Options", -1.2, 0.8, 5);
}
void options() {
	glPushMatrix();
	glScalef(3, 1, 1);
	glTranslatef(0, 4, 0);
	
	glBegin(GL_QUADS);
	//select player button
		glColor3f(0.3, 0.5, 0.4);
		glNormal3f(0, 0, 1);
		glVertex3f(-1, 0, 1);
		glVertex3f(-1, 2, 1);
		glVertex3f(1, 2, 1);
		glVertex3f(1, 0, 1);
	glEnd();

	//select coins color button
	glTranslatef(0, -2.2, 0);
	glBegin(GL_QUADS);
		glColor3f(0.3, 0.5, 0.4);
		glNormal3f(0, 0, 1);
		glVertex3f(-1, 0, 1);
		glVertex3f(-1, 2, 1);
		glVertex3f(1, 2, 1);
		glVertex3f(1, 0, 1);
	glEnd();

	//back button
	glTranslatef(0, -2.2, 0);
	glBegin(GL_QUADS);
	glColor3f(0.3, 0.5, 0.4);
		glNormal3f(0, 0, 1);
		glVertex3f(-1, 0, 1);
		glVertex3f(-1, 2, 1);
		glVertex3f(1, 2, 1);
		glVertex3f(1, 0, 1);
	glEnd();
	glPopMatrix();
	//text position and set
		drawBitmapText("Select Player", -1.65, 4.2, 5);
		drawBitmapText("Select Coins", -1.5, 2.5, 5);
		drawBitmapText("Back", -0.9, 0.8, 5);

}
void options_selectPlayer(){
	//select player in the option menu
	//draw the selected player in select player screen
	if (selected_player == JACK)
		Jack();
	else if (selected_player == ROBOT)
		Robot();
	glPushMatrix();
	glTranslatef(1, -1, 0);
	//right triangle
	glBegin(GL_TRIANGLES);
		glVertex3f(-0.2, 0.2, 0);
		glVertex3f(-0.2, -0.2, 0);
		glVertex3f(0.2, 0, 0);
	glEnd();

	//left triangle
	glTranslatef(-2, 0, 0);
	glBegin(GL_TRIANGLES);
		glVertex3f(0.2, 0.2, 0);
		glVertex3f(0.2, -0.2, 0);
		glVertex3f(-0.2, 0, 0);
	glEnd();

	glTranslatef(1, -0.5, 0);
	glScalef(0.4, 0.2 , 1);
	glBegin(GL_QUADS);
		glNormal3f(0, 0, 1);
		glVertex3f(-1, 0, 1);
		glVertex3f(-1, 2, 1);
		glVertex3f(1, 2, 1);
		glVertex3f(1, 0, 1);
	glEnd();

	drawBitmapText("Back", -0.65, 2, 2);
	glPopMatrix();
}
void options_selectCoins() {

	//select coins in the option menu
	//draw the selected coin color in select coins screen
	glPushMatrix();
	glScalef(2.6, 2.6, 0);
	BonusCoins();
	glTranslatef(1, -1, 0);
	if (select_coins_flag != YELLOW_COINS)
	{
		//right triangle
		glBegin(GL_TRIANGLES);
		glVertex3f(-0.2, 0.2, 0);
		glVertex3f(-0.2, -0.2, 0);
		glVertex3f(0.2, 0, 0);
		glEnd();
	}

	glTranslatef(-2, 0, 0);

	if (select_coins_flag != RED_COINS)
	{
		//left triangle
		glBegin(GL_TRIANGLES);
		glVertex3f(0.2, 0.2, 0);
		glVertex3f(0.2, -0.2, 0);
		glVertex3f(-0.2, 0, 0);
		glEnd();
	}
	glTranslatef(1, -0.5, 0);
	glScalef(0.4, 0.2, 1);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex3f(-1, 0, 1);
	glVertex3f(-1, 2, 1);
	glVertex3f(1, 2, 1);
	glVertex3f(1, 0, 1);
	glEnd();
	glPopMatrix();
	drawBitmapText("Back", -0.6, -2.3, 5);
}
void Pause_game_button() {

	//pause game button that appears in the top right of the screen to pause the game when clicked
	glPushMatrix();
	
	glTranslatef(4.2, 9.2, 0);
	//pause game
	glBegin(GL_QUADS);
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 1);
	glVertex3f(0, 0.8, 1);
	glVertex3f(0.8, 0.8, 1);
	glVertex3f(0.8, 0, 1);
	glEnd();

	if (playing_menu_flag == PAUSE)
	{
		glTranslatef(0.3, 0.24, 0);
		glColor3f(0, 0, 0);
		//right triangle
		glBegin(GL_TRIANGLES);
		glVertex3f(-0.2, 0.2, 1.2);
		glVertex3f(-0.2, -0.2, 1.2);
		glVertex3f(0.2, 0, 1.2);
		glEnd();
		//startflag = GO;
	}
	else
	{
		glColor3f(0, 0, 0);
		glTranslatef(0.13, 0.1, 0);
		glScalef(0.14, 0.4, 1);
		glBegin(GL_QUADS);
		glNormal3f(0, 0, 1);
		glVertex3f(0, 0, 1.2);
		glVertex3f(0, 0.8, 1.2);
		glVertex3f(0.8, 0.8, 1.2);
		glVertex3f(0.8, 0, 1.2);
		glEnd();

		glTranslatef(1.5, 0, 0);
		
		glBegin(GL_QUADS);
		
		glNormal3f(0, 0, 1);
		glVertex3f(0, 0, 1.2);
		glVertex3f(0, 0.8, 1.2);
		glVertex3f(0.8, 0.8, 1.2);
		glVertex3f(0.8, 0, 1.2);
		glEnd();

		//startflag = PAUSE;
		
	}
	
	glPopMatrix();
}
void Pause_game() {
	//pause game menu, appears when pause game button is clicked
	glPushMatrix();
	glScalef(3, 1, 1);
	glTranslatef(-0.5, 6, 0);
	//Resume
	
	glBegin(GL_QUADS);
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 1);
	glVertex3f(0, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 0, 1);
	glEnd();
	//back to menu
	
	glTranslatef(0, -1.1, 0);
	glBegin(GL_QUADS);
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 1);
	glVertex3f(0, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 0, 1);
	glEnd();
	//exit game
	
	glTranslatef(0, -1.1, 0);
	glBegin(GL_QUADS);
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 1);
	glVertex3f(0, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 0, 1);
	glEnd();
	glPopMatrix();

	glColor3f(1, 1, 1);
	drawBitmapText("Resume", -0.5, 6.2, 2);
	drawBitmapText("Menu", -0.5, 5.2, 2);
	drawBitmapText("Exit", -0.5, 4.2, 2);
}
void high_score() {
	//high score screen
	glPushMatrix();
	glTranslatef(-3, -2, 0);
	glScalef(5, 7, 1);
	glBegin(GL_QUADS);
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 1);
	glVertex3f(0, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 0, 1);
	glEnd();
	glPopMatrix();

	//back button
	glTranslatef(-0.6, -3, 0);
	glBegin(GL_QUADS);
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(-0.8, 0, 1);
	glVertex3f(-0.8, 0.8, 1);
	glVertex3f(0.8, 0.8, 1);
	glVertex3f(0.8, 0, 1);
	glEnd();
	glTranslatef(0.6, 3, 0);

	//set and draw the texts
	drawBitmapText("HIGH SCORE", -1, 4.2, 5);
	drawBitmapText("Back", -0.6, -0.6, 5);

	//open the file
	FILE *file = fopen("highscore.txt", "r");
	char read_line[20];
	char number[5];
	float y = 2.9;
	drawBitmapText("Rank    Score", -1.6, 3.3, 5);
	
	//read from the file to a string and draw it on the screen
	glColor3f(0, 0, 0);
	for (int i = 0;i < 10;i++, y-=0.3)
	{
		sprintf(number, "%d. ", i+1);
		fgets(read_line, 20, file);
		if (i == new_score_position)
		{
			
			glColor3f(1.0, 1.0, 1.0);

		}
		drawBitmapText(number, -1.6, y, 5);
		drawBitmapText(read_line, -0.8, y, 5);
		glColor3f(0, 0, 0);
	}
	fclose(file);
}
void exit_button()
{
	//draw exit button. to exit when clicked
	glPushMatrix();

	glTranslatef(9, 11.5, 0);
	glScalef(1.3, 1.3, 1);
	glBegin(GL_QUADS);
	glColor3f(0.3, 0.5, 0.4);
	//exit button
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 1);
	glVertex3f(0, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 0, 1);
	glEnd();

	drawBitmapText("X", 0.2, 0.2, 1.1);
	glPopMatrix();
	
	
}
void player_lose_menu (){
	//the menu that the player gets when he crashes on an obstacle

	glPushMatrix();
	glScalef(3, 1, 1);
	glTranslatef(-0.5, 6, 0);
	//continue 
	glBegin(GL_QUADS);
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 1);
	glVertex3f(0, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 0, 1);
	glEnd();
	//play new
	glTranslatef(0, -1.1, 0);
	glBegin(GL_QUADS);
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 1);
	glVertex3f(0, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 0, 1);
	glEnd();
	//menu 
	glTranslatef(0, -1.1, 0);
	glBegin(GL_QUADS);
	glColor3f(0.3, 0.5, 0.4);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 1);
	glVertex3f(0, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 0, 1);
	glEnd();
	glPopMatrix();

	glColor3f(1, 1, 1);
	drawBitmapText("continue", -0.5, 6.2, 2);
	drawBitmapText("new game", -0.8, 5.2, 2);
	drawBitmapText("Menu", -0.5, 4.2, 2);
}

//game

//triangle area calculation
float Triangle_area(int x1, int y1, int x2, int y2, int x3, int y3)
{
	//returns the triangle area
	return (abs(x1*(y2 - y3) + x2*(y3 - y1) + x3*(y1 - y2)) / 2.0);
}
bool isInside(int x1, int y1, int x2, int y2, int x3, int y3, int x, int y)
{
	//function that return true if the click was inside the triangle or false if not
	/* Calculate area of triangle ABC */
	float A = Triangle_area(x1, y1, x2, y2, x3, y3);

	/* Calculate area of triangle PBC */
	float A1 = Triangle_area(x, y, x2, y2, x3, y3);

	/* Calculate area of triangle PAC */
	float A2 = Triangle_area(x1, y1, x, y, x3, y3);

	/* Calculate area of triangle PAB */
	float A3 = Triangle_area(x1, y1, x2, y2, x, y);

	/* Check if sum of A1, A2 and A3 is same as A */
	return (A == A1 + A2 + A3);
}

//text rending 
void drawBitmapText(char *string, float x, float y, float z)
{
	//function that gets a string and position and draws on the screen
	char *c;
	c = string;
	glRasterPos3f(x, y, z);
	do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c); while (*(++c));
}

//play setting
void draw_player() {
	if (players[JACK].status == PLAYER_ON)
		Jack();
	else if (players[ROBOT].status == PLAYER_ON)
		Robot();
}
void mouse(int btn, int state, int x, int y)
{
	if (menu_flag == MENU)
	{
		//if play button was clicked, switch to the game and start the timer
		if (btn == GLUT_LEFT_BUTTON && (x > 217 && x < 380) && (y > 191 && y < 244) && state == GLUT_DOWN)
		{
			menu_flag = PLAY;
			playing_menu_flag = GO;
			game_started_flag = 1;
			t1 = clock();
		}
		//switch to highscore screen if clicked
		else if (btn == GLUT_LEFT_BUTTON && (x > 217 && x < 380) && (y > 250 && y < 303) && state == GLUT_DOWN)
		{
			menu_flag = HIGHSCORE;
		}
		//switch to options screen if clicked
		else if (btn == GLUT_LEFT_BUTTON && (x > 217 && x < 380) && (y > 312 && y < 364) && state == GLUT_DOWN)
		{
			menu_flag = OPTIONS;
		}
		//exit the whole game if clicked
		else if (btn == GLUT_LEFT_BUTTON && (x > 546 && x < 580) && (y > 5 && y < 37) && state == GLUT_DOWN)
		{
			exit(1);
		}
	}
	else if (menu_flag == OPTIONS)
	{
		if (options_flag == DEFAULT_OPTIONS)
		{
			//switch to select player screen if clicked
			if (btn == GLUT_LEFT_BUTTON && (x > 217 && x < 380) && (y > 191 && y < 244) && state == GLUT_DOWN)
			{
				options_flag = SELECT_PLAYER;
			}
			//switch to select coins screen if clicked
			else if (btn == GLUT_LEFT_BUTTON && (x > 217 && x < 380) && (y > 250 && y < 303) && state == GLUT_DOWN)
			{
				options_flag = SELECT_COINS;
			}
			//switch to back to menu screen if clicked
			else if (btn == GLUT_LEFT_BUTTON && (x > 217 && x < 380) && (y > 312 && y < 364) && state == GLUT_DOWN)
			{
				menu_flag = MENU;
			}
		}
		// inside the optios - if select player was choosen
		else if (options_flag == SELECT_COINS)
		{
			//right triangle click
			if (btn == GLUT_LEFT_BUTTON && isInside(354, 406, 354, 431, 379, 419, x, y) && state == GLUT_DOWN)
			{
				if (select_coins_flag == RED_COINS)
					select_coins_flag = GREEN_COINS;
				else if (select_coins_flag == GREEN_COINS)
					select_coins_flag = BLUE_COINS;
				else if (select_coins_flag == BLUE_COINS)
					select_coins_flag = YELLOW_COINS;
			}
			//left triangle click
			else if (btn == GLUT_LEFT_BUTTON && isInside(244, 407, 219, 418, 244, 430, x, y) && state == GLUT_DOWN)
			{
				if (select_coins_flag == YELLOW_COINS)
					select_coins_flag = BLUE_COINS;
				else if (select_coins_flag == BLUE_COINS)
					select_coins_flag = GREEN_COINS;
				else if (select_coins_flag == GREEN_COINS)
					select_coins_flag = RED_COINS;
			}
			//back button click
			else if (btn == GLUT_LEFT_BUTTON && (x>274 && x< 325) && (y>427 && y<450) && state == GLUT_DOWN)
			{
				options_flag = DEFAULT_OPTIONS;
			}
		}
		else if (options_flag == SELECT_PLAYER)
		{
			//right triangle click
			if (btn == GLUT_LEFT_BUTTON && isInside(351, 404, 351, 425, 374, 416, x, y) && state == GLUT_DOWN)
			{
				if (selected_player == JACK && game_started_flag == 0)
				{
					selected_player = ROBOT;
					players[ROBOT].status = PLAYER_ON;
					players[JACK].status = PLAYER_OFF;
				}
				else if (selected_player == ROBOT && game_started_flag == 0)
				{
					selected_player = JACK;
					players[ROBOT].status = PLAYER_OFF;
					players[JACK].status = PLAYER_ON;
				}
			}
			//left triangle click
			else if (btn == GLUT_LEFT_BUTTON && isInside(247, 404, 247, 425, 226, 416, x, y) && state == GLUT_DOWN)
			{
				if (selected_player == JACK && game_started_flag == 0)
				{
					selected_player = ROBOT;
					players[ROBOT].status = PLAYER_ON;
					players[JACK].status = PLAYER_OFF;
				}
				else if (selected_player == ROBOT && game_started_flag == 0)
				{
					selected_player = JACK;
					players[ROBOT].status = PLAYER_OFF;
					players[JACK].status = PLAYER_ON;
				}
			}
			//back button click
			else if (btn == GLUT_LEFT_BUTTON && (x>271 && x< 327) && (y>440 && y<466) && state == GLUT_DOWN)
			{
				options_flag = DEFAULT_OPTIONS;
			}
		}
	}
	else if (menu_flag == PLAY)
	{
		// PAUSE button click
		if (btn == GLUT_LEFT_BUTTON && (x > 542 && x < 587) && (y > 12 && y < 56) && state == GLUT_DOWN)
		{
			if (playing_menu_flag == GO)
				playing_menu_flag = PAUSE;
			else if (playing_menu_flag == PAUSE)
				playing_menu_flag = GO;
				
			
		}
		if (playing_menu_flag == PAUSE)
		{ 
			// resume button click
			if (btn == GLUT_LEFT_BUTTON && (x > 213 && x < 385) && (y > 185 && y < 241) && state == GLUT_DOWN)
			{
				t1 = clock(); // set the timer again after resume
				
				playing_menu_flag = GO;
			}
			// menu button click
			else if (btn == GLUT_LEFT_BUTTON && (x > 213 && x < 385) && (y > 248 && y < 305) && state == GLUT_DOWN)
				menu_flag = MENU;
			// exit button click
			else if (btn == GLUT_LEFT_BUTTON && (x > 213 && x < 385) && (y > 313 && y < 367) && state == GLUT_DOWN)
				exit(1);
		}
		if (playing_menu_flag == HIT)
		{ 
			// continue button click
			if (btn == GLUT_LEFT_BUTTON && (x > 213 && x < 385) && (y > 185 && y < 241) && state == GLUT_DOWN)
			{
				t1 = clock(); // set the timer again after resume
				startflag=GO;
				playing_menu_flag = GO;
			}
			// play new button click
			if (btn == GLUT_LEFT_BUTTON && (x > 213 && x < 385) && (y > 248 && y < 305) && state == GLUT_DOWN)
			{
				ResetGame();
				t1 = clock(); // set the timer again after resume
				playing_menu_flag = GO;

			}
			// menu button click
			else if (btn == GLUT_LEFT_BUTTON && (x > 213 && x < 385) && (y > 313 && y < 367) && state == GLUT_DOWN)
				menu_flag = MENU;

		}
	}
	else if (menu_flag == HIGHSCORE)
	{
		//back button click
		if (btn == GLUT_LEFT_BUTTON && (x > 219 && x < 310) && (y > 542 && y < 587) && state == GLUT_DOWN)
			menu_flag = MENU;
	}
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		printf("%d,%d ", x, y);
	}
	glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y)
{
	if (key == 27) exit(1);
	/*if (key == 'f')
		if (menu_flag == PLAY)
			startflag = GO;
	
	if (key == 'p')
		startflag = PAUSE;
		*/
	if (key == '8')
		if (Jump_status == WALKING)
			Jump_status = JUMPING_UP;
	if (key == '4') { //turn left key
		//path change in the floor if the player still havn't reach the end of the floor
		if (floor_count < (Floor_length - 0.5)) {
			if (LFloorFlag > 0) {
				players[selected_player].x -= 1.2;
				LFloorFlag--;
				RFloorFlag++;
			}
		}
		//player went left on the junction
		else
		{
			//if the junction has no left option the player loses
			if (Jflag == R_JUNCTION)
			{
				check_junction_flag = OBSTACLE_HIT;
				Game_Over();
			}
			else
			{
				//save the current z of the player to draw and start new floor from that position
				players[selected_player].z -= ((Floor_length + 2.5) - floor_count);
				fz = players[selected_player].z;
				floor_count = 0;
				initRandom();
				rot = -90;
				if (Jflag == R_JUNCTION)
					rot = 90;
				if (Jflag == T_JUNCTION)
					rot = 0;
			}
		}
	}
	if (key == '6') { // turn right key
		//path change in the floor if the player still havn't reach the end of the floor
		if (floor_count < (Floor_length - 2.5)) {
			if (RFloorFlag > 0) {
				players[selected_player].x += 1.2;
				RFloorFlag--;
				LFloorFlag++;
			}
		}
		//player went right on the junction
		else
		{
			//if the junction has no right option the player loses
			if (Jflag == L_JUNCTION)
			{
				check_junction_flag = OBSTACLE_HIT;
				Game_Over();
			}
			else
			{
				//save the current z of the player to draw and start new floor from that position
				players[selected_player].z -= ((Floor_length + 2.5) - floor_count);
				fz = players[selected_player].z;
				floor_count = 0;
				initRandom();
				rot = 90;
				//initTexture();
				if (Jflag == L_JUNCTION)
				{
					rot = -90;
				}
				if (Jflag == T_JUNCTION)
					rot = 0;
			}
		}
	}
	//bend if 2 was pressed
	if (key == '2') {
		bend_status = BEND_DOWN;
		Player_MovementStatus = STANDING;
	}
	glutPostRedisplay();
}
void draw()
{
	//printf("%d \n", playing_menu_flag);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	loadTexture(); // load sky texture
	glFlush(); // empty texture memory

	if (menu_flag == PLAY)
	{	
			glTranslatef(0, -5, -10); //initial situation
			
			// pause/exit button
			Pause_game_button();

			if (playing_menu_flag == PAUSE)
			{
				temp_time = t2; // save the time when pause button was clicked
				startflag = PAUSE; // pause the game
				Pause_game(); // pause window	
			}
			else if (playing_menu_flag == HIT) {
				temp_time = t2; //save the time after game is paused
				startflag = PAUSE; // pause the game
				player_lose_menu(); //draw the menu
			}
			else if (playing_menu_flag == GO) {
				startflag = GO;
				t2 = (clock() - t1) / 1000 + temp_time; // time running
				min = t2 / 60;
				sec = t2 % 60;
				sprintf(timer_str, "time %d : %d", min, sec);
			}
			
			//rending score text
			sprintf(score_str, "%d", score); // show player's current score
			drawBitmapText("score: ", -5.7, 10, 0);
			drawBitmapText(score_str, -4.5, 10, 0);
			drawBitmapText(timer_str, -2.5, 10, 0); //timer text 
			drawBitmapText("life :", 0.5, 10, 0); //timer text 
			sprintf(life_str, "%d", life + 1);
			drawBitmapText(life_str, 1.4, 10, 0);

			glTranslatef(-players[selected_player].x, -players[selected_player].y, -players[selected_player].z); //movement in the world

			glTranslatef(players[selected_player].x, players[selected_player].y + 0.8, players[selected_player].z - 1);//player control
				glRotatef(bend_angle, 1, 0, 0); // player bend
					draw_player();
				glRotatef(-bend_angle, 1, 0, 0);
			glTranslatef(-players[selected_player].x, (-players[selected_player].y) - 0.8, -players[selected_player].z);

			glTranslatef(0, 0, fz); // new junction by the existing junction x,z
				glRotatef(rot, 0, 1, 0); // junction rotation
					randJunc();
				glRotatef(-rot, 0, 1, 0);
			glTranslatef(0, 0, -fz);

			if (startflag == GO)
				//increase score in each iteration by 1
				score++;
	}
	//menu window
	else if (menu_flag == MENU)
	{
		glTranslatef(0, -2, -20); //initial situation
		
		exit_button();
		menu();
	}
	//options window
	else if (menu_flag == OPTIONS)
	{
		glTranslatef(0, -2, -20); //initial situation
		switch (options_flag)
		{
		case SELECT_PLAYER: 
			options_selectPlayer();
			break;
		case SELECT_COINS: 
			options_selectCoins();
			break;
		case DEFAULT_OPTIONS:
			options();

			break;
		}
		
	}
	//highscore window
	else if (menu_flag == HIGHSCORE) {
		glTranslatef(0, -2, -10); //initial situation
		high_score();
	}

	if (floor_count > (Floor_length + 4.7)) // lost
	{
		obstacles[MISSING_FLOOR].Obstacle_Hit = OBSTACLE_HIT; //when the player passes the floor end, he falls, just as the missing floor animation
	}
	glColor3f(1.0, 1.0, 1.0);

	glutSwapBuffers();			// display the output
}
void PlayerMovement(float v)
{
	Jump();
	bend();
	Bodymovement();
	if (playing_menu_flag != GAME_FINISHED_NO_MENU)
	{
		players[selected_player].z -= v;
		floor_count += v;
	}
}
void Idle()
{
	if (menu_flag == PLAY)
	{
		//set the velocity of the player by his current floor length and type
		//the velocity changes relatevily by how heavy the floor is 
		if (startflag == GO)
			if (Floor_length == 15)
				if (temp_Floor_Num == SOLID)
					PlayerMovement(floors[Floor_Num].velocity * 0.65 + 0.03);
				else PlayerMovement(floors[Floor_Num].velocity * 0.65 + 0.01);
			else if (Floor_length == 30)
				if (temp_Floor_Num == SOLID)
					PlayerMovement(floors[Floor_Num].velocity * 0.85 + 0.03);
				else PlayerMovement(floors[Floor_Num].velocity * 0.85 + 0.01);
			else if (Floor_length == 45)
				if (temp_Floor_Num == SOLID)
					PlayerMovement(floors[Floor_Num].velocity + 0.03);
				else PlayerMovement(floors[Floor_Num].velocity + 0.01);

			if (Blade_start_flag == BLADE_START)
				Blade_movement();
			Punch_movement();
			Check_if_crashed();
	}
	draw();
}

// Set OpenGL parameters
void ResetGame() {

	 life = 2; 

	 Floor_Num = 0;
	 temp_Floor_Num = 0;

	 bend_angle = 0;
	 bend_status = STANDING;
	 SkateBoard_flag = SKATEBOARD_OFF; // skateboard status flag
	 skipObstacle_flag = SKIPOBSTACLE_OFF; // skateboard status flag
	 skipObstacle_angle = 0;


	  RFloorFlag = 1, LFloorFlag = 1; // RF=right floor  LF=left floor
	  Turnflag = 2; // 0-went right  1- went left  2-defualt, straight  3-opposite z
	  Junction_Flag = T_JUNCTION;
	  JunctionTurnFlag = 0; // 1=right on the junction,  0= left on the junction
	  Jflag = 0; // 0-TJunction, 1-RJunction 2-LJunction
	  
	 selected_player = JACK;
	 Jump_status = 0; // movement status 0-walking, 1-jump up, 2- jump down
	 startflag = 0; // start flag  'f'=1=start 'p'=0=stop
	 rot = 0; // rotation angle
	 floor_count = 0; //position in the floor
	 fx = 0.0, fy = 0.5, fz = 0;
	 score = 0;

	 temp_time = 0;
	 strcpy(timer_str, "");
	 min=0;
	 sec=0;
     playing_menu_flag = GO;
	 game_started_flag = 0;

	initPlayer();
	initCoins();
	initFloor();
	initObstacles();
	initRandom();

	t1 = time(NULL);
}
void setting_new_game() {

		players[selected_player].x = 0;
	    players[selected_player].y = 0.5;
	    players[selected_player].z = 0;
		
	 bend_angle = 0;
	 bend_status = STANDING;
	 SkateBoard_flag = SKATEBOARD_OFF; // skateboard status flag
	 skipObstacle_flag = SKIPOBSTACLE_OFF; // skateboard status flag
	 skipObstacle_angle = 0;


	  RFloorFlag = 1, LFloorFlag = 1; // RF=right floor  LF=left floor
	
	  
	 Jump_status = 0; // movement status 0-walking, 1-jump up, 2- jump down
	 
	 floor_count = 0; //position in the floor
	 //fx = 0.0, fy = 0.5;
	 fz = 0;
	
	t1 = time(NULL);

}
void initTexture() {
	
	//make sky pic
	glGenTextures(2, images_arr);
	glBindTexture(GL_TEXTURE_2D, images_arr[0]);
	glBindTexture(GL_TEXTURE_2D, images_arr[1]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

}
void init()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1.0, 0.1, 60);
	glMatrixMode(GL_MODELVIEW);

	// Lighting parameters

	GLfloat mat_ambdif[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 0.0, 1.0, 0.0, 0.0 };
	GLfloat mat_shininess[] = { 80.0 };
	GLfloat light_position[] = { 0.0, 1.0, 2.0, 0.0 };
	
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

}
void initFloorColor()
{
	FloorColor[0].red = 0.804;
	FloorColor[0].green = 0.522;
	FloorColor[0].blue = 0.247;

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
	if (Floor_Num == SOLID) // so there can't be 2 solid floors, one by another
		switch (randnum(0, 1))
		{
		 case 0: temp_Floor_Num = BRIDGE;
			break;
		 case 1: temp_Floor_Num = PIPE;
		}
	else
		temp_Floor_Num = randnum(0, 2);

	///// Generate random junction
	Jflag = randnum(0, 2);
	if (Jflag == R_JUNCTION)
		rot = 90;
	else if (Jflag == L_JUNCTION)
		rot = -90;
	else
		rot = 0;
	
	//the floor after the junction starts with non obstacle
	floors[temp_Floor_Num].obstacles[0] = &obstacles[NONE_OBSTACLE];
	floors[temp_Floor_Num].obstacles[1] = &obstacles[NONE_OBSTACLE];

	////// generate random obstacle by floor length
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
			Obstacle1_flag = randnum(3, 4);

		else if (Floor_Num == SOLID)
			Obstacle1_flag = randnum(5, 5);


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
			Obstacle1_flag = randnum(3, 4);
			Obstacle2_flag = randnum(3, 4);
		}
		else if (Floor_Num == SOLID)
		{
			Obstacle1_flag = randnum(5, 5);
			Obstacle2_flag = randnum(5, 5);
		}
		floors[Floor_Num].obstacles[0] = &obstacles[Obstacle1_flag];
		floors[Floor_Num].obstacles[1] = &obstacles[Obstacle2_flag];
	}
	////generate random FIRST obstacle pos in floor
	for (int i = 0; i < 2;i++)
	{
		if (floors[Floor_Num].obstacles[i]->pos_in_floor_options == OBSTACLE_POSITION_ONLY_RIGHT_AND_LEFT)
		{
			switch (randnum(0, 1))
			{
			case 0:floors[Floor_Num].obstacles[i]->pos_in_floor = OBSTACLE_IN_RIGHT;
				switch (randnum(0, 1))
				{
				case 0:coins[BONUS_COINS].coin_pos[i] = COINS_IN_LEFT;
					break;
				case 1:coins[BONUS_COINS].coin_pos[i] = COINS_IN_CENTER;
					break;
				}
				break;
			case 1: floors[Floor_Num].obstacles[i]->pos_in_floor = OBSTACLE_IN_LEFT;
				switch (randnum(0, 1))
				{
					coins[BONUS_COINS].coin_pos[i] = COINS_IN_RIGHT;
					break;
				case 1:coins[BONUS_COINS].coin_pos[i] = COINS_IN_CENTER;
					break;
				}
				break;
			}
		}
		else if (floors[Floor_Num].obstacles[i]->pos_in_floor_options == OBSTACLE_POSITION_THREE_OPTIONS)
		{
			switch (randnum(0, 2))
			{
			case 0:floors[Floor_Num].obstacles[i]->pos_in_floor = OBSTACLE_IN_RIGHT;
				switch (randnum(0, 1))
				{
				case 0:coins[BONUS_COINS].coin_pos[i] = COINS_IN_LEFT;
					break;
				case 1:coins[BONUS_COINS].coin_pos[i] = COINS_IN_CENTER;
					break;
				}break;
			case 1: floors[Floor_Num].obstacles[i]->pos_in_floor = OBSTACLE_IN_LEFT;
				switch (randnum(0, 1))
				{
					coins[BONUS_COINS].coin_pos[i] = COINS_IN_RIGHT;
					break;
				case 1:coins[BONUS_COINS].coin_pos[i] = COINS_IN_CENTER;
					break;
				}
				break;
			case 2: floors[Floor_Num].obstacles[i]->pos_in_floor = OBSTACLE_IN_CENTER;
				switch (randnum(0, 1))
				{
				case 0:coins[BONUS_COINS].coin_pos[i] = COINS_IN_LEFT;
					break;
				case 1:coins[BONUS_COINS].coin_pos[i] = COINS_IN_RIGHT;
					break;
				}
				break;
			}
		}
		else if (floors[Floor_Num].obstacles[i]->pos_in_floor_options == OBSTACLE_POSITION_ONE_OPTION)
			floors[Floor_Num].obstacles[i]->pos_in_floor = OBSTACLE_IN_CENTER;
			switch (randnum(0, 1))
			{
			case 0:coins[BONUS_COINS].coin_pos[0] = COINS_IN_LEFT;
				break;
			case 1:coins[BONUS_COINS].coin_pos[0] = COINS_IN_RIGHT;
				break;
			}
	}

	//set bonus coins quantity to put in floor
	coins[BONUS_COINS].quantity = Floor_length * 2 / 3;

	//allocate memory for bonus position in the floor so we can take them off if the player hit a coin
	coins[BONUS_COINS].pos_in_floor = (int *)malloc(sizeof(int) * (Floor_length * 2 / 3));
	if (coins[BONUS_COINS].pos_in_floor == NULL)
		exit(1);
	//set the position of each floor by the new array size
	coins[BONUS_COINS].pos_in_floor[0] = (Floor_length / 3);
	for (int i = 1; i < coins[BONUS_COINS].quantity; i++)
		coins[BONUS_COINS].pos_in_floor[i] = coins[BONUS_COINS].pos_in_floor[i-1] + 1;
	

	//generate random position for the bonus coin - chances are 1/6 to get a coin
	if (skipObstacle_flag == SKIPOBSTACLE_OFF && SkateBoard_flag == SKATEBOARD_OFF)
	{
		switch (randnum(0, 6))
		{
		case 3:
			switch (randnum(1, 2)) {
			case 1:
				coins[SKATEBOARD_COIN].coin_pos[0] = coins[BONUS_COINS].coin_pos[0];
				coins[SKATEBOARD_COIN].status = COIN_ON;
				*coins[SKATEBOARD_COIN].pos_in_floor = randnum(7, Floor_length * 2 / 3);
				break;
			case 2:
				coins[SKIP_OBSTACLE_COIN].coin_pos[0] = coins[BONUS_COINS].coin_pos[0];
				coins[SKIP_OBSTACLE_COIN].status = COIN_ON;
				*coins[SKIP_OBSTACLE_COIN].pos_in_floor = randnum(7, Floor_length * 2 / 3);
				break;
			}
			break;
		default:  coins[SKATEBOARD_COIN].status = COIN_OFF;
				 coins[SKIP_OBSTACLE_COIN].status = COIN_OFF;
		}
	}

	//duration statments
	if (SkateBoard_flag == SKATEBOARD_ON || skipObstacle_flag == SKIPOBSTACLE_ON)
	{
		if (bonus_duration <= 0)
		{
			if (SkateBoard_flag == SKATEBOARD_ON)
				SkateBoard_flag = SKATEBOARD_OFF;
			else if (skipObstacle_flag == SKIPOBSTACLE_ON)
				skipObstacle_flag = SKIPOBSTACLE_OFF;
			bonus_duration = 2;
		}
        else bonus_duration--;
	}
	
	if(floors[BRIDGE].velocity <= 0.6)
		floors[BRIDGE].velocity += 0.01;
	if (floors[SOLID].velocity <= 0.6)
		floors[SOLID].velocity += 0.01;
	if (floors[PIPE].velocity <= 0.6)
		floors[PIPE].velocity += 0.01;

	glutPostRedisplay();
}
void initObstacles() {
	obstacles[NONE_OBSTACLE].name = NONE_OBSTACLE;
	obstacles[NONE_OBSTACLE].floor = NONE_OBSTACLE;
	obstacles[NONE_OBSTACLE].pos_in_floor_options = OBSTACLE_POSITION_THREE_OPTIONS;
	obstacles[NONE_OBSTACLE].Obstacle_Hit = OBSTACLE_MISS;
	obstacles[NONE_OBSTACLE].GameOver = GAME_OVER_OFF;

	obstacles[DOWN_BLADE].name = DOWN_BLADE;
	obstacles[DOWN_BLADE].floor = PIPE;
	obstacles[DOWN_BLADE].pos_in_floor_options = OBSTACLE_POSITION_THREE_OPTIONS;
	obstacles[DOWN_BLADE].Obstacle_Hit = OBSTACLE_MISS;
	obstacles[DOWN_BLADE].GameOver = GAME_OVER_OFF;

	obstacles[UPPER_BLADE].name = UPPER_BLADE;
	obstacles[UPPER_BLADE].floor = PIPE;
	obstacles[UPPER_BLADE].pos_in_floor_options = OBSTACLE_POSITION_ONLY_RIGHT_AND_LEFT;
	obstacles[UPPER_BLADE].Obstacle_Hit = OBSTACLE_MISS;
	obstacles[UPPER_BLADE].GameOver = GAME_OVER_OFF;

	obstacles[MISSING_FLOOR].name = MISSING_FLOOR;
	obstacles[MISSING_FLOOR].floor = BRIDGE;
	obstacles[MISSING_FLOOR].pos_in_floor_options = OBSTACLE_POSITION_THREE_OPTIONS;
	obstacles[MISSING_FLOOR].Obstacle_Hit = OBSTACLE_MISS;
	obstacles[MISSING_FLOOR].GameOver = GAME_OVER_OFF;

	obstacles[AIRPLANE_CRASH].name = AIRPLANE_CRASH;
	obstacles[AIRPLANE_CRASH].floor = BRIDGE;
	obstacles[AIRPLANE_CRASH].pos_in_floor_options = OBSTACLE_POSITION_ONLY_RIGHT_AND_LEFT;
	obstacles[AIRPLANE_CRASH].Obstacle_Hit = OBSTACLE_MISS;
	obstacles[AIRPLANE_CRASH].GameOver = GAME_OVER_OFF;

	obstacles[PUNCH].name = PUNCH;
	obstacles[PUNCH].floor = SOLID;

	obstacles[PUNCH].pos_in_floor_options = OBSTACLE_POSITION_ONLY_RIGHT_AND_LEFT;
	obstacles[PUNCH].Obstacle_Hit = OBSTACLE_MISS;
	obstacles[PUNCH].GameOver = GAME_OVER_OFF;
	
}
void initCoins() {
	coins[BONUS_COINS].name = BONUS_COINS;
	coins[BONUS_COINS].status = COIN_OFF;

	coins[SKATEBOARD_COIN].name = SKATEBOARD_COIN;
	coins[SKATEBOARD_COIN].status = COIN_OFF;
	coins[SKATEBOARD_COIN].quantity = 1;

	coins[SKIP_OBSTACLE_COIN].name = SKIP_OBSTACLE_COIN;
	coins[SKIP_OBSTACLE_COIN].status = COIN_OFF;
	coins[SKIP_OBSTACLE_COIN].quantity = 1;

	//only one coin for the skateboard and skip obstacles coins
	coins[SKATEBOARD_COIN].pos_in_floor = (int *)malloc(sizeof(int) * 1);
	coins[SKIP_OBSTACLE_COIN].pos_in_floor = (int *)malloc(sizeof(int) * 1);

}
void initFloor()
{
	initFloorColor();
	initObstacles();
	initCoins();

	floors[0].FloorColor.red = FloorColor[0].red;
	floors[0].FloorColor.green = FloorColor[0].green;
	floors[0].FloorColor.blue = FloorColor[0].blue;
	floors[0].type = BRIDGE;
	floors[0].velocity = 0.08;
	floors[0].obstacles_pos[0] = OBSTACLE1_POSITION;
	floors[0].obstacles_pos[1] = OBSTACLE2_POSITION;

	floors[1].FloorColor.red = FloorColor[1].red;
	floors[1].FloorColor.green = FloorColor[1].green;
	floors[1].FloorColor.blue = FloorColor[1].blue;
	floors[1].type = SOLID;
	floors[1].velocity = 0.09;
	floors[1].obstacles_pos[0] = OBSTACLE1_POSITION;
	floors[1].obstacles_pos[1] = OBSTACLE2_POSITION;

	floors[2].FloorColor.red = FloorColor[2].red;
	floors[2].FloorColor.green = FloorColor[2].green;
	floors[2].FloorColor.blue = FloorColor[2].blue;
	floors[2].type = PIPE;
	floors[2].velocity = 0.075;
	floors[2].obstacles_pos[0] = OBSTACLE1_POSITION * 1.1;
	floors[2].obstacles_pos[1] = OBSTACLE2_POSITION * 1.1;
}
void initPlayer()
{
	players[JACK].name = JACK;
	players[JACK].status = PLAYER_ON;
	players[JACK].x = 0;
	players[JACK].y = 0.5;
	players[JACK].z = 0;

	players[ROBOT].name = JACK;
	players[ROBOT].status = PLAYER_OFF;
	players[ROBOT].x = 0;
	players[ROBOT].y = 0.5;
	players[ROBOT].z = 0;
}
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);	// RGB display, double-buffered, with Z-Buffer
	glutInitWindowSize(600, 600);					// 500 x 500 pixels
	glutCreateWindow("3D");
	init();
	initFloor();
	initPlayer();
	initRandom();
	glutDisplayFunc(draw);						// Set the display function
	glutKeyboardFunc(keyboard);					// Set the keyboard function
	glutMouseFunc(mouse);
	glutIdleFunc(Idle);

	glutMainLoop();							// Start the main event loop
}