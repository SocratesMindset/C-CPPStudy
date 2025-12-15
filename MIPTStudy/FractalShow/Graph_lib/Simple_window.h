#ifndef SIMPLE_WINDOW_GUARD
#define SIMPLE_WINDOW_GUARD

#include "GUI.h"

namespace Graph_lib {

// Basic scaffolding for one-window interaction with a single "Next" button.
struct Simple_window : Window {
  Simple_window(Point xy, int w, int h, const string& title);

  // Run a minimal event loop until the "Next" button is pressed.
  bool wait_for_button();

 private:
  Button next_button;
  bool button_pushed;

  static void cb_next(Address, Address pw);
  void next();
};

}  // namespace Graph_lib

#endif
