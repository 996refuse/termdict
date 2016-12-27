#include <stdbool.h>

void
noop(XEvent *ev);

void
print_properties(Display *const d, Window w);

bool
print_window_name(Display* d, Window w);

bool
print_window_class(Display* d, Window w);

bool
print_window_info(Display* d, Window w);

bool
window_id_to_pid(Display *const dpy, Window w, unsigned long *pid);

bool
get_process_name_from_window(Display *const d, Window w, char **name);

