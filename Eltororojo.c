#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

void HandleSDL_Error(const char *msg);

int main(int argc, char *argv[])
{
	uint_least16_t width = 640;
	uint_least16_t height = 480;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		HandleSDL_Error("SDL_Init() failed");
	}

	SDL_Window *window = SDL_CreateWindow(
		"EL_toro-ro-jo",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN
	);
	if (!window) HandleSDL_Error("SDL_CreateWindow() failed");

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		SDL_DestroyWindow(window);
		HandleSDL_Error("SDL_CreateRenderer() failed");
	}

	SDL_Surface *surface = SDL_LoadBMP("roma.bmp");
	if (!surface)
	{
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		HandleSDL_Error("SDL_LoadBMP() failed");
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	if (!texture)
	{
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		HandleSDL_Error("SDL_CreateTextureFromSurface() failed");
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
	SDL_Log("%s: %s\n", msg, SDL_GetError());
	SDL_Quit();
	exit(-1);
}
