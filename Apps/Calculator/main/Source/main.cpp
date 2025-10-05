#include <tt_app.h>
#include "Calculator.h"

static void onShowApp(AppHandle appHandle, void* data, lv_obj_t* parent) {
    static_cast<Calculator*>(data)->onShow(appHandle, parent);
}

static void* createAppData() {
    return new Calculator();
}

static void destroyAppData(void* app) {
    delete static_cast<Calculator*>(app);
}

extern "C" {

int main(int argc, char* argv[]) {
    tt_app_register((AppRegistration) {
        .createData = createAppData,
        .destroyData = destroyAppData,
        .onShow = onShowApp,
    });
    return 0;
}

}
