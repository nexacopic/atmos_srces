#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) {
        printf("Error: Cannot open display\n");
        return 1;
    }

    Window root = DefaultRootWindow(dpy);

    // Redirect window management to our program
    XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask);
    XSync(dpy, False);

    printf("Simple Xlib window manager running...\n");

    while (1) {
        XEvent ev;
        XNextEvent(dpy, &ev);
        if (ev.type == MapRequest) {
            XMapWindow(dpy, ev.xmaprequest.window);
        }
    }

    XCloseDisplay(dpy);
    return 0;
}
