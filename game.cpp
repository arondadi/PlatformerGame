#include "game_math.h"

#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

/*
TOTO impliment GetTileValue for collision(position as param and check what tile is there)
TODO: Dash mechanic -> press a button to dash quickly a fixed distance and cool down timer on that mechanic. Opptional: Genji super dash off edges
TODO: Gamepad support in platform layer
TODO: Add absolute oordinates to position the playing field in the center of window
TODO: check and implement tips: https://www.youtube.com/watch?v=vFsJIrm2btU
*/

/* DONE
Impliment Tilemap map
*/


struct graphics_entity
{
	float half_size_x;
	float half_size_y;
};


struct physics_entity
{
	v2 P;
	v2 dP;
	v2 ddP;
};


bool initilized = false;

//float gravity = -120.f;
float gravity = 0;

bool grounded = true;
bool attached = false;

float arena_half_size_x = 85, arena_half_size_y = 45;

graphics_entity PlayerGraphics;
physics_entity PlayerPhysics;

// Test platform
float test_x = 0;
float test_y = -20;
float test_half_size_x = 30;
float test_half_size_y = 12;

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

		PlayerGraphics.half_size_x = 16;
		PlayerGraphics.half_size_y = 16;

		PlayerPhysics.P.X = 16;
		PlayerPhysics.P.Y = 9;
		PlayerPhysics.dP.X = 0;
		PlayerPhysics.dP.Y = 0;
		PlayerPhysics.ddP.X = 0;
		PlayerPhysics.ddP.Y = 0;
	}


#if 1
	PlayerPhysics.ddP.X = 0.f;
	PlayerPhysics.ddP.Y = 0.f;

	// physics_entity to move the player with new input values
	physics_entity NewPlayerPhysics = PlayerPhysics;

	// Control input
	if (is_down(BUTTON_LEFT)) NewPlayerPhysics.ddP.X -= 300;
	if (is_down(BUTTON_RIGHT)) NewPlayerPhysics.ddP.X += 300;

	//TODO: Remove double jump after attached 
	if (grounded || attached)
	{
		if (pressed(BUTTON_SPACE))
		{
			NewPlayerPhysics.dP.Y += 85;
			grounded = false;
			attached = false;
		}
	}

	// Friction
	NewPlayerPhysics.ddP.X -= NewPlayerPhysics.dP.X * 5.f;
	
	NewPlayerPhysics.dP.Y = NewPlayerPhysics.dP.Y + gravity * dt;
	NewPlayerPhysics.P.Y = NewPlayerPhysics.P.Y + NewPlayerPhysics.dP.Y * dt;

	NewPlayerPhysics.dP.X = NewPlayerPhysics.dP.X + NewPlayerPhysics.ddP.X * dt;
	NewPlayerPhysics.P.X = NewPlayerPhysics.P.X + NewPlayerPhysics.dP.X * dt + NewPlayerPhysics.ddP.X * dt * dt;


	// Collision with platform
	if (aabb_collision(NewPlayerPhysics.P.X, NewPlayerPhysics.P.Y, PlayerGraphics.half_size_x, PlayerGraphics.half_size_y, test_x, test_y, test_half_size_x, test_half_size_y))
	{

		/*
		Conserve momentum

		Check for collision aabb by checking (x,y) and size of player and object
		Check players next postition v2_0

		Find length of vector up to collision object v2_1
		Rest of vector length is moved in direction not blocked by collision object v2_2
		v2_1+v2_2 = v2_3

		*/

		if ((PlayerPhysics.P.Y + PlayerGraphics.half_size_y > test_y + test_half_size_y) && (NewPlayerPhysics.dP.Y < 0))
		{
			NewPlayerPhysics.P.Y = (test_y + test_half_size_y + PlayerGraphics.half_size_y);
			NewPlayerPhysics.dP.Y = 0;
			grounded = true;
		}
		else if (PlayerPhysics.P.Y + PlayerGraphics.half_size_y < test_y - test_half_size_y)
		{
			NewPlayerPhysics.P.Y = (test_y - test_half_size_y - PlayerGraphics.half_size_y);
			NewPlayerPhysics.dP.Y = 0;
		}
		else if (PlayerPhysics.P.X - PlayerGraphics.half_size_x < test_x - test_half_size_x)
		{
			NewPlayerPhysics.P.X = (test_x - test_half_size_x - PlayerGraphics.half_size_x);
			NewPlayerPhysics.dP.X = 0;
			

			attached = true;
		}
		else if (PlayerPhysics.P.X + PlayerGraphics.half_size_x > test_x + test_half_size_x)
		{
			NewPlayerPhysics.P.X = (test_x + test_half_size_x + PlayerGraphics.half_size_x);
			NewPlayerPhysics.dP.X = 0;
			

			attached = true;
		}
	}

	PlayerPhysics = NewPlayerPhysics;

	// Collision with area
	if (PlayerPhysics.P.X + PlayerGraphics.half_size_x > arena_half_size_x)
	{
		PlayerPhysics.P.X = arena_half_size_x - PlayerGraphics.half_size_x;
		PlayerPhysics.dP.X *= -0.2;
		PlayerPhysics.dP.Y *= 0.5;
		attached = true;
	}
	if (PlayerPhysics.P.X - PlayerGraphics.half_size_x < -arena_half_size_x)
	{
		PlayerPhysics.P.X = -arena_half_size_x + PlayerGraphics.half_size_x;
		PlayerPhysics.dP.X *= -0.2;
		PlayerPhysics.dP.Y *= 0.5;
		attached = true;
	}

	if (PlayerPhysics.P.Y - PlayerGraphics.half_size_y < -arena_half_size_y)
	{
		PlayerPhysics.P.Y = -arena_half_size_y + PlayerGraphics.half_size_y;
		PlayerPhysics.dP.Y = 0;
		grounded = true;
	}	
	
	if (PlayerPhysics.P.Y + PlayerGraphics.half_size_y > arena_half_size_y)
	{
		PlayerPhysics.P.Y = arena_half_size_y - PlayerGraphics.half_size_y;
		PlayerPhysics.dP.Y *= -0.75;
	}

#endif

	u32 TileWidth = 32;
	u32 TileHeight = 32;

	// Clear screen
	clear_screen(0xff5500);

	// The origin is in lower left corner, we draw the map and then flip it before rendering
	u32 TileMapFlipped[TileMapHeight][TileMapWidth] =
	{
		{1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  1, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  0, 0, 0, 0,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
		{1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1}
	};


	u32 TileMap[TileMapHeight][TileMapWidth] = {};
	for (u32 i = 0; i < TileMapHeight; ++i)
	{
		for (u32 j = 0; j < TileMapWidth; ++j)
		{
			TileMap[i][j] = TileMapFlipped[TileMapHeight - i - 1][j];
		}
	}


	// Draw play area
	for (u32 row = 0; row < TileMapHeight; ++row)
	{
		for (u32 col = 0; col < TileMapWidth; ++col)
		{
			u32 TileID = TileMap[row][col];
			u32 color = 0xff5500;

			if (TileID == 1)
			{
				color = 0xffaa33;
			}
			u32 RowTile = row * 32;
			u32 ColTile = col * 32;

			draw_rect(ColTile, RowTile, TileHeight, TileWidth, color);
		}
	}
	



	// Player rect
	//draw_rect(PlayerPhysics.P.X*32, PlayerPhysics.P.Y*32, PlayerGraphics.half_size_x, PlayerGraphics.half_size_y, 0xff2200);
	draw_rect(PlayerPhysics.P.X*32, PlayerPhysics.P.Y*32, 32, 32, 0xff2200);
};