#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>

void draw_root_window(Display *dpy, Window root, GC gc, int screen_width, int screen_height) {
    XSetForeground(dpy, gc, WhitePixel(dpy, 0));
    XFillRectangle(dpy, root, gc, 0, 0, screen_width, screen_height);
}

void enforce_window_size(Display *dpy, Window win) {
    XWindowAttributes attrs;
    if (XGetWindowAttributes(dpy, win, &attrs)) {
        if (attrs.width <= 0 || attrs.height <= 0) {
            XResizeWindow(dpy, win, 500, 500);
        }
    }
}

void manage_existing_windows(Display *dpy, Window root) {
    Window root_return, parent_return, *children;
    unsigned int nchildren;
    
    if (XQueryTree(dpy, root, &root_return, &parent_return, &children, &nchildren)) {
        for (unsigned int i = 0; i < nchildren; i++) {
            Window win = children[i];
            XWindowAttributes attrs;
            
            if (XGetWindowAttributes(dpy, win, &attrs) && attrs.override_redirect)
                continue;

            XMapWindow(dpy, win);
            enforce_window_size(dpy, win);
            XRaiseWindow(dpy, win);
        }
        XFree(children);
    }
}

int main() {
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) return 1;

    Window root = DefaultRootWindow(dpy);
    Screen *screen = DefaultScreenOfDisplay(dpy);
    int screen_width = screen->width;
    int screen_height = screen->height;
    
    GC gc = XCreateGC(dpy, root, 0, NULL);
    XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask | FocusChangeMask);
    
    manage_existing_windows(dpy, root);
    draw_root_window(dpy, root, gc, screen_width, screen_height);
    XSync(dpy, False);

    printf("COCOA DREAMY1 Good Morning\n");

    while (1) {
        XEvent ev;
        XNextEvent(dpy, &ev);

        switch (ev.type) {
            case MapRequest: {
                Window child = ev.xmaprequest.window;
                XMapWindow(dpy, child);
                enforce_window_size(dpy, child);
                XRaiseWindow(dpy, child);
                break;
            }
            case ConfigureRequest: {
                XConfigureRequestEvent *cre = &ev.xconfigurerequest;
                XWindowChanges wc = {
                    .x = cre->x,
                    .y = cre->y,
                    .width = (cre->width > 0) ? cre->width : 500,
                    .height = (cre->height > 0) ? cre->height : 500,
                    .border_width = cre->border_width,
                    .sibling = cre->above,
                    .stack_mode = cre->detail
                };
                XConfigureWindow(dpy, cre->window, cre->value_mask, &wc);
                break;
            }
            case Expose: {
                if (ev.xexpose.window == root && ev.xexpose.count == 0) {
                    draw_root_window(dpy, root, gc, screen_width, screen_height);
                }
                break;
            }
            case FocusIn: {
                printf("Window focused: %lu\n", ev.xfocus.window);
                break;
            }
            default: break;
        }
    }

    XFreeGC(dpy, gc);
    XCloseDisplay(dpy);
    return 0;
}