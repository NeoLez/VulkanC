#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <vk_types.h>

#include <time.h>
#include <chrono>
#include <thread>

constexpr bool ENGINE_USE_VALIDATION_LAYERS = false;

struct AppData {
    SDL_Window* window;
    int frameCount{};
    long long lastTimeRendered;
    float deltaTime{};
    VkExtent2D _windowExtent;
    bool stopRendering;
};

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    SDL_SetAppMetadata("SDL", "0.0.1", "com.neolez.sdl");
    if (SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        struct AppData* app_data = (AppData*)malloc(sizeof(struct AppData));
        app_data->_windowExtent = VkExtent2D{ 800, 600 };
        app_data->lastTimeRendered = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        app_data->stopRendering = false;

        app_data->window = SDL_CreateWindow("SDL", app_data->_windowExtent.width, app_data->_windowExtent.height, SDL_WINDOW_VULKAN);

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
    struct AppData* app_data = (AppData*)appstate;

    long long currentTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    long long msDifference = currentTime - app_data->lastTimeRendered;
    app_data->deltaTime = msDifference / 1000000.0f;
    app_data->lastTimeRendered = currentTime;

    if (app_data->stopRendering) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    else {

        //Draw

    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    AppData *appData = (AppData*)appstate;
    //SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Event Received: %X", event->type);
    switch (event->type) {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_WINDOW_MINIMIZED:
        appData->stopRendering = true;
        break;
    case SDL_EVENT_WINDOW_RESTORED:
        appData->stopRendering = false;
        break;
    case SDL_EVENT_KEY_DOWN:
        printf("%x", event->key.key);
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    AppData* appData = (AppData*)appstate;
    SDL_DestroyWindow((appData->window));
    free(appData);

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Application quit");
}