#include <tt_app.h>
#include "Application.h"

static void onCreateApp(AppHandle appHandle, void* data) {
    static_cast<Application*>(data)->onCreate(appHandle);
}

static void onDestroyApp(AppHandle appHandle, void* data) {
    static_cast<Application*>(data)->onDestroy(appHandle);
}

static void onShowApp(AppHandle appHandle, void* data, lv_obj_t* parent) {
    static_cast<Application*>(data)->onShow(appHandle, parent);
}

static void onHideApp(AppHandle appHandle, void* data) {
    static_cast<Application*>(data)->onHide(appHandle);
}

static void* createAppData() {
    return new Application();
}

static void destroyAppData(void* app) {
    delete static_cast<Application*>(app);
}

extern "C" {

int main(int argc, char* argv[]) {
    tt_app_register((AppRegistration) {
        .createData = createAppData,
        .destroyData = destroyAppData,
        .onCreate = onCreateApp,
        .onDestroy = onDestroyApp,
        .onShow = onShowApp,
        .onHide = onHideApp,
    });
    return 0;
}

}
