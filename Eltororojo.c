#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

void HandleSDL_Error(const char *message);

int main(int argc, char *argv[])
{
	uint_least16_t width = 640;
	uint_least16_t height = 480;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		HandleSDL_Error("SDL_Init failed");
	}

	// Create a window
	SDL_Window *window = SDL_CreateWindow(
		"Test",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_VULKAN |
		SDL_WINDOW_SHOWN |
		SDL_WINDOW_FULLSCREEN
	);
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

	VkSurfaceKHR surface = VK_NULL_HANDLE;
	if (!SDL_Vulkan_CreateSurface(window, instance, &surface))
	{
		HandleSDL_Error("SDL_Vulkan_CreateSurface failed");
	}

	// Keep the window open
	while (1)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_WINDOWEVENT:
//					if (event.window.windowID == windowID)
//					{
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
//					}
					break;
				case SDL_QUIT:
					SDL_DestroyWindow(window);
					SDL_Quit();
					return 0;
			}
		}
	// paste code for renderer here
	}
}

void HandleSDL_Error(const char *message)
{
	SDL_Log("%s: %s\n", message, SDL_GetError());
	exit(-1);
}
