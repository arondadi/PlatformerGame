#include "utils.cpp"
#include <windows.h>
#include <Xinput.h>

/*
	 Using initially:
	 Exclude other files that platform layer from build
	 Change subsystem to windows
*/


global_variable bool running = true;

global_variable const u32 TileMapHeight = 23;
global_variable const u32 TileMapWidth = 40;

global_variable const u32 TileWidth = 32;
global_variable const u32 TileHeight = 32;


struct Render_State {
	int width;
	int height;
	void* memory;
	BITMAPINFO bitmap_info;
};

global_variable Render_State render_state;

#include "platform_common.cpp"
#include "renderer.cpp"
#include "game.cpp"


/*
XInput code below:
*/
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
	return(0);
}
global_variable x_input_get_state* XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_


#define X_INPUT_SET_STATE(name) DWORD WINAPI name( DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
	return(0);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void
Win32LoadXInput(void)
{
	HMODULE XInputLibrary = LoadLibrary("xinput1_3.dll");
	if (XInputLibrary)
	{
		XInputGetState = (x_input_get_state *) GetProcAddress(XInputLibrary, "XInputGetState");
		XInputSetState = (x_input_set_state *) GetProcAddress(XInputLibrary, "XInputSetState");
	}
}



LRESULT CALLBACK
window_callback(
	HWND   hwnd,
	UINT   uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	LRESULT result = 0;

	switch (uMsg)
	{
	case WM_CLOSE:
	{
		// If user presses close, close the window
		DestroyWindow(hwnd);
	}break;
	case WM_DESTROY:
	{
		running = false;
	} break;

	case WM_SIZE:
	{
#if 1
		RECT rect;
		GetClientRect(hwnd, &rect);
#endif
#if 0
		//TODO: Hard coded rect values so ClientRect is right. Fix hard coding
		DWORD windowStyles = WS_VISIBLE;
		RECT rect = { 0, 0, 1280, 720 };
		BOOL success = AdjustWindowRectEx(&rect, windowStyles, false, 0);
#endif
		render_state.width = rect.right - rect.left;
		render_state.height = rect.bottom - rect.top;

		int size = render_state.width * render_state.height * sizeof(u32);

		if (render_state.memory)
		{
			VirtualFree(render_state.memory, 0, MEM_RELEASE);
		}
		render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
		render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
		render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
		render_state.bitmap_info.bmiHeader.biPlanes = 1;
		render_state.bitmap_info.bmiHeader.biBitCount = 32;
		render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;
	} break;

	default: {
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
	return result;
};


// Change Linker subsystem -> Windows + All config + All platforms
int
WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd
)
{
	// Load XInput
	Win32LoadXInput();


	// Create Window Class
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = "Game Window Class";
	window_class.lpfnWndProc = window_callback;

	// Register Class
	RegisterClass(&window_class);

	// Create Window
	HWND window = CreateWindow(window_class.lpszClassName,
		"Platformer Game",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1400,
		800,
		0,
		0,
		hInstance,
		0
	);

	// Handle to Device Context
	HDC hdc = GetDC(window);
#if 0
	// Hide the cursor over the window
	ShowCursor(false);
#endif
	Input input = {};

	// 60 FPS time
	float delta_time = 0.0166666f;
	LARGE_INTEGER frame_begin_time;
	QueryPerformanceCounter(&frame_begin_time);

	float performance_frequency;
	LARGE_INTEGER perf;
	QueryPerformanceFrequency(&perf);
	performance_frequency = (float)perf.QuadPart;


	while (running)
	{
		// Input
		MSG message;

		for (int i = 0; i < BUTTON_COUNT; ++i)
		{
			input.buttons[i].changed = false;
		}
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
		{
			switch (message.message)
			{
			case WM_KEYUP:
			case WM_KEYDOWN:
			{
				u32 vk_code = (u32)message.wParam;
				bool is_down = ((message.lParam & (1 << 31)) == 0);

#define process_button(b, vk)\
case vk: {\
input.buttons[b].changed = is_down != input.buttons[b].is_down;\
input.buttons[b].is_down = is_down;\
} break;

				switch (vk_code) {
					process_button(BUTTON_LEFT, VK_LEFT);
					process_button(BUTTON_RIGHT, VK_RIGHT);
					process_button(BUTTON_SPACE, VK_SPACE);
				case VK_ESCAPE:
				{
					running = false;
				}break;
				}

			}break;

			default:
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}break;

			}

			for (DWORD ControllerIndex = 0; ControllerIndex < XUSER_MAX_COUNT; ++ControllerIndex)
			{
				XINPUT_STATE ControllerState;
				if (XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
				{
					// This controller is plugged in
					XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

					bool Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
					bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
					bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
					bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
					bool Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
					bool Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
					bool LeftShoulder = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
					bool RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
					bool AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
					bool BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
					bool XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
					bool YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);

					u16 StickX = Pad->sThumbLX;
					u16 StickY = Pad->sThumbLY;

					if (AButton || Up || Down || Left || Right || BButton || XButton || YButton)
					{
						bool is_down = true;;
						input.buttons[BUTTON_SPACE].changed = is_down != input.buttons[BUTTON_SPACE].is_down; 
						input.buttons[BUTTON_SPACE].is_down = is_down; 
					}
				}
				else
				{
					// The controller is not available
				}
			}


		}



		// Simulate
		simulate_game(&input, delta_time);

		// Render
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0,
			render_state.width, render_state.height, render_state.memory,
			&render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);


		// Frame per seconds calculations
		LARGE_INTEGER frame_end_time;
		QueryPerformanceCounter(&frame_end_time);

		delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;

		// Fixed framerate
		if(delta_time > 0.01666667)
		{
			delta_time = 0.01666667;
		}
		frame_begin_time = frame_end_time;
	}

};