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
	{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
	{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
	{1, 0, 0, 0,  1, 1, 1, 1,  1, 1, 1, 1,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
	{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
	{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
	{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1, 1, 1, 1,  1, 1, 1, 1,  0, 0, 0, 0,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
	{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1, 1, 1, 1,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
	{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1, 1, 1, 1,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
	{1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1, 1, 1, 1,	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
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
aabb_collision(float x1, float y1, float half_size_x1, float half_size_y1, float x2, float y2, float half_size_x2, float half_size_y2)
{
	if ((y1 - half_size_y1) < (y2 + half_size_y2) &&
		(y1 + half_size_y1) > (y2 - half_size_y2) &&
		(x1 + half_size_x1) > (x2 - half_size_x2) &&
		(x1 - half_size_x1) < (x2 + half_size_x2))
	{
		return true;
	}
	else
	{
		return false;
	}

}

enum Sides
{
	NONE,
	BOTTOM,
	TOP,
	LEFT,
	RIGHT
};

internal Sides
CollisionSide(v2 collider, float half_size_x1, float half_size_y1, v2 collided, float half_size_x2, float half_size_y2)
{
	// Returns the side of the collider
	Sides Result;
	
	if ((collider.Y - half_size_y1) < (collided.Y + half_size_y2 * .5f) &&
		(collider.Y + half_size_y1) > (collided.Y - half_size_y2 * .5f) &&
		(collider.X + half_size_x1) > (collided.X - half_size_x2 * .5f) &&
		(collider.X - half_size_x1) < (collided.X + half_size_x2 * .5f))
	{
		float bottom, top, left, right;
		bottom = (collider.Y - half_size_y1) - (collided.Y + half_size_y2 * .5);
		top = (collided.Y - half_size_y2 * .5f) - (collider.Y + half_size_y1 * .5f);
		right = (collider.X + half_size_x1) - (collided.X - half_size_x2 * .5f);
		left = (collided.X + half_size_x2 *.5f) - (collider.X - half_size_x1);

		float vals[4] = { Absolute(bottom), Absolute(top), Absolute(left), Absolute(right) };
		Sides index[4] = { BOTTOM, TOP,LEFT, RIGHT };

		float lowest = vals[0];
		Result = index[0];

		for (u32 i = 0; i < 4; ++i)
		{
			if (vals[i] < lowest)
			{
				lowest = vals[i];
				Result = index[i];
			}
		}
		return Result;
	}
	return Result = NONE;
}

internal v2
GetTileIndex(float x, float y)
{
	v2 Result;

	Result.X = (u32) round((x / TileWidth));
	Result.Y = (u32) (y / TileHeight);

	return Result;
};

internal v2
GetTileAbs(u32 x, u32 y)
{
	v2 Result;

	Result.X = x * TileWidth;
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

		PlayerPhysics.P.X = 22 * 32;
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


	// Check the tile the player is currently in and the tile the player is going to be in.
	// Add 1 tile to each side, else the player clipps into walls until new position goes into new tile
	u32 x0 = min(OldPlayerTileIndex.X, NewPlayerTileIndex.X) - 1;
	u32 x1 = max(OldPlayerTileIndex.X, NewPlayerTileIndex.X) + 1;
	u32 y0 = min(OldPlayerTileIndex.Y, NewPlayerTileIndex.Y) - 1;
	u32 y1 = max(OldPlayerTileIndex.Y, NewPlayerTileIndex.Y) + 1;


	for (u32 i = x0; i <= x1; ++i)
	{
		for (u32 j = y0; j <= y1; ++j)
		{
			if (TileMap[j][i] == 1)
			{
				v2 TileAbsPos = GetTileAbs(i, j);

				// Collision side of player/colliding object
				Sides CurrentSide = CollisionSide(NewPlayerPhysics.P, PlayerGraphics.half_size_x, PlayerGraphics.half_size_y, TileAbsPos, TileWidth, TileHeight);

 				if (CurrentSide != 0)
				{
					switch (CurrentSide)
					{
						case BOTTOM:
						{
							NewPlayerPhysics.P.Y = TileAbsPos.Y + TileHeight * .5f + PlayerGraphics.half_size_x;
							NewPlayerPhysics.dP.Y = 0;
							grounded = true;
						}break;

						case TOP:
						{
							NewPlayerPhysics.P.Y = TileAbsPos.Y - TileHeight * .5f - PlayerGraphics.half_size_x;
							NewPlayerPhysics.dP.Y = 0;
						}break;	

						case RIGHT:
						{
							Sides TestSide = CollisionSide(NewPlayerPhysics.P, PlayerGraphics.half_size_x, PlayerGraphics.half_size_y, TileAbsPos, TileWidth, TileHeight);
							NewPlayerPhysics.P.X = TileAbsPos.X - TileWidth * .5f - PlayerGraphics.half_size_x;
							NewPlayerPhysics.dP.X *= -1;
						}break;

						case LEFT:
						{
							Sides TestSide = CollisionSide(NewPlayerPhysics.P, PlayerGraphics.half_size_x, PlayerGraphics.half_size_y, TileAbsPos, TileWidth, TileHeight);
							NewPlayerPhysics.P.X = TileAbsPos.X + TileWidth * .5f + PlayerGraphics.half_size_x;
							NewPlayerPhysics.dP.X *= -1;
						}break;
					}
				}
			}
		}
	}





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
			u32 RowTile = row * TileHeight;
			u32 ColTile = col * TileWidth;

			draw_rect(ColTile, RowTile, TileHeight, TileWidth, color);
		}
	}
	



	// Player rect
	//draw_rect(PlayerPhysics.P.X*32, PlayerPhysics.P.Y*32, PlayerGraphics.half_size_x, PlayerGraphics.half_size_y, 0xff2200);
	draw_rect(PlayerPhysics.P.X, PlayerPhysics.P.Y, 32, 32, 0xff2200);
};