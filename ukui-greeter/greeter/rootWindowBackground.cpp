/* rootWindowBackground.cpp
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
**/

#include <QScreen>
#include <QX11Info>
#include <QRect>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <stdio.h>
#include <X11/Xlib.h>
#include <Imlib2.h>

#include "rootWindowBackground.h"

void setRootWindowBackground(char *filename)
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
    int width = 0,height = 0;
   
    width = QApplication::desktop()->geometry().width();
    height = QApplication::desktop()->geometry().height();
 
    scn = DefaultScreenOfDisplay(dpy);
    root = DefaultRootWindow(dpy);

    pix = XCreatePixmap(dpy, root, width, height,
        DefaultDepthOfScreen(scn));

    imlib_context_set_display(dpy);
    imlib_context_set_visual(DefaultVisualOfScreen(scn));
    imlib_context_set_colormap(DefaultColormapOfScreen(scn));
    imlib_context_set_drawable(pix);

    for(QScreen *screen : QApplication::screens()){
        //在每个屏幕上绘制背景
        QRect rect = screen->geometry();
        imlib_render_image_on_drawable_at_size(rect.x(), rect.y(), rect.width(),rect.height());
    }
 
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
