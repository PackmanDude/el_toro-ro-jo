#include <SDL2/SDL.h>
#include <stdio.h>

uint32_t flags = SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

uint16_t width = 640;
uint16_t height = 480;

void HandleSDL_Error(char *argv[], const char *msg);

int main(int argc, char *argv[])
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		HandleSDL_Error(argv, "SDL could not initialize!");
	}

	// Create a window
	SDL_Window *window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	if (!window)
	{
		HandleSDL_Error(argv, "Window could not be created!");
	}

	// Get its ID
	uint32_t windowID = SDL_GetWindowID(window);

	// Get window surface
	SDL_Surface *screenSurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
	if (!screenSurface)
	{
		HandleSDL_Error(argv, "Surface could not be created!");
	}

	// Fill the surface with gray
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 15, 15, 15));

	// Update the surface
	SDL_UpdateWindowSurface(window);

	// Keep the window open
	while (1)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_WINDOWEVENT:
					if (event.window.windowID == windowID)
					{
						switch (event.window.event)
						{
							case SDL_WINDOWEVENT_SIZE_CHANGED:
								width = event.window.data1;
								height = event.window.data2;
								break;
							case SDL_WINDOWEVENT_CLOSE:
								event.type = SDL_QUIT;
								SDL_PushEvent(&event);
								break;
						}
					}
					break;
				case SDL_QUIT:
					SDL_Quit();
					return 0;
			}
		}
	}
}

void HandleSDL_Error(char *argv[], const char *msg)
{
		fprintf(stderr, "%s: %s %s\n", argv[0], msg, SDL_GetError());
		exit(-1);
}
