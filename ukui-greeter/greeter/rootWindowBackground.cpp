#include <stdio.h>
#include <X11/Xlib.h>
#include <Imlib2.h>

#include "rootWindowBackground.h"

void setRootWindowBackground(int width,int height,char *filename)
{
    Imlib_Image img;
    Display *dpy;
    Pixmap pix;
    Window root;
    Screen *scn;

    img = imlib_load_image(filename);
    if (!img) {
        fprintf(stderr, "%s:Unable to load image\n", filename);
    	return ;
    }
    imlib_context_set_image(img);

    dpy = XOpenDisplay(NULL);
    if (!dpy)
        return ;
    scn = DefaultScreenOfDisplay(dpy);
    root = DefaultRootWindow(dpy);

    pix = XCreatePixmap(dpy, root, width, height,
        DefaultDepthOfScreen(scn));

    imlib_context_set_display(dpy);
    imlib_context_set_visual(DefaultVisualOfScreen(scn));
    imlib_context_set_colormap(DefaultColormapOfScreen(scn));
    imlib_context_set_drawable(pix);

    imlib_render_image_on_drawable_at_size(0, 0, width, height);
    XSetWindowBackgroundPixmap(dpy, root, pix);
    XClearWindow(dpy, root);

    while (XPending(dpy)) {
        XEvent ev;
        XNextEvent(dpy, &ev);
    }
    XFreePixmap(dpy, pix);
    imlib_free_image();
    XCloseDisplay(dpy);
    return ;
}
