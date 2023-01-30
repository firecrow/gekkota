void setup_keyboard() { xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS); }

void set_cursor_from_pool(
    FrontEndService *fe, unsigned width, unsigned height, int32_t hot_spot_x,
    int32_t hot_spot_y, struct wl_buffer *buffer
) {
  struct pointer_data *data;

  data = (struct pointer_data *)malloc(sizeof(struct pointer_data));

  if (data == NULL)
    goto error;

  data->hot_spot_x = hot_spot_x;
  data->hot_spot_y = hot_spot_y;
  data->surface = fe->fe_global.surface;

  if (data->surface == NULL)
    goto cleanup_alloc;

  data->buffer = buffer;

  if (data->buffer == NULL)
    goto cleanup_surface;

  return;

cleanup_surface:
  wl_surface_destroy(data->surface);
cleanup_alloc:
  free(data);
error:
  perror("Unable to allocate cursor");
}

static void pointer_handle_motion(
    void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t x,
    wl_fixed_t y
) {
  printf("motion: %d %d\n", (int)x, (int)y);
}

static void pointer_handle_button(
    void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time,
    uint32_t button, uint32_t state
) {

  struct pointer_data *pointer_data;
  void (*callback)(uint32_t);

  if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED) {
    printf("clicked..\n");
  }
}

static void keyboard_enter(
    void *data, struct wl_keyboard *keyboard, uint32_t serial,
    struct wl_surface *surface, struct wl_array *keys
) {}

static void keyboard_leave(
    void *data, struct wl_keyboard *keyboard, uint32_t serial,
    struct wl_surface *surface
) {}

static void keyboard_modifiers(
    void *data, struct wl_keyboard *keyboard, uint32_t serial,
    uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
    uint32_t group
) {
  xkb_state_update_mask(
      xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group
  );
}

static void keyboard_keymap(
    void *data, struct wl_keyboard *keyboard, uint32_t format, int32_t fd,
    uint32_t size
) {
  char *keymap_string = (char *)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
  xkb_keymap_unref(keymap);

  keymap = xkb_keymap_new_from_string(
      xkb_context, keymap_string, XKB_KEYMAP_FORMAT_TEXT_V1,
      XKB_KEYMAP_COMPILE_NO_FLAGS
  );
  munmap(keymap_string, size);
  close(fd);

  xkb_state_unref(xkb_state);
  xkb_state = xkb_state_new(keymap);
}

static void keyboard_key(
    void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time,
    uint32_t key, uint32_t state
) {
  if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
    xkb_keysym_t keysym = xkb_state_key_get_one_sym(xkb_state, key + 8);
    uint32_t utf32 = xkb_keysym_to_utf32(keysym);
    if (utf32) {
      if (utf32 >= 0x21 && utf32 <= 0x7E) {
        printf("the key %c was pressed\n", (char)utf32);
        if (utf32 == 'q')
          FrontEndService::getInstance()->running = 0;
      } else {
        printf("the key U+%04X was pressed\n", utf32);
      }
    } else {
      char name[64];
      xkb_keysym_get_name(keysym, name, 64);
      printf("the key %s was pressed\n", name);
    }
  }
}

/************** set up events ***************/

void pointer_handle_enter(
    void *, struct wl_pointer *, uint32_t, struct wl_surface *, wl_fixed_t,
    wl_fixed_t
) {}
void pointer_handle_leave(void *, struct wl_pointer *, uint32_t, struct wl_surface *) {
}
void pointer_handle_axis(
    void *, struct wl_pointer *, uint32_t, uint32_t, wl_fixed_t
){};

static const struct wl_pointer_listener pointer_listener = {
    .enter = pointer_handle_enter,
    .leave = pointer_handle_leave,
    .motion = pointer_handle_motion,
    .button = pointer_handle_button,
    .axis = pointer_handle_axis,
};

static struct wl_keyboard_listener keyboard_listener = {
    .keymap = keyboard_keymap,
    .enter = keyboard_enter,
    .leave = keyboard_leave,
    .key = keyboard_key,
    .modifiers = keyboard_modifiers,
};

static void seat_handle_capabilities(
    void *data, struct wl_seat *seat, uint32_t capabilities
) {
  if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
    struct wl_pointer *pointer = wl_seat_get_pointer(seat);
    wl_pointer_add_listener(pointer, &pointer_listener, seat);
  }
  if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
    struct wl_keyboard *keyboard = wl_seat_get_keyboard(seat);
    wl_keyboard_add_listener(keyboard, &keyboard_listener, NULL);
  }
}

