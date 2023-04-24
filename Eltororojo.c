#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include "Eltororojo.h"

void HandleSDL_Error(const char *msg);
void CheckValidationLayerSupport(const char *layer_name);
VkResult ChooseGPU(VkInstance, VkPhysicalDevice *gpu, uint32_t *queue_index,
	VkPhysicalDeviceFeatures*);

int main(int argc, char *argv[])
{
	SDL_SetHint("SDL_HINT_APP_NAME", "El Toro-ro-jo");
	SDL_SetHint("SDL_HINT_VIDEO_EXTERNAL_CONTEXT", "1");

	uint_least16_t width = 640U;
	uint_least16_t height = 480U;
	int window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

	if (SDL_Init(SDL_INIT_EVERYTHING)) HandleSDL_Error("SDL_Init() failed");
	if (!IMG_Init(IMG_INIT_PNG)) HandleSDL_Error("IMG_Init() failed");

	const char *layer_names[] = { "VK_LAYER_KHRONOS_validation" };
	CheckValidationLayerSupport("VK_LAYER_KHRONOS_validation");
	VkInstanceCreateInfo instance_info =
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &(VkApplicationInfo)
		{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = "El Toro-ro-jo"
		},
		sizeof(layer_names) / sizeof(layer_names[0]),
		// You need malloc() (or probably calloc() ) here.
//		(const char * const*)layer_name
		layer_names
	};

	SDL_Window *window = SDL_CreateWindow(
		"El Toro-ro-jo",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		window_flags
	);
	if (!window) HandleSDL_Error("SDL_CreateWindow() failed");

	SDL_Vulkan_GetInstanceExtensions(window, &instance_info.enabledExtensionCount, NULL);
	const char *extensions[instance_info.enabledExtensionCount];
	SDL_Vulkan_GetInstanceExtensions(window, &instance_info.enabledExtensionCount, extensions);
	instance_info.ppEnabledExtensionNames = extensions;

	VkInstance instance = NULL;
	VkResult ret = vkCreateInstance(&instance_info, NULL, &instance);
	if (ret)
	{
		SDL_DestroyWindow(window);
		SDL_SetError("%d", ret);
		HandleSDL_Error("vkCreateInstance() failed; VkResult is");
	}

	VkPhysicalDevice gpu = NULL;
	uint32_t queue_index = 0U;
	VkPhysicalDeviceFeatures device_features = { 0 };
	ret = ChooseGPU(instance, &gpu, &queue_index, &device_features);
	if (ret || !gpu)
	{
		vkDestroyInstance(instance, NULL);
		SDL_DestroyWindow(window);
		SDL_SetError("%d", ret);
		HandleSDL_Error("vkEnumeratePhysicalDevices() failed; VkResult is");
	}

	VkDevice device = NULL;
	VkDeviceCreateInfo device_info =
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = 1U,
		&(VkDeviceQueueCreateInfo)
		{
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = queue_index,
			1U,
			(float[]){ 0.0f }
		},
		.enabledExtensionCount = instance_info.enabledExtensionCount,
		instance_info.ppEnabledExtensionNames,
		&device_features
	};
	vkCreateDevice(gpu, &device_info, NULL, &device);

	VkSurfaceKHR surface = VK_NULL_HANDLE;
	if (!SDL_Vulkan_CreateSurface(window, instance, &surface))
	{
		vkDestroyDevice(device, NULL);
		vkDestroyInstance(instance, NULL);
		SDL_DestroyWindow(window);
		HandleSDL_Error("SDL_Vulkan_CreateSurface() failed");
	}

/*	SDL_Texture *texture = IMG_LoadTexture(renderer, "gfx/tiles/roma.png");
	if (!texture)
	{
		vkDestroySurfaceKHR(instance, surface, NULL);
		vkDestroyDevice(device, NULL);
		vkDestroyInstance(instance, NULL);
		SDL_DestroyWindow(window);
		HandleSDL_Error("IMG_LoadTexture(gfx/tiles/roma.png) failed");
	}
*/

	/// insert stb_image here



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
					vkDestroySurfaceKHR(instance, surface, NULL);
					vkDestroyDevice(device, NULL);
					vkDestroyInstance(instance, NULL);
					SDL_DestroyWindow(window);
					SDL_Quit();
					return EXIT_SUCCESS;
			}
		}

