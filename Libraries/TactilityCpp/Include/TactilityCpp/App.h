#pragma once

#include <tt_app.h>
#include <type_traits>

class App {
public:
    virtual void onCreate(AppHandle app) {}
    virtual void onDestroy(AppHandle app) {}
    virtual void onShow(AppHandle context, lv_obj_t* parent) {}
    virtual void onHide(AppHandle context) {}
    virtual void onResult(AppHandle app, void* _Nullable data, AppLaunchId launchId, AppResult result, BundleHandle resultData) {}
};

template <typename T>
concept AppClass = std::is_base_of<App, T>::value;

template<AppClass T>
void onAppCreate(AppHandle app, void* _Nullable data) {
    static_cast<T*>(data)->onCreate(app);
}

template<AppClass T>
void onAppDestroy(AppHandle app, void* _Nullable data) {
    static_cast<T*>(data)->onDestroy(app);
}

template<AppClass T>
void onAppShow(AppHandle context, void* _Nullable data, lv_obj_t* parent) {
    static_cast<T*>(data)->onShow(context, parent);
}

template<AppClass T>
void onAppHide(AppHandle context, void* _Nullable data) {
    static_cast<T*>(data)->onHide(context);
}

template<AppClass T>
void onAppResult(AppHandle app, void* _Nullable data, AppLaunchId launchId, AppResult result, BundleHandle resultData) {
    static_cast<T*>(data)->onResult(app, data, launchId, result, resultData);
}

template<AppClass T>
void* createAppData() {
    return new T();
}

template<AppClass T>
void destroyAppData(void* appData) {
    auto* app = static_cast<T*>(appData);
    delete app;
}

template<AppClass T>
void registerApp() {
    tt_app_register((AppRegistration) {
        .createData = createAppData<T>,
        .destroyData = destroyAppData<T>,
        .onCreate = onAppCreate<T>,
        .onDestroy = onAppDestroy<T>,
        .onShow = onAppShow<T>,
        .onHide = onAppHide<T>,
        .onResult = onAppResult<T>
    });
}
