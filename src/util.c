#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define __USE_XOPEN_EXTENDED //strdup
#include <string.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "util.h"

void
noop(XEvent *ev)
{
    ;//printf("noop!\n");
}

void
print_properties(Display *const dpy, Window w)
{
    int prop_count;
    Atom *const atoms = XListProperties(dpy, w, &prop_count);
    printf("property count: %d\n", prop_count);
    for (int i = 0; i < prop_count; ++i) {
        unsigned long rem;
        int format;
        unsigned char *xdata;
        char *data = NULL;

        if (Success == XGetWindowProperty(dpy, w, atoms[i], 0, 32, False, AnyPropertyType,
                    &(Atom){0}, &format, &(unsigned long){0}, &rem, &xdata)) {
            if (8 == format) {
                data = (NULL == xdata) ? strdup("<null>") : strdup((char *)xdata);
            } else {
                data = strdup("< non-char data >");
            }
            XFree(xdata);
        } else {
            data = strdup("<Failed to read property value>");
        }

        printf("[%d]: %s => %s%s\n", i, XGetAtomName(dpy, atoms[i]), data ? data : "< ??? >", rem ? "..." : "");
        free(data);
    }

    XFree(atoms);
}

// https://gist.github.com/kui/2622504
// (XFetchName cannot get a name with multi-byte chars)
bool
print_window_name(Display* d, Window w)
{
    XTextProperty prop;

    printf("window name:\n");

    if (!XGetWMName(d, w, &prop)) {
        fprintf(stderr, "XGetWMName failed\n");
        return false;
    }

    // for XmbTextPropertyToTextList
    // setlocale(LC_ALL, ""); // see man locale

    char **list = NULL;
    if (Success != XmbTextPropertyToTextList(d, &prop, &list, &(int){0})) {
        fprintf(stderr, "XmbTextPropertyToTextList failed\n");
        return false;
    }

    printf("\t%s\n", list[0]);
    XFreeStringList(list);
    return true;
}

// https://gist.github.com/kui/2622504
bool
print_window_class(Display* d, Window w)
{
    XClassHint *const class = XAllocClassHint(); // see man
    if (!class) {
        fprintf(stderr, "XAllocClassHint failed!\n");
        return false;
    }

    const Status s = XGetClassHint(d, w, class); // see man
    if (!s) {
        fprintf(stderr, "XGetClassHint failed\n");
        return false;
    }

    printf("application:\n\tname: %s\n\tclass: %s\n", class->res_name, class->res_class);
    return true;
}

bool
get_process_name_from_window(Display *const d, Window w, char **const name)
{
    assert(d && name);

    XClassHint *const class = XAllocClassHint(); // see man
    if (!class) {
        fprintf(stderr, "XAllocClassHint failed!\n");
        return false;
    }

    const Status s = XGetClassHint(d, w, class); // see man
    if (!s) {
        fprintf(stderr, "XGetClassHint failed\n");
        return false;
    }

    *name = strdup(class->res_class);

    XFree(class->res_name);
    XFree(class->res_class);
    return true;
}

// https://gist.github.com/kui/2622504
bool
print_window_info(Display* d, Window w)
{
    printf("--\n");
    if (false == print_window_name(d, w)) {
        fprintf(stderr, "print_window_name failed\n");
        return false;
    }
    if (false == print_window_class(d, w)) {
        fprintf(stderr, "print_window_class failed\n");
        return false;
    }

    return true;
}

bool
window_id_to_pid(Display *const dpy, Window w, unsigned long *pid)
{
    unsigned long nitems, rem;
    int format;
    Atom type;
    unsigned char *data;

    Atom status = XGetWindowProperty(dpy, w, XInternAtom(dpy, "_NET_WM_PID", 0), 0, sizeof(*pid), False, XA_CARDINAL,
        &type, &format, &nitems, &rem, &data);
    if (Success != status) {
        return false;
    }

    if (!((XA_CARDINAL == type) && (32 == format) && (1 == nitems) && (0 == rem))) {
        fprintf(stderr, "0x%lX: unexpected data format from XGetWindowProperty for _NET_WM_PID\n", w);
        XFree(data);
        return false;
    }

    *pid = ((unsigned long *)data)[0];
    XFree(data);
    return true;
}

