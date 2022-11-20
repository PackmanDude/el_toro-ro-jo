#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

uint_least16_t width = 640;
uint_least16_t height = 480;

uint_least32_t flags = SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

void HandleSDL_Error(const char *msg);

int main(int argc, char *argv[])
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) HandleSDL_Error("SDL_Init failed");

	// Create a window
	SDL_Window *window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	if (!window) HandleSDL_Error("SDL_CreateWindow failed");

	// Get its ID
	uint_least32_t windowID = SDL_GetWindowID(window);

	VkInstanceCreateInfo instance_info = { 0 };

	unsigned int count = 0;
	SDL_Vulkan_GetInstanceExtensions(window, &count, NULL);
	const char **extensions = malloc(sizeof(char*) * count);
	SDL_Vulkan_GetInstanceExtensions(window, &count, extensions);

	instance_info.enabledExtensionCount = count;
	instance_info.ppEnabledExtensionNames = extensions;

	VkInstance instance = VK_NULL_HANDLE;
	if (vkCreateInstance(&instance_info, NULL, &instance) != VK_SUCCESS)
	{
		HandleSDL_Error("vkCreateInstance failed");
	}

	VkSurfaceKHR screen_surface = VK_NULL_HANDLE;
	if (!SDL_Vulkan_CreateSurface(window, instance, &screen_surface))
	{
		HandleSDL_Error("SDL_Vulkan_CreateSurface failed");
	}

	// Fill the surface with gray color		DOES NOT WORK
//	SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface->format, 15, 15, 15));

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

void HandleSDL_Error(const char *msg)
{
	SDL_Log("%s: %s\n", msg, SDL_GetError());
	exit(-2);
}