static const struct wl_seat_listener seat_listener = {
    .capabilities = seat_handle_capabilities,
};

static void
handle_global_remove(void *data, struct wl_registry *registry, uint32_t name) {}

static void handle_global(
    void *data, struct wl_registry *registry, uint32_t name,
    const char *interface, uint32_t version
) {

  if (strcmp(interface, wl_shm_interface.name) == 0) {
    FrontEndService::getInstance()->fe_global.shm =
        (struct wl_shm *)wl_registry_bind(registry, name, &wl_shm_interface, 1);

  } else if (strcmp(interface, wl_seat_interface.name) == 0) {
    struct wl_seat *seat =
        (wl_seat *)wl_registry_bind(registry, name, &wl_seat_interface, 1);
    wl_seat_add_listener(seat, &seat_listener, NULL);

  } else if (strcmp(interface, wl_compositor_interface.name) == 0) {
    FrontEndService::getInstance()->fe_global.compositor =
        (struct wl_compositor *)wl_registry_bind(
            registry, name, &wl_compositor_interface, 1
        );

  } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    FrontEndService::getInstance()->fe_global.xdg_wm_base =
        (struct xdg_wm_base *)wl_registry_bind(
            registry, name, &xdg_wm_base_interface, 1
        );
  }
}

static const struct wl_registry_listener registry_listener = {
    .global = handle_global,
    .global_remove = handle_global_remove,
};

void setup_screen(FrontEndService *fe) {
  struct wl_display *display = wl_display_connect(NULL);
  if (display == NULL) {
    fprintf(stderr, "failed to create display\n");
    exit(1);
  }

  struct wl_registry *registry = wl_display_get_registry(display);
  wl_registry_add_listener(registry, &registry_listener, NULL);
  wl_display_roundtrip(display);

  if (fe->fe_global.shm == NULL || fe->fe_global.compositor == NULL ||
      fe->fe_global.xdg_wm_base == NULL) {
    fprintf(stderr, "no wl_shm, wl_compositor or xdg_wm_base support\n");
    exit(1);
  }
  fe->fe_global.display = display;
}

/************** set up surface/shm ***************/
static void xdg_surface_handle_configure(
    void *data, struct xdg_surface *xdg_surface, uint32_t serial
) {
  xdg_surface_ack_configure(
      FrontEndService::getInstance()->fe_global.xdg_surface, serial
  );
  wl_surface_commit(FrontEndService::getInstance()->fe_global.surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_handle_configure,
};

static void
xdg_toplevel_handle_close(void *data, struct xdg_toplevel *xdg_toplevel) {
  FrontEndService::getInstance()->running = 0;
}

void xdg_configure_handle(void *, struct xdg_toplevel *, int32_t, int32_t, struct wl_array *) {
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_configure_handle,
    .close = xdg_toplevel_handle_close,
};
/************** manage lifecycle ***************/

void setup_surface(FrontEndService *fe) {
  fe->fe_global.surface =
      wl_compositor_create_surface(fe->fe_global.compositor);
  fe->fe_global.xdg_surface = xdg_wm_base_get_xdg_surface(
      fe->fe_global.xdg_wm_base, fe->fe_global.surface
  );
  fe->fe_global.xdg_toplevel =
      xdg_surface_get_toplevel(fe->fe_global.xdg_surface);

  xdg_surface_add_listener(
      fe->fe_global.xdg_surface, &xdg_surface_listener, NULL
  );
  xdg_toplevel_add_listener(
      fe->fe_global.xdg_toplevel, &xdg_toplevel_listener, NULL
  );
}

void commit_ui(FrontEndService *fe, struct wl_buffer *buffer) {
  wl_surface_commit(fe->fe_global.surface);

  wl_display_roundtrip(fe->fe_global.display);

  wl_surface_attach(fe->fe_global.surface, buffer, 0, 0);
  wl_surface_commit(FrontEndService::getInstance()->fe_global.surface);
}

void gka_wayland_tear_down(struct wl_buffer *buffer) {
  FrontEndService *fe = FrontEndService::getInstance();
  xdg_toplevel_destroy(fe->fe_global.xdg_toplevel);
  xdg_surface_destroy(fe->fe_global.xdg_surface);
  wl_surface_destroy(fe->fe_global.surface);
  wl_buffer_destroy(buffer);
}
