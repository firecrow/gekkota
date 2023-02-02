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

/**
 * This is a callback function convention used by the wayland api's to send
 * keyboard events for the active window
 */
typedef void (*on_key_func
)(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time,
  uint32_t key, uint32_t state);

/**
 * The front end service is a class that manages the front end window manager,
 * This is presently Wayland on Linux
 *
 */
class FrontEndService {
public:
  /**
   * These functions and instnaces are for global access, usually used for areas
   * of the application to draw diagrams and propogate visualization events when
   * certain actions happen or data is available
   */
  static FrontEndService *getInstance();
  static FrontEndService *_instance;
  FrontEndService();
  /**
   * This function initializes the screen properties from Wayland and the
   * keyboard properties from XKeyboard, as well as the drawing api Blend2D
   * which is used extensively in plot-visual
   */
  void Init();
  /**
   * This is the loop that listens for events and updates to the screen, this is
   * launched in a the foreground and treated as the main thread of the
   * application
   */
  void loop();
  /**
   * This function instnantiates the buffer for committing the pixels in memory
   * onto the screen
   */
  struct wl_buffer *createBuffer();
  /**
   * tear down and cleanup memory
   */
  void teardown();
  /**
   * This function commits changes from the buffer to the screen
   */
  void commit();
  /**
   * Variable used to determine if certain repetative events should continue
   */
  bool running;
  /**
   * These are components used by Wayland callbacks which are global callbacks
   * into the Wayland framework
   */
  struct gka_wl_components fe_global;
  /**
   * Local buffer stored for presenting information to the screen
   */
  wl_buffer *buffer;
  /**
   * This is the Blend2D context used for drawing visulizations
   */
  BLContext *uictx;
  /**
   * This is the Blend2D image object which sits on top of the buffer to place
   * Blend2D designs into the buffer so that the commit function can present
   * them to the screen using Wayland
   */
  BLImage *_img;
};