// initial skeleton is a clone from https://github.com/jpcy/bgfx-minimal-example
//
#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/timer.h>
#include <debugdraw/debugdraw.h>
#include "app.h"
#include <flecs.h>
#include "ecsTypes.h"
#include "roguelike.h"

int main(int argc, const char **argv)
{
  int width = 1920;
  int height = 1080;
  if (!app_init(width, height))
    return 1;
  ddInit();

  bgfx::setDebug(BGFX_DEBUG_TEXT);

  bx::Vec3 eye(0.f, 0.f, -16.f);
  bx::Vec3 at(0.f, 0.f, 0.f);
  bx::Vec3 up(0.f, 1.f, 0.f);

  float view[16];
  float proj[16];
  bx::mtxLookAt(view, bx::load<bx::Vec3>(&eye.x), bx::load<bx::Vec3>(&at.x), bx::load<bx::Vec3>(&up.x) );

  flecs::world ecs;

  init_roguelike(ecs);

  DebugDrawEncoder dde;
  while (!app_should_close())
  {
    process_turn(ecs);

    app_poll_events();
    // Handle window resize.
    app_handle_resize(width, height);
    //bx::mtxOrtho(proj, 0.f, width, 0.f, height, 0.f, 100.f, 0.f, bgfx::getCaps()->homogeneousDepth);
    bx::mtxProj(proj, 60.0f, float(width)/float(height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(0, view, proj);

    // This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
    const bgfx::ViewId kClearView = 0;
    bgfx::touch(kClearView);

    dde.begin(0);
    dde.drawGrid(bx::Vec3(0, 0, 1), bx::Vec3(0.5, 0.5, 0), 40);
    dde.end();
    print_stats(ecs);

    ecs.progress();
    // Advance to next frame. Process submitted rendering primitives.
    bgfx::frame();
  }
  ddShutdown();
  bgfx::shutdown();
  app_terminate();
  return 0;
}
