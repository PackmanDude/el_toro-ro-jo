#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <stdio.h>

uint_least16_t width = 640;
uint_least16_t height = 480;

uint_least32_t flags = SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

void HandleSDL_Error(const char *msg);

int main(int argc, char *argv[])
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) HandleSDL_Error("Unable to initialize SDL");

	// Create a window
	SDL_Window *window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	if (window == NULL) HandleSDL_Error("Window could not be created");

	// Get its ID
	uint_least32_t windowID = SDL_GetWindowID(window);

	// WIP
	unsigned int count;
	if (!SDL_Vulkan_GetInstanceExtensions(window, &count, NULL)) HandleSDL_Error("Unable to get Vulkan instance extensions");

	// Create Vulkan instance
	VkInstance instance;
	vkCreateInstance(NULL, NULL, &instance);

	// Get window surface
	SDL_Surface *screenSurface;
	if (!SDL_Vulkan_CreateSurface(window, instance, &screenSurface)) HandleSDL_Error("Unable to create screen surface");

	// Fill the surface with gray color
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
	return -1;
}

void HandleSDL_Error(const char *msg)
{
	SDL_Log("%s: %s", msg, SDL_GetError());
	exit(-2);
}
