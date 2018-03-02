/*
This file is part of LightDM-KDE.

Copyright 2011, 2012 David Edmundson <kde@davidedmundson.co.uk>

LightDM-KDE is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LightDM-KDE is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with LightDM-KDE.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "rootimageapp.h"
#include <QApplication>
#include <QPixmap>
#include <QX11Info>
#include <QDesktopWidget>
#include <QTimer>
#include <QDebug>
#include <QFile>

#include <X11/Xlib.h>
#include <X11/cursorfont.h>

RootImageApp::RootImageApp(int &argc, char **argv):
    QApplication(argc, argv)
{
    XDefineCursor(QX11Info::display(), QX11Info::appRootWindow(), XCreateFontCursor(QX11Info::display(), XC_circle));

    QTimer::singleShot(0, this, SLOT(setBackground()));
}

void RootImageApp::setBackground()
{
    QFile stdIn;
    stdIn.open(0, QIODevice::ReadOnly);

    QImage image;
    image.load(&stdIn, "xpm");
    XGrabServer(QX11Info::display());

    QPalette palette;
    palette.setBrush(desktop()->backgroundRole(), QBrush(image));
    desktop()->setPalette(palette);
    XClearWindow(QX11Info::display(), desktop()->winId());

    quit();
}

int
main(int argc, char *argv[])
{
    RootImageApp app(argc, argv);
    XSetCloseDownMode(QX11Info::display(), RetainTemporary);

    app.exec();
    app.flush();

    return 0;
}
