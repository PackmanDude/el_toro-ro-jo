#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include "Eltororojo.h"

void HandleSDL_Error(const char *msg);

int main(int argc, char *argv[])
{
	uint_least16_t width = 640;
	uint_least16_t height = 480;
	int window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

	if (SDL_Init(SDL_INIT_EVERYTHING)) HandleSDL_Error("SDL_Init() failed");
	if (!IMG_Init(IMG_INIT_PNG)) HandleSDL_Error("IMG_Init() failed");

	SDL_Window *window = SDL_CreateWindow(
		"El Toro-ro-jo",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		window_flags
	);
	if (!window) HandleSDL_Error("SDL_CreateWindow() failed");

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		SDL_DestroyWindow(window);
		HandleSDL_Error("SDL_CreateRenderer() failed");
	}

	VkInstanceCreateInfo instance_info = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	SDL_Vulkan_GetInstanceExtensions(window, &instance_info.enabledExtensionCount, NULL);
	SDL_Vulkan_GetInstanceExtensions(window, &instance_info.enabledExtensionCount, (const char**)instance_info.ppEnabledExtensionNames);

	VkInstance instance = VK_NULL_HANDLE;
	if (vkCreateInstance(&instance_info, NULL, &instance))
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		HandleSDL_Error("vkCreateInstance() failed");
	}

	VkSurfaceKHR surface = VK_NULL_HANDLE;
	if (!SDL_Vulkan_CreateSurface(window, instance, &surface))
	{
		vkDestroyInstance(instance, NULL);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		HandleSDL_Error("SDL_Vulkan_CreateSurface() failed");
	}

	SDL_Texture *texture = IMG_LoadTexture(renderer, "gfx/tiles/roma.png");
	if (!texture)
	{
		vkDestroySurfaceKHR(instance, surface, NULL);
		vkDestroyInstance(instance, NULL);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		HandleSDL_Error("IMG_LoadTexture(gfx/tiles/roma.png) failed");
	}

	SDL_Event event;
	// Main loop
	while (1)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_WINDOWEVENT:
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
					break;
				case SDL_QUIT:
					SDL_DestroyTexture(texture);
					SDL_DestroyRenderer(renderer);
					SDL_DestroyWindow(window);
					SDL_Quit();
					return EXIT_SUCCESS;
			}
		}

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}
}

void HandleSDL_Error(const char *msg)
{
	SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s: %s\n", msg, SDL_GetError());
	SDL_Quit();
	exit(-1);
}
