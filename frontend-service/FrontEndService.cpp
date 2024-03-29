#include "../gekkota.h"
#include "xdg-shell-client-protocol.h"
#include "wayland_helpers.c"
#include "shm.c"

struct wl_buffer *FrontEndService::createBuffer() {
  int stride = width * 4;
  int size = stride * height;

  int fd = create_shm_file(size);
  if (fd < 0) {
    fprintf(stderr, "creating a buffer file for %d B failed: %m\n", size);
    return NULL;
  }

  void *shm_data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (shm_data == MAP_FAILED) {
    fprintf(stderr, "mmap failed: %m\n");
    close(fd);
    return NULL;
  } else {
    printf("data allocated behind fd at address %ld\n", shm_data);
  }

  struct wl_shm_pool *pool = wl_shm_create_pool(this->fe_global.shm, fd, size);
  struct wl_buffer *buffer = wl_shm_pool_create_buffer(
      pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888
  );
  wl_shm_pool_destroy(pool);

  this->fe_global.shm_data = shm_data;
  this->fe_global.shm_data_size = size;
  this->fe_global.shm_data_stride = stride;
  return buffer;
}

FrontEndService::FrontEndService() {}
void FrontEndService::Init() {
  this->running = 1;
  cout << "setting up frontend " << endl;

  this->fe_global = {
      .data = nullptr,
      .shm_data = nullptr,
      .shm_data_size = 0,
      .shm_data_stride = 0,
      .display = nullptr,
      .shm = nullptr,
      .compositor = nullptr,
      .surface = nullptr,
      .xdg_surface = nullptr,
      .xdg_wm_base = nullptr,
      .xdg_toplevel = nullptr,
  };

  setup_keyboard();
  setup_screen(this);
  setup_surface(this);

  struct wl_buffer *buffer = this->createBuffer();
  if (buffer == NULL) {
    exit(1);
  }
  set_cursor_from_pool(this, height, width, 10, 20, buffer);

  this->_img = new BLImage();
  this->_img->createFromData(
      width, height, BL_FORMAT_PRGB32, this->fe_global.shm_data,
      this->fe_global.shm_data_stride
  );
  this->uictx = new BLContext(*this->_img);
  this->uictx->setCompOp(BL_COMP_OP_SRC_COPY);
  this->uictx->fillAll();

  this->uictx->setCompOp(BL_COMP_OP_SRC_COPY);
  this->uictx->fillAll();

  this->buffer = buffer;
  commit_ui(this, buffer);
}

void FrontEndService::commit() {
  this->uictx->end();
  commit_ui(this, buffer);
}

void FrontEndService::teardown() {
  cout << "tearing down frontend " << endl;
  gka_wayland_tear_down(this->buffer);
};

void FrontEndService::loop() {
  while (wl_display_dispatch(this->fe_global.display) != -1 && this->running) {
    // This intentially left blank
  }
  cout << "end front end loop" << endl;
};

FrontEndService *FrontEndService::_instance = nullptr;
FrontEndService *FrontEndService::getInstance() {
  if (_instance == nullptr) {
    _instance = new FrontEndService();
    cout << "allocating the fe" << endl;
  }
  return _instance;
}