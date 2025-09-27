#include <tt_app.h>
#include "Application.h"

static void onShow(AppHandle appHandle, void* data, lv_obj_t* parent) {
    static_cast<Application*>(data)->onShow(appHandle, parent);
}

static void* createApp() {
    return new Application();
}

static void destroyApp(void* app) {
    delete static_cast<Application*>(app);
}

ExternalAppManifest manifest = {
    .createData = createApp,
    .destroyData = destroyApp,
    .onShow = onShow,
};

extern "C" {

int main(int argc, char* argv[]) {
    tt_app_register(&manifest);
    return 0;
}

}
