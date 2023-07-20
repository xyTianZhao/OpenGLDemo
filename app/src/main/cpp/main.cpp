//
// Created by max on 2023/7/19.
//

#include <android_native_app_glue.h>
#include <jni.h>

#include "Layer.h"
#include "Render.h"
#include "Logger.h"

struct AppData {
    bool isPause = false;
    Render* render = nullptr;
    Layer* layer = new Layer();
};

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    auto *appData = reinterpret_cast<AppData*>(app->userData);
    if (!appData->isPause) {
        appData->render->handleInput(event);
    }
    return 0;
}

static void handle_cmd(struct android_app *app, int32_t cmd) {
    auto *appData = reinterpret_cast<AppData*>(app->userData);
    switch (cmd) {
        case APP_CMD_INIT_WINDOW: {
            LOGI("APP_CMD_INIT_WINDOW")
            appData->render = new Render(app,appData->layer);
            break;
        }
        case APP_CMD_TERM_WINDOW: {
            LOGI("APP_CMD_TERM_WINDOW")
            delete appData->render;
            break;
        }
        case APP_CMD_RESUME: {
            LOGI("APP_CMD_RESUME")
            appData->isPause = false;
            break;
        }
        case APP_CMD_PAUSE: {
            LOGI("APP_CMD_PAUSE")
            appData->isPause = true;
            break;
        }
        case APP_CMD_STOP: {
            LOGI("APP_CMD_STOP")
            break;
        }
        case APP_CMD_DESTROY: {
            LOGI("APP_CMD_DESTROY")
            break;
        }
        default:
            break;
    }
}

void android_main(struct android_app *app) {
    LOGI("Welcome to android_main")
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;
    AppData appData{};
    appData.layer->layers.push_back(new BackgroundLayer());
    app->userData = &appData;

    // 循环等待要做的事情。
    while (!app->destroyRequested) {
        int events;
        struct android_poll_source *source;
        // 获取需要处理的事件
        if (ALooper_pollAll(0, nullptr, &events, (void **) &source) >= 0) {
            if (source) {
                source->process(app, source);
            }
        }
        // do app something
        if (!appData.isPause && appData.render != nullptr) {
            // Render a frame
            appData.render->render();
        }
    }
}