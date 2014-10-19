// Example program:
// Using SDL2 to create an application window
// gcc SDL2GraphicsTest.c -o SDL2GraphicsTest.o `sdl2-config --cflags --libs`

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>


#define screenWidth       640
#define screenHeight      480
#define pixelCount     307200
#define subPixelCount 1228800

uint8_t pixelData[subPixelCount];

int main(int argc, char* argv[]) {
	
	SDL_Window   *window   = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture  *texture  = NULL;
	
	SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2
	
	// Create an application window with the following settings:
	window = SDL_CreateWindow(
		"An SDL2 window",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		screenWidth,                       // width, in pixels
		screenHeight,                      // height, in pixels
		SDL_WINDOW_OPENGL                  // flags - see below
	);
	
	// Check that the window was successfully made
	if (window == NULL) {
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		screenWidth, screenHeight
	);
	
	
	memset(pixelData, 0, subPixelCount);
	int i;
	for (i = 0; i < subPixelCount; i+=4) {
		if (i < subPixelCount/4)
			pixelData[i] = 255;
		else if (i < subPixelCount/2)
			pixelData[i+1] = 255;
		else if (i < 3*(subPixelCount/4))
			pixelData[i+2] = 255;
		else
			pixelData[i+3] = 255;
	}
	
	
	bool running = true;
	
	// The window is open: enter program loop (see SDL_PollEvent)
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT)
				running = false;
		}
		
		SDL_UpdateTexture(
			texture, NULL, 
			(uint32_t*)pixelData, screenWidth * sizeof(uint32_t)
		);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		
		SDL_Delay(20);
	}
	
	// Clean up
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
