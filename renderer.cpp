internal void 
clear_screen(u32 color)
{
	u32* pixel = (u32*)render_state.memory;
	for (int y = 0; y < render_state.height; ++y)
	{
		for (int x = 0; x < render_state.width; ++x)
		{
			*pixel++ = color;
		}
	}
}



//TODO: Make pixel independant
internal void
draw_rect_in_pixels(int x0, int y0, int x1, int y1, u32 color) {

	x0 = clamp(0, x0, render_state.width);
	x1 = clamp(0, x1, render_state.width);
	y0 = clamp(0, y0, render_state.height);
	y1 = clamp(0, y1, render_state.height);

	for (int y = y0; y < y1; y++) {
		u32* pixel = (u32*)render_state.memory + x0 + y * render_state.width;
		for (int x = x0; x < x1; x++) {
			*pixel++ = color;
		}
	}
}

global_variable float render_scale = 1.f;


internal void
draw_rect(float x, float y, float width, float height, u32 color) 
{
	// Offset from origin in center of rect
	x += 0.5f * width;
	y += 0.5f * height;

	// Change to pixels
	s32 x0 = x - width * 0.5f;
	s32 x1 = x + width * 0.5f;
	s32 y0 = y - height * 0.5f;
	s32 y1 = y + height * 0.5f;

	draw_rect_in_pixels(x0, y0, x1, y1, color);
}
