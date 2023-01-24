#include "gekkota.h"

Engine _engine;
Engine &Engine::instance = {_engine};

using namespace std;

Engine::Engine() {}

double *
Engine::render(vector<struct gka_entry *> blocks, int count, uint32_t rate) {
  // TODO: make this a round robin lottery to balance handlers
  RenderHandler *handler = this->handlers.front();

  Title *title = &Title::instance;
  gka_time_t elapsed = gka_now() - title->start_time;

  vector<thread *> thd;
  vector<RenderHandler *> hinst;

  // crunch all the numbers:
  for (struct gka_entry *m : blocks) {
    RenderHandler *h = handler->makeInstance(m, count, rate);

    thd.push_back(new thread(h->getAction(elapsed)));
    hinst.push_back(h);
  }

  for (thread *t : thd) {
    t->join();
  }

  // gather all the framesets:
  RenderFinalizer finalizer = RenderFinalizer(count);
  for (RenderHandler *h : hinst) {
    finalizer.marryToDest(h->dest);
    delete h;
  }
  return finalizer.dest;
}