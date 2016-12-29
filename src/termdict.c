#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "util.h"

#include <sys/time.h>

const char *Yes     = "YES";
const char *No      = "NO";
const char *Unknown = "UNKNOWN";

typedef unsigned char uchar;

//static void selrequest(XEvent *);
static void selnotify(XEvent *);
static void debounce(uchar *);

//struct timespec spec;


static void (*handler[LASTEvent])(XEvent *) = {
    [KeyPress] = noop,
    [ClientMessage] = noop,
    [ConfigureNotify] = noop,
    [VisibilityNotify] = noop,
    [UnmapNotify] = noop,
    [Expose] = noop,
    [FocusIn] = noop,
    [FocusOut] = noop,
    [MotionNotify] = noop,
    [ButtonPress] = noop,
    [ButtonRelease] = noop,
    [SelectionClear] = noop,
    [SelectionNotify] = selnotify,
    [SelectionRequest] = noop,
    //[SelectionRequest] = selrequest,
};


Display *dpy;
Window ww;

struct timeval  tv;

int
main()
{
    //clock_gettime(CLOCK_REALTIME, &spec);
    gettimeofday(&tv, NULL);


    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "XOpenDisplay failed!\n");
        abort();
    }

    Window w;
    XGetInputFocus(dpy, &w, &(int){0}); // see man
    if(w == None) {
        fprintf(stderr, "no focus window\n");
        XCloseDisplay(dpy);
        return 1;
    }
    
    //Window root= XDefaultRootWindow(dpy);
    //XGetInputFocus(dpy, &w, &(int){0}); // see man
    // Own clipboards
    XSetSelectionOwner(dpy, XA_PRIMARY, w, CurrentTime);
    //XSetSelectionOwner(dpy, XInternAtom(dpy, "CLIPBOARD", 0), w, CurrentTime);
    //XFlush(dpy);
    
    //Window interClientCommunicationWindow;

    //interClientCommunicationWindow = XCreateSimpleWindow(dpy, RootWindow(dpy, 0), 10, 10, 10, 10, 0, 0, 0);
    //XSelectInput(dpy, interClientCommunicationWindow, StructureNotifyMask);



    //const int scr = XDefaultScreen(dpy);
    //ww = XCreateSimpleWindow(dpy, RootWindow(dpy, scr), 10, 10, 100, 100, 1,
    //                        BlackPixel(dpy, scr), WhitePixel(dpy, scr));
    //ww = XCreateSimpleWindow(dpy, RootWindow(dpy, 0), 1, 1, 256, 256, 0, 
    //                        BlackPixel (dpy, 0), BlackPixel(dpy, 0));
    // create a dummy window, that we can use to end the blocking XNextEvent call
    ww = XCreateSimpleWindow(dpy, RootWindow(dpy, 0), 10, 10, 10, 10, 0, 0, 0);
    //XMapWindow(dpy, ww);

    const Atom xtarget = XInternAtom(dpy, "UTF8_STRING", 0),
        clipboard = XInternAtom(dpy, "PRIMARY", 0);



    XEvent ev;
    while (true) {
        /*
        XGetInputFocus(dpy, &w, &(int){0}); // see man
        XSetSelectionOwner(dpy, XA_PRIMARY, w, CurrentTime);
        */

        XNextEvent(dpy, &ev);
        //printf("event received: %d\n", ev.type);

        if(ev.type == 29) {
            //printf("@SHIT\n");

            XConvertSelection(dpy, clipboard, xtarget, XA_PRIMARY, ww, CurrentTime);
            XFlush(dpy);
            //XNextEvent(dpy, &ev);
            //printf("event received: %d\n", ev.type);
           
            XGetInputFocus(dpy, &w, &(int){0}); // see man
            XSetSelectionOwner(dpy, XA_PRIMARY, w, CurrentTime);
        }

        if(XFilterEvent(&ev, None))
            continue;
        if(handler[ev.type]) (handler[ev.type])(&ev);
    }

    return 0;
}

/*
static void
selrequest(XEvent *ev)
{
    printf("selrequest\n");
    XSelectionRequestEvent *xsre;
    XSelectionEvent xev;

    xsre = (XSelectionRequestEvent *)ev;
    xev.type = SelectionNotify;
    xev.requestor = xsre->requestor;
    xev.selection = xsre->selection;
    xev.target = xsre->target;
    xev.time = xsre->time;
    xev.property = None;

    // ripped from xev
    {
        const XAnyEvent *const e = (XAnyEvent *) ev;
        printf ("\tSelectionRequest event, type %d, serial %ld, synthetic %s, window 0x%lx\n",
            e->type, e->serial, e->send_event ? Yes : No, e->window);
    }

    {
        const XSelectionRequestEvent *const e = (XSelectionRequestEvent *) ev;
        char *sname = XGetAtomName (dpy, e->selection);
        char *tname = XGetAtomName (dpy, e->target);
        char *pname = XGetAtomName (dpy, e->property);

        printf ("\t    owner 0x%lx, requestor 0x%lx, selection 0x%lx (%s),\n",
            e->owner, e->requestor, e->selection, sname ? sname : Unknown);
        printf ("\t    target 0x%lx (%s), property 0x%lx (%s), time %lu\n\n",
            e->target, tname ? tname : Unknown, e->property,
            pname ? pname : Unknown, e->time);

        XFree (sname);
        XFree (tname);
        XFree (pname);
    }
    // -- xev

    assert(xsre->target == XInternAtom(dpy, "UTF8_STRING", 0));

    const char msg[] = "hello world!";
    XChangeProperty(xsre->display, xsre->requestor, xsre->property,
            xsre->target, 8, PropModeReplace,
            (uchar *) msg, strlen(msg));
    xev.property = xsre->property;

    if (!XSendEvent(xsre->display, xsre->requestor, True, 0, (XEvent *) &xev)) {
        fprintf(stderr, "Error sending SelectionNotify event\n");
    }
}
*/

static void
selnotify(XEvent *const eventp)
{
    //printf("selnotify!\n");
    
    long nitems, rem;
    int format;
    uchar *data;
    Atom type;

    
    if(XGetWindowProperty(dpy, ww, XA_PRIMARY, 0, 32, False, AnyPropertyType,
                &type, &format, &nitems, &rem, &data)) {
        fprintf(stderr, "Clipboard allocation failed\n");
        return;
    }
    
    //assert(0 == rem);

    //{
    //    const XAnyEvent *const ev = (XAnyEvent *)eventp;
        //printf ("\tSelectionNotify event, type %d, serial %ld, synthetic %s, window 0x%lx, time %lu\n",
        //    ev->type, ev->serial, ev->send_event ? Yes : No, ev->window, ((XSelectionEvent *)eventp)->time);
    //}
    // -- xev
    debounce(data);
}


static void debounce(uchar *data) {

    int tmp = tv.tv_sec;
    //clock_gettime(CLOCK_REALTIME, &spec);
    gettimeofday(&tv, NULL);
    int now = tv.tv_sec;
    
    if(now - tmp > 0) {
        //printf("%s\n", data);
        fprintf(stdout, "%s\n", data);
        fflush(stdout);
    }
        //printf("\tdata: %s\n\n", data);
    //printf("tttttttttttttt %d  ttttttt \n", now);
    //printf("tttttttttttttt %d  ttttttt \n", tmp);
}
