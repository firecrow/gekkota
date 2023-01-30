#include "gekkota.h"

Engine _engine;
Engine &Engine::instance = {_engine};

using namespace std;

Engine::Engine() {}

void Engine::render(
    RenderFinalizer *finalizer, vector<struct gka_entry *> blocks, int count,
    uint32_t rate
) {
  // TODO: make this a round robin lottery to balance handlers
  RenderHandler *handler = this->handlers.front();

  Title *title = &Title::instance;
  gka_time_t elapsed = gka_now() - title->start_time;

  vector<RenderHandler *> hinst;

  for (struct gka_entry *m : blocks) {
    RenderHandler *h = handler->makeInstance(m, count, rate);

    h->render(elapsed);
    hinst.push_back(h);
  }

  // gather all the framesets:
  for (RenderHandler *h : hinst) {
    finalizer->marryToDest(h->dest);
    delete h;
  }
}