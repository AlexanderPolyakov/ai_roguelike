#pragma once
#include <functional>

bool app_init(int width, int height);
bool app_should_close();
void app_poll_events();
void app_handle_resize(int &width, int &height);
void app_terminate();

typedef std::function<void(int, float, float)> mouse_cb_t;

void app_set_on_mouse_click(mouse_cb_t cb);

bool app_keypressed(int key);

