#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_log.h>
#include <stdio.h>

#include <vk_types.h>
#include <vk_initializers.h>
#include "VkBootstrap.h"

#include <time.h>
#include <chrono>
#include <thread>

constexpr bool ENGINE_USE_VALIDATION_LAYERS = false;

struct AppData {
    SDL_Window* _window;
    int frameCount{};
    long long lastTimeRendered;
    float deltaTime{};
    VkExtent2D _windowExtent;
    bool stopRendering;
    bool useValidationLayers{ true };

    VkInstance _instance;
    VkDebugUtilsMessengerEXT _debug_messenger;
    VkPhysicalDevice _GPU;
    VkDevice _device;
    VkSurfaceKHR _surface;
};

void init_vulkan(AppData*);
void init_swapchain(AppData*);
void init_commands(AppData*);
void init_sync_structures(AppData*);

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    SDL_SetAppMetadata("SDL", "0.0.1", "com.neolez.sdl");
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize subsystems");
        return SDL_APP_FAILURE;
    }
    

    struct AppData* app_data = (AppData*)malloc(sizeof(struct AppData));
    app_data->_windowExtent = VkExtent2D{ 800, 600 };
    app_data->lastTimeRendered = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    app_data->stopRendering = false;
    app_data->_window = SDL_CreateWindow("SDL", app_data->_windowExtent.width, app_data->_windowExtent.height, SDL_WINDOW_VULKAN);

    if (app_data->_window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window");
        free(app_data);
        return SDL_APP_FAILURE;
    }

    init_vulkan(app_data);

    init_swapchain(app_data);

    init_commands(app_data);

    init_sync_structures(app_data);

    *appstate = app_data;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Application initialized");
    return SDL_APP_CONTINUE;
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
    SDL_DestroyWindow((appData->_window));
    free(appData);

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Application quit");
}

void init_vulkan(AppData* appData)
{
    vkb::InstanceBuilder builder;

    //make the vulkan instance, with basic debug features
    auto inst_ret = builder.set_app_name("Example Vulkan Application")
        .request_validation_layers(appData->useValidationLayers)
        .use_default_debug_messenger()
        .require_api_version(1, 3, 0)
        .build();

    vkb::Instance vkb_inst = inst_ret.value();

    //grab the instance 
    appData->_instance = vkb_inst.instance;
    appData->_debug_messenger = vkb_inst.debug_messenger;

    SDL_Vulkan_CreateSurface(appData->_window, appData->_instance, NULL, &appData->_surface);

    //vulkan 1.3 features
    VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    features.dynamicRendering = true;
    features.synchronization2 = true;

    //vulkan 1.2 features
    VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;


    //use vkbootstrap to select a gpu. 
    //We want a gpu that can write to the SDL surface and supports vulkan 1.3 with the correct features
    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    vkb::PhysicalDevice physicalDevice = selector
        .set_minimum_version(1, 3)
        .set_required_features_13(features)
        .set_required_features_12(features12)
        .set_surface(appData->_surface)
        .select()
        .value();


    //create the final vulkan device
    vkb::DeviceBuilder deviceBuilder{ physicalDevice };

    vkb::Device vkbDevice = deviceBuilder.build().value();

    // Get the VkDevice handle used in the rest of a vulkan application
    appData->_device = vkbDevice.device;
    appData->_GPU = physicalDevice.physical_device;
}
void init_swapchain(AppData* appData)
{
}
void init_commands(AppData* appData)
{
}
void init_sync_structures(AppData* appData)
{
}
