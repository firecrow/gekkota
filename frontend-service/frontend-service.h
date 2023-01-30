#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <wayland-server-protocol.h>
#include <wayland-cursor.h>
#include <blend2d.h>
#include <xkbcommon/xkbcommon.h>
#include <linux/input-event-codes.h>

void setup_keyboard();
void commit_ui(struct wl_buffer *buffer);
void setup_surface();
void set_cursor_from_pool(
    unsigned width, unsigned height, int32_t hot_spot_x, int32_t hot_spot_y,
    struct wl_buffer *buffer
);
void gka_wayland_tear_down(struct wl_buffer *buffer);

extern struct wl_pointer *pointer;
struct pointer_data {
  struct wl_surface *surface;
  struct wl_buffer *buffer;
  int32_t hot_spot_x;
  int32_t hot_spot_y;
  struct wl_surface *target_surface;
};

int create_shm_file(off_t size);
static const int width = 1024;
static const int height = 480;

static void noop() {
  // This space intentionally left blank
}

static struct xkb_state *xkb_state = NULL;
static struct xkb_context *xkb_context;
static struct xkb_keymap *keymap = NULL;
struct gka_wl_components {
  void *data;
  void *shm_data;
  size_t shm_data_size;
  int shm_data_stride;
  struct wl_display *display;
  struct wl_shm *shm;
  struct wl_compositor *compositor;
  struct wl_surface *surface;
  struct xdg_surface *xdg_surface;
  struct xdg_wm_base *xdg_wm_base;
  struct xdg_toplevel *xdg_toplevel;
};

typedef void (*on_key_func
)(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time,
  uint32_t key, uint32_t state);

void run_audio_output();

class FrontEndService {
public:
  static FrontEndService *getInstance();
  static FrontEndService *_instance;
  FrontEndService();
  void Init();
  void loop();
  struct wl_buffer *createBuffer();
  void teardown();
  void drawDemo();
  void commit();
  void plotPeriodData(double *frames);
  bool running;
  struct gka_wl_components fe_global;
  wl_buffer *buffer;
  BLContext *uictx;
  BLImage *_img;
};