#include "game_math.h"

#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

/*
//TODO: Vector calculations on collision to conserve momentum and move player in right direction depending on angle of collision (up or down walls and edges)
//TODO: Dash mechanic -> press a button to dash quickly a fixed distance and cool down timer on that mechanic. Opptional: Genji super dash off edges
//TODO: Gamepad support in platform layer
//TODO: Impliment Tilemap map
//TOTO impliment GetTileValue for collision(position as param and check what tile is there)
TODO: check and implement tips: https://www.youtube.com/watch?v=vFsJIrm2btU
*/

struct game_entity
{
	float x;
	float y;
	float half_size_x;
	float half_size_y;
	float dx;
	float dy;
	float ddx;
	float ddy;
};


struct physics_entity
{
	v2 P;
	v2 dP;
	v2 ddP;
};


bool initilized = false;

float gravity = -120.f;

bool grounded = true;
bool attached = false;

float arena_half_size_x = 85, arena_half_size_y = 45;

game_entity Player;
physics_entity PlayerPhysics;

// Test platform
float test_x = 0;
float test_y = -20;
float test_half_size_x = 30;
float test_half_size_y = 4;


//TODO: implement player struct and "last-frame-ghost" entity(last few frames..)
// TODO Insert into simulate player:
internal void
simulate_player(float *p, float *dp, float *half_size_x, float *half_size_y, float ddp, float dt)
{
	ddp -= *dp * 10.f;

	*p = *p + *dp * dt + ddp * dt * dt * .5f;
	*dp = *dp + ddp * dt;

	if (*p + *half_size_y > arena_half_size_y) {
		*p = arena_half_size_y - *half_size_y;
		*dp = 0;
	}
	else if (*p - *half_size_y < -arena_half_size_y) {
		*p = -arena_half_size_y + *half_size_y;
		*dp = 0;
	}
}


internal bool
aabb_collision(float x1, float y1, float half_size_x1, float half_size_y1, float x2, float y2, float half_size_x, float half_size_y)
{
	if ((y1 - half_size_y1) < (y2 + half_size_y) &&
		(y1 + half_size_y1) > (y2 - half_size_y) &&
		(x1 + half_size_x1) > (x2 - half_size_x) &&
		(x1 - half_size_x1) < (x2 + half_size_x))
	{
		return true;
	}
	else
	{
		return false;
	}

}


internal void
simulate_game(Input* input, float dt)
{
	if (!initilized)
	{
		initilized = true;

		Player.x = 0;
		Player.y = 0;
		Player.dx = 0;
		Player.dy = 0;
		Player.ddx = 0;
		Player.ddy = 0;
		Player.half_size_x = 2;
		Player.half_size_y = 2;

		PlayerPhysics.P.X = 0;
		PlayerPhysics.P.Y = 0;
		PlayerPhysics.dP.X = 0;
		PlayerPhysics.dP.Y = 0;
		PlayerPhysics.ddP.X = 0;
		PlayerPhysics.ddP.Y = 0;
	}


	Player.ddx = 0.f;
	Player.ddy = 0.f;

	// TODO: make new position and check if it is legal. If not make a legal move. 
	physics_entity NewPlayerPhysics;

	// Control input
	if (is_down(BUTTON_LEFT)) Player.ddx -= 300;
	if (is_down(BUTTON_RIGHT)) Player.ddx += 300;

	if (grounded || attached)
	{
		if (pressed(BUTTON_SPACE))
		{
			Player.dy += 85;
			grounded = false;
			attached = false;
		}
	}

	// Friction
	Player.ddx -= Player.dx * 5.f;
	
	Player.y = Player.y + Player.dy * dt;
	Player.dy = Player.dy + gravity * dt;

	Player.x = Player.x + Player.dx * dt + Player.ddx * dt * dt;
	Player.dx = Player.dx + Player.ddx * dt;


	// Collision with platform
	if (aabb_collision(Player.x, Player.y, Player.half_size_x, Player.half_size_y, test_x, test_y, test_half_size_x, test_half_size_y))
	{

		//TODO: Conserve momentum
		/*
		
		Check for collision aabb by checking (x,y) and size of player and object
		Check players next postition v2_0

		Find length of vector up to collision object v2_1
		Rest of vector length is moved in direction not blocked by collision object v2_2
		v2_1+v2_2 = v2_3

		*/

		if (Player.dy < 0)
		{
			Player.y = -20 + 4 + Player.half_size_y;
			Player.dy = 0;
			grounded = true;
		}
		else if (Player.dy >= 0)
		{
			//TODO: Change for edge cases (climb up on ledges, don't bounce down on edge. Fix with ghost player
			Player.y = -20 - 4 - Player.half_size_x;
			Player.dy *= -0.75;
		}
	}



	// Collision with area
	if (Player.x + Player.half_size_x > arena_half_size_x)
	{
		Player.x = arena_half_size_x - Player.half_size_x;
		Player.dx *= -0.2;
		Player.dy *= 0.5;
		attached = true;
	}
	if (Player.x - Player.half_size_x < -arena_half_size_x)
	{
		Player.x = -arena_half_size_x + Player.half_size_x;
		Player.dx *= -0.2;
		Player.dy *= 0.5;
		attached = true;
	}

	if (Player.y - Player.half_size_y < -arena_half_size_y)
	{
		Player.y = -arena_half_size_y + Player.half_size_y;
		Player.dy = 0;
		grounded = true;
	}	
	
	if (Player.y + Player.half_size_y > arena_half_size_y)
	{
		Player.y = arena_half_size_y - Player.half_size_y;
		Player.dy *= -0.75;
	}


	// Clear screen
	clear_screen(0xff5500);

	// Draw play area
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xffaa33);
	draw_rect(test_x, test_y, test_half_size_x, test_half_size_y, 0xff5500);

	// Player rect
	draw_rect(Player.x, Player.y, Player.half_size_x, Player.half_size_y, 0xff2200);
};