//		SDL_RenderClear(renderer);
//		SDL_RenderCopy(renderer, texture, NULL, NULL);
//		SDL_RenderPresent(renderer);
	}
}

void HandleSDL_Error(const char *msg)
{
	SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s: %s", msg, SDL_GetError());
	SDL_Quit();
	exit(-1);
}

void CheckValidationLayerSupport(const char *layer_name)
{
	uint32_t layers_count = 0U;

	vkEnumerateInstanceLayerProperties(&layers_count, NULL);
	VkLayerProperties available_layers[layers_count];
	vkEnumerateInstanceLayerProperties(&layers_count, available_layers);
	for (uint32_t i = 0U; i < layers_count; ++i)
	{
		if (!strcmp(available_layers[i].layerName, layer_name)) return;
	}
	SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "No '%s' layer"
		" supported/installed!", layer_name);
	SDL_Quit();
	exit(-1);
}

// Not well tested
VkResult ChooseGPU(VkInstance instance, VkPhysicalDevice *gpu,
	uint32_t *queue_index, VkPhysicalDeviceFeatures *device_features)
{
	uint32_t gpu_number = 0U;

	uint32_t gpu_total = 0U;
	vkEnumeratePhysicalDevices(instance, &gpu_total, NULL);
	VkPhysicalDevice physical_devices[gpu_total];
	memset(&physical_devices, 0, sizeof(physical_devices));
	VkPhysicalDeviceProperties device_properties[gpu_total];
	memset(&device_properties, 0, sizeof(device_properties));

	VkResult ret = vkEnumeratePhysicalDevices(instance,
		&gpu_total, physical_devices);
	if (ret) return ret;

	size_t max_device_name_length = 0U;
	uint32_t graphics_queue_node_index = 0U;
	for (; gpu_number < gpu_total; ++gpu_number)
	{
		*gpu = physical_devices[gpu_number];

		vkGetPhysicalDeviceProperties(*gpu, &device_properties[gpu_number]);
		size_t current_device_name_length = strlen(device_properties[gpu_number].deviceName);
		if (current_device_name_length > max_device_name_length)
		{
			max_device_name_length = current_device_name_length;
		}

		uint32_t queue_count = 0U;
		vkGetPhysicalDeviceQueueFamilyProperties(*gpu, &queue_count, NULL);

		VkQueueFamilyProperties queue_properties[queue_count];
		memset(&queue_properties, 0, sizeof(queue_properties));
		vkGetPhysicalDeviceQueueFamilyProperties(*gpu, &queue_count, queue_properties);

		vkGetPhysicalDeviceFeatures(*gpu, device_features);

		graphics_queue_node_index = UINT32_MAX;
		for (uint32_t i = 0U; i < queue_count; ++i)
		{
			if ((queue_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
				graphics_queue_node_index == UINT32_MAX)
			{
				graphics_queue_node_index = i;
			}
		}
		// This GPU supports graphics queue, quit the loop.
		if (graphics_queue_node_index != UINT32_MAX)
		{
			break;
		}
	}
	uint32_t selected_gpu = gpu_number + 1U;
	// How length is computed: 16 characters for GPU ID (up to 10^15 - 1 GPUs
	// can be listed) + other characters in the string + GPU name string
	// length.
	size_t per_gpu_list_length = (35U + max_device_name_length) * gpu_total;
	char *per_gpu_list = calloc(per_gpu_list_length, 1);
	if (!per_gpu_list) perror("calloc() failed");

	for (gpu_number = 0U; gpu_number < gpu_total; ++gpu_number)
	{
		snprintf(per_gpu_list + gpu_number * per_gpu_list_length,
			per_gpu_list_length / gpu_total,
			"\t%u - %s [%x:%x];", gpu_number + 1U,
			device_properties[gpu_number].deviceName,
			device_properties[gpu_number].vendorID,
			device_properties[gpu_number].deviceID
		);
	}

	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
		"List of available GPUs for rendering: (total %u)\n%s",
		gpu_total, per_gpu_list
	);
	free(per_gpu_list);

	if (graphics_queue_node_index == UINT32_MAX)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR,
			"No queue for graphics found! Tried all available GPUs.");
		*gpu = NULL;
		return ret;
	}
	*queue_index = graphics_queue_node_index;
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
		"Selecting (%u) %s.",
		selected_gpu, device_properties[selected_gpu - 1].deviceName
	);
	return ret;
}
