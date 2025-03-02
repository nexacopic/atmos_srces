#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>

void draw_window(Display *dpy, Window win, GC gc, int screen_width, int screen_height) {
    // Set the window background to a color (light grey in this case)
    XSetForeground(dpy, gc, WhitePixel(dpy, 0));
    XFillRectangle(dpy, win, gc, 0, 0, screen_width, screen_height);
}

int main() {
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) {
        printf("error: Cannot open display\n");
        return 1;
    }

    Window root = DefaultRootWindow(dpy);
    Screen *screen = DefaultScreenOfDisplay(dpy);
    int screen_width = screen->width;
    int screen_height = screen->height;
    
    // Create a graphics context for drawing
    GC gc = XCreateGC(dpy, root, 0, NULL);

    // Redirect window management to our program
    XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask | FocusChangeMask);
    XSync(dpy, False);

    printf("COCOA DREAMY1 Good Morning\n");

    // Event loop
    while (1) {
        XEvent ev;
        XNextEvent(dpy, &ev);

        switch (ev.type) {
            case MapRequest: {
                XMapWindow(dpy, ev.xmaprequest.window);
                break;
            }
            case FocusIn: {
                // Handle focus event, mark the focused window
                Window focused_window = ev.xfocus.window;
                printf("Window focused: %lu\n", focused_window);
                break;
            }
            case Expose: {
                // Draw the window when exposed
                if (ev.xexpose.count == 0) {
                    Window win = ev.xexpose.window;
                    draw_window(dpy, win, gc, screen_width, screen_height);
                }
                break;
            }
            default:
                break;
        }
    }

    XFreeGC(dpy, gc);
    XCloseDisplay(dpy);
    return 0;
}
