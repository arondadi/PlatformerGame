#include "game_math.h"

#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

/*
TODO: check aabb collsion in tilemapcollision
TODO: Dash mechanic -> press a button to dash quickly a fixed distance and cool down timer on that mechanic. Opptional: Genji super dash off edges
TODO: Gamepad support in platform layer
TODO: Add absolute oordinates to position the playing field in the center of window
TODO: check and implement tips: https://www.youtube.com/watch?v=vFsJIrm2btU
*/

/* DONE
Impliment Tilemap map
GetTileValueFromAbsPos
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

float gravity = -700.f;
float PlayerAcc = 2000.f;
float PlayerJumpVel = 450.f;

bool grounded = true;
bool attached = false;

float arena_half_size_x = 85, arena_half_size_y = 45;

graphics_entity PlayerGraphics;
physics_entity PlayerPhysics;

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
	{1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1},
	{1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1}
};

u32 TileMap[TileMapHeight][TileMapWidth];


#if 0
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

#endif

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

internal v2
GetTileIndex(float x, float y)
{
	v2 Result;

	Result.X = (u32)(x / TileWidth + .5f);
	Result.Y = (u32)(y / TileHeight);

	return Result;
};

internal v2
GetTileAbs(u32 x, u32 y)
{
	v2 Result;

	Result.X = x * TileWidth + .5f * TileWidth;
	Result.Y = y * TileWidth;

	return Result;
}


internal void
simulate_game(Input* input, float dt)
{
	if (!initilized)
	{
		initilized = true;

		PlayerGraphics.half_size_x = 16;
		PlayerGraphics.half_size_y = 16;

		PlayerPhysics.P.X = render_state.width * 0.5f;
		PlayerPhysics.P.Y = render_state.height * 0.25f;
		PlayerPhysics.dP.X = 0;
		PlayerPhysics.dP.Y = 0;
		PlayerPhysics.ddP.X = 0;
		PlayerPhysics.ddP.Y = 0;

		// The origin is in lower left corner, we draw the map and then flip it before rendering

		for (u32 i = 0; i < TileMapHeight; ++i)
		{
			for (u32 j = 0; j < TileMapWidth; ++j)
			{
				TileMap[i][j] = TileMapFlipped[TileMapHeight - i - 1][j];
			}
		}
	}


	PlayerPhysics.ddP.X = 0.f;
	PlayerPhysics.ddP.Y = 0.f;

	// physics_entity to move the player with new input values
	physics_entity NewPlayerPhysics = PlayerPhysics;

	// Control input
	if (is_down(BUTTON_LEFT)) NewPlayerPhysics.ddP.X -= PlayerAcc;
	if (is_down(BUTTON_RIGHT)) NewPlayerPhysics.ddP.X += PlayerAcc;

	//TODO: Remove double jump after attached 
	if (grounded || attached)
	{
		if (pressed(BUTTON_SPACE))
		{
			NewPlayerPhysics.dP.Y += PlayerJumpVel;
			grounded = true;
			attached = false;
		}
	}
	
	// Friction
	NewPlayerPhysics.ddP.X -= NewPlayerPhysics.dP.X * 5.f;
	
	NewPlayerPhysics.dP.Y = NewPlayerPhysics.dP.Y + gravity * dt;
	NewPlayerPhysics.P.Y = NewPlayerPhysics.P.Y + NewPlayerPhysics.dP.Y * dt;

	NewPlayerPhysics.dP.X = NewPlayerPhysics.dP.X + NewPlayerPhysics.ddP.X * dt;
	NewPlayerPhysics.P.X = NewPlayerPhysics.P.X + NewPlayerPhysics.dP.X * dt + NewPlayerPhysics.ddP.X * dt * dt;




	// Collision
	v2 OldPlayerTileIndex = GetTileIndex(PlayerPhysics.P.X, PlayerPhysics.P.Y);
	v2 NewPlayerTileIndex = GetTileIndex(NewPlayerPhysics.P.X, NewPlayerPhysics.P.Y);

	u32 x0 = min(OldPlayerTileIndex.X, NewPlayerTileIndex.X);
	u32 x1 = max(OldPlayerTileIndex.X, NewPlayerTileIndex.X);
	u32 y0 = min(OldPlayerTileIndex.Y, NewPlayerTileIndex.Y);
	u32 y1 = max(OldPlayerTileIndex.Y, NewPlayerTileIndex.Y);


	for (u32 i = x0; i <= x1; ++i)
	{
		for (u32 j = y0; j <= y1; ++j)
		{
			if (TileMap[j][i] == 1)
			{

				v2 TileAbsPos = GetTileAbs(i, j);

 				if (aabb_collision(NewPlayerPhysics.P.X, NewPlayerPhysics.P.Y, PlayerGraphics.half_size_x, PlayerGraphics.half_size_y, TileAbsPos.X ,TileAbsPos.Y , TileWidth, TileHeight))
				{

					//TODO debug collision in -X direction
					// Check GetTileAbs func
					// Check aabb and the collision for loops
					if ((NewPlayerPhysics.P.Y + PlayerGraphics.half_size_y > TileAbsPos.Y + TileHeight) && (NewPlayerPhysics.dP.Y < 0 || !grounded))
					{
						NewPlayerPhysics.P.Y = (TileAbsPos.Y + TileHeight);
						NewPlayerPhysics.dP.Y = 0;
						grounded = true;
					}

					if (NewPlayerPhysics.P.Y + PlayerGraphics.half_size_y < TileAbsPos.Y - TileHeight)
					{
						NewPlayerPhysics.P.Y = (TileAbsPos.Y - TileHeight - PlayerGraphics.half_size_y);
						NewPlayerPhysics.dP.Y = 0;
					}
#if 1
					if ((NewPlayerPhysics.P.X - PlayerGraphics.half_size_x < TileAbsPos.X - TileWidth) && (NewPlayerPhysics.dP.X < 0))
					{
						NewPlayerPhysics.P.X = (TileAbsPos.X - TileWidth - PlayerGraphics.half_size_x);
						NewPlayerPhysics.dP.X = 0;

						attached = true;
					}

					if ((NewPlayerPhysics.P.X + PlayerGraphics.half_size_x > TileAbsPos.X - TileWidth) && (NewPlayerPhysics.dP.X > 0))
					{
						NewPlayerPhysics.P.X = (TileAbsPos.X + TileWidth + PlayerGraphics.half_size_x);
						NewPlayerPhysics.dP.X = 0;

						attached = true;
					}
#endif
				}
			}
		}
	}


#if 0
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



	

	// Collision with area
	if (PlayerPhysics.P.X + PlayerGraphics.half_size_x > 1280)
	{
		PlayerPhysics.P.X = arena_half_size_x - PlayerGraphics.half_size_x;
		PlayerPhysics.dP.X *= -0.2;
		PlayerPhysics.dP.Y *= 0.5;
		attached = true;
	}
	if (PlayerPhysics.P.X - PlayerGraphics.half_size_x < 0)
	{
		PlayerPhysics.P.X = -arena_half_size_x + PlayerGraphics.half_size_x;
		PlayerPhysics.dP.X *= -0.2;
		PlayerPhysics.dP.Y *= 0.5;
		attached = true;
	}

	if (PlayerPhysics.P.Y - (.5f*PlayerGraphics.half_size_y) < 48)
	{
		PlayerPhysics.P.Y = 48 + (.5f * PlayerGraphics.half_size_y);
		PlayerPhysics.dP.Y = 0;
		grounded = true;
	}	
	
	if (PlayerPhysics.P.Y + PlayerGraphics.half_size_y > 720)
	{
		PlayerPhysics.P.Y = arena_half_size_y - PlayerGraphics.half_size_y;
		PlayerPhysics.dP.Y *= -0.75;
	}

#endif

	PlayerPhysics = NewPlayerPhysics;

	// Clear screen
	clear_screen(0xff5500);

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
	draw_rect(PlayerPhysics.P.X, PlayerPhysics.P.Y, 32, 32, 0xff2200);
};