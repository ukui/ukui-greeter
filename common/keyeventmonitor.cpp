/*
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
**/
#include "keyeventmonitor.h"
#include <X11/Xlibint.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <stdio.h>
#include <stdlib.h>

KeyEventMonitor *KeyEventMonitor::instance_ = nullptr;

KeyEventMonitor::KeyEventMonitor(QObject *parent)
    : QThread(parent),
      hasModifer(false)
{

}

KeyEventMonitor *KeyEventMonitor::instance(QObject *parent)
{
    if(instance_ == nullptr)
        instance_ = new KeyEventMonitor(parent);

    return instance_;
}

KeyEventMonitor::~KeyEventMonitor()
{
    requestInterruption();
    terminate();
    wait();
}

bool KeyEventMonitor::checkCapsState()
{
    //判断大写键状态
    Display *display = XOpenDisplay(NULL);
    bool capsState = false;
    if(display) {
        unsigned int n;
        XkbGetIndicatorState(display, XkbUseCoreKbd, &n);
        capsState = (n & 0x01) == 1;
    }
    return capsState;
}

void KeyEventMonitor::run()
{
    isCapsLock = checkCapsState();

    while(!isInterruptionRequested()) {
	// 创建记录 XRecord 协议的 X 专用连接
	Display* display = XOpenDisplay(0);

	// 连接打开检查
	if (display == 0)
	{
		fprintf(stderr, "unable to open display\n");
		return;
	}

	// 初始化 XRecordCreateContext 所需的 XRecordClientSpec 参数
	// XRecordAllClients 的意思是 "记录所有 X Client" 的事件
	XRecordClientSpec clients = XRecordAllClients;

	// 创建 XRecordRange 变量，XRecordRange 用于控制记录事件的范围
	XRecordRange* range = XRecordAllocRange();
	if (range == 0)
	{
		fprintf(stderr, "unable to allocate XRecordRange\n");
		return;
	}

	// 初始化记录事件范围，范围开头设置成 KeyPress, 范围结尾设置成 MotionNotify 后
	// 事件的类型就包括 KeyPress、KeyRelase、ButtonPress、ButtonRelease、MotionNotify五种事件
	memset(range, 0, sizeof(XRecordRange));
	range->device_events.first = KeyPress;
	range->device_events.last = MotionNotify;

	// 根据上面的记录客户端类型和记录事件范围来创建　“记录上下文”
	// 然后把 XRecordContext 传递给 XRecordEnableContext 函数来开启事件记录循环
	XRecordContext context = XRecordCreateContext(display, 0, &clients, 1, &range, 1);
	if (context == 0)
	{
		fprintf(stderr, "XRecordCreateContext failed\n");
		return;
	}
	XFree(range);

	// XSync 的作用就是把上面的 X 代码立即发给 X Server
	// 这样 X Server 接受到事件以后会立即发送给 XRecord 的 Client 连接
	XSync(display, True);

	// 建立一个专门读取 XRecord 协议数据的 X 链接
	Display* display_datalink = XOpenDisplay(0);
	if (display_datalink == 0)
	{
		fprintf(stderr, "unable to open second display\n");
		return;
	}

	// 调用 XRecordEnableContext 函数建立 XRecord 上下文
	// XRecordEnableContext 函数一旦调用就开始进入堵塞时的事件循环，直到线程或所属进程结束
	// X Server 事件一旦发生就传递给事件处理回调函数
	if (!XRecordEnableContext(display_datalink, context, callback, (XPointer) this)) {
		fprintf(stderr, "XRecordEnableContext() failed\n");
		return;
	}
    }
}

// handleRecordEvent 函数的wrapper，避免 XRecord 代码编译不过的问题
void KeyEventMonitor::callback(XPointer ptr, XRecordInterceptData* data)
{
	((KeyEventMonitor *) ptr)->handleRecordEvent(data);
}

// 真实处理 X 事件监听的回调函数
void KeyEventMonitor::handleRecordEvent(XRecordInterceptData* data)
{
    Display *display = XOpenDisplay(NULL);
	if (data->category == XRecordFromServer)
	{
		xEvent * event = (xEvent *)data->data;
        int keycode = event->u.u.detail;
		switch (event->u.u.type)
		{
		case ButtonPress:
            //printf("X11 ButtonPress\n");
			break;
		case ButtonRelease:
			//printf("X11 ButtonRelease\n");
			break;
		case KeyPress:
//            printf("X11 KeyPress %s \n",  XKeysymToString(XKeycodeToKeysym(display, keycode, 0)));
            switch(XKeycodeToKeysym(display, keycode, 0)) {
            case XK_Caps_Lock:
//                printf("cap lock");
                isCapsLock = !isCapsLock;
                Q_EMIT CapsLockChanged(isCapsLock);
                break;
            case XK_Super_L:
                hasModifer = true;
//                printf("SUPER pressed\n");
                break;
            case XK_p:
            case XK_F3:
            case XK_F7:
                if(hasModifer) {
//                    Q_EMIT displaySwitchSelect();
                }
                break;
            }

			break;
		case KeyRelease:
//            printf("X11 KeyRelease %d \n", ((unsigned char*) data->data)[1] );
            switch(XKeycodeToKeysym(display, keycode, 0)) {
            case XK_Super_L:
                hasModifer = false;
                Q_EMIT displaySwitchConfirm();
//                printf("SUPER pressed\n");
                break;
            case XK_p:
            case XK_F3:
            case XK_F7:
                if(hasModifer) {
                    Q_EMIT displaySwitchSelect();
//                    printf("SUPER+P pressed\n");
                }
//                printf("p pressed\n");
                break;
            }
			break;
		default:
			break;
		}
	}
    XCloseDisplay(display);

	fflush(stdout);
	XRecordFreeData(data);
}
