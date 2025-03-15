#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <imgui.h>
#include <cglm.h>
#include <stb_image.h>
#include <imgui.h>

struct AppData {
    SDL_Window* window;
};

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    SDL_SetAppMetadata("SDL", "0.0.1", "com.neolez.sdl");
    if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        struct AppData* app_data = (AppData*)malloc(sizeof(struct AppData));

        app_data->window = SDL_CreateWindow("SDL", 800, 600, SDL_WINDOW_OPENGL);

        if (app_data->window == NULL) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window");
            free(app_data);
            return SDL_APP_FAILURE;
        }

        *appstate = app_data;
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Application initialized");
        return SDL_APP_CONTINUE;
    }
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize subsystems");

    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Event Received: %X", event->type);
    switch (event->type) {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    free(appstate);
    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Application quit");
}