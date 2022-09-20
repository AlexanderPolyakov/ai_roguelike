#include <stdio.h>

#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <GLFW/glfw3.h>
#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

#include <functional>
#include "app.h"

// global state
static GLFWwindow *window = nullptr;

static mouse_cb_t mcb = mouse_cb_t();// do nothing by default

static void glfw_errorCallback(int error, const char *description)
{
	fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void glfw_mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
  if (action != GLFW_PRESS)
    return;
  double x, y;
  glfwGetCursorPos(window, &x, &y);
  mcb(button, x, y);
}

bool app_init(int width, int height)
{
  // Create a GLFW window without an OpenGL context.
  glfwSetErrorCallback(glfw_errorCallback);
  if (!glfwInit())
    return false;
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window = glfwCreateWindow(width, height, "AI practice", nullptr, nullptr);
  if (!window)
    return false;
  // Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create a render thread.
  // Most graphics APIs must be used on the same thread that created the window.
  bgfx::renderFrame();
  // Initialize bgfx using the native window handle and window resolution.
  bgfx::Init init;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
  init.platformData.ndt = glfwGetX11Display();
  init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
#elif BX_PLATFORM_OSX
  init.platformData.nwh = glfwGetCocoaWindow(window);
#elif BX_PLATFORM_WINDOWS
  init.platformData.nwh = glfwGetWin32Window(window);
#endif
  glfwGetWindowSize(window, &width, &height);
  init.resolution.width = (uint32_t)width;
  init.resolution.height = (uint32_t)height;
  init.resolution.reset = BGFX_RESET_VSYNC;
  if (!bgfx::init(init))
    return false;
  // Set view 0 to the same dimensions as the window and to clear the color buffer.
  const bgfx::ViewId kClearView = 0;
  bgfx::setViewClear(0
      , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
      , 0x303030ff
      , 1.0f
      , 0
      );
  bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
  //glfwSetMouseButtonCallback(window, glfw_mouseCallback);
  return true;
}

bool app_should_close()
{
  return glfwWindowShouldClose(window);
}

void app_poll_events()
{
  glfwPollEvents();
}

void app_handle_resize(int &width, int &height)
{
  int oldWidth = width, oldHeight = height;
  glfwGetWindowSize(window, &width, &height);
  const bgfx::ViewId kClearView = 0;
  if (width != oldWidth || height != oldHeight)
  {
    bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);
    bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
  }
}

void app_terminate()
{
  glfwTerminate();
}

void app_set_on_mouse_click(mouse_cb_t cb)
{
  mcb = cb;
}

bool app_keypressed(int key)
{
  return glfwGetKey(window, key) == GLFW_PRESS;
}


