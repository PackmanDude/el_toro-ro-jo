#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include "Eltororojo.h"

#undef SDL_HINT_APP_NAME
#define SDL_HINT_APP_NAME "El Toro-ro-jo"
#undef SDL_HINT_VIDEO_EXTERNAL_CONTEXT
#define SDL_HINT_VIDEO_EXTERNAL_CONTEXT 1

void HandleSDL_Error(const char *msg);
void CheckValidationLayerSupport(const char *layer_name, VkInstanceCreateInfo *found_layers);

int main(int argc, char *argv[])
{
	uint_least16_t width = 640;
	uint_least16_t height = 480;
	int window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

	if (SDL_Init(SDL_INIT_EVERYTHING)) HandleSDL_Error("SDL_Init() failed");
	if (!IMG_Init(IMG_INIT_PNG)) HandleSDL_Error("IMG_Init() failed");

	VkInstanceCreateInfo layers_info = { 0 }, instance_info =
	{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		&layers_info,
		.pApplicationInfo = &(VkApplicationInfo)
		{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = SDL_HINT_APP_NAME
		}
	};
	CheckValidationLayerSupport("VK_LAYER_KHRONOS_validation", &layers_info);

	SDL_Window *window = SDL_CreateWindow(
		SDL_HINT_APP_NAME,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		window_flags
	);
	if (!window) HandleSDL_Error("SDL_CreateWindow() failed");

	SDL_Vulkan_GetInstanceExtensions(window, &instance_info.enabledExtensionCount, NULL);
	SDL_Vulkan_GetInstanceExtensions(window, &instance_info.enabledExtensionCount, (const char**)instance_info.ppEnabledExtensionNames);

	VkInstance instance = NULL;
	VkResult ret;
	if (ret = vkCreateInstance(&instance_info, NULL, &instance))
	{
		SDL_DestroyWindow(window);
		SDL_SetError("%d", ret);
		HandleSDL_Error("vkCreateInstance() failed; VkResult is");
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		vkDestroyInstance(instance, NULL);
		SDL_DestroyWindow(window);
		HandleSDL_Error("SDL_CreateRenderer() failed");
	}

	VkSurfaceKHR surface = NULL;
	if (!SDL_Vulkan_CreateSurface(window, instance, &surface))
	{
		SDL_DestroyRenderer(renderer);
		vkDestroyInstance(instance, NULL);
		SDL_DestroyWindow(window);
		HandleSDL_Error("SDL_Vulkan_CreateSurface() failed");
	}

	SDL_Texture *texture = IMG_LoadTexture(renderer, "gfx/tiles/roma.png");
	if (!texture)
	{
		vkDestroySurfaceKHR(instance, surface, NULL);
		SDL_DestroyRenderer(renderer);
		vkDestroyInstance(instance, NULL);
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
					vkDestroySurfaceKHR(instance, surface, NULL);
					SDL_DestroyRenderer(renderer);
					vkDestroyInstance(instance, NULL);
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

// TODO: Change integer sizes to more sane ones.
void CheckValidationLayerSupport(const char *layer_name, VkInstanceCreateInfo *found_layers)
{
	uint32_t layers_count = 0;
	VkLayerProperties available_layers[255] = { 0 };

	vkEnumerateInstanceLayerProperties(&layers_count, NULL);
	vkEnumerateInstanceLayerProperties(&layers_count, available_layers);
	if (found_layers)
	{
		found_layers->enabledLayerCount = layers_count;
		found_layers->ppEnabledLayerNames = (const char**)available_layers;
	}
	for (uint_least8_t i = 0; i < sizeof(available_layers) / sizeof(available_layers[0]); ++i)
	{
		if (!strcmp(available_layers[i].layerName, layer_name)) return;
	}
	SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "No '%s' layer supported/installed!\n", layer_name);
	SDL_Quit();
	exit(-1);
}
