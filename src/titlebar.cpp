/********************************************************************
Copyright (C) 2010 Jin Liu <m.liu.jin@gmail.com>

The embed() function is modified from the nitrogen-minimal project:
Copyright (c) 2009, 2010 Hugo Pereira <hugo.pereira@free.fr>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#include "titlebar.h"
#include "button.h"
#include "chromiclient.h"

#include <QHBoxLayout>
#include <QLayoutItem>
#include <QPainter>
#include <QX11Info>

#include <X11/Xlib.h>

#include <assert.h>

namespace Chromi
{

TitleBar::TitleBar(ChromiClient* client)
    : QWidget(client->widget()),
      m_client(client)
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addStretch();
    layout->addWidget(new Button(Button::MINIMIZE, client, this));
    layout->addWidget(new Button(Button::MAXIMIZE, client, this));
    layout->addWidget(new Button(Button::CLOSE, client, this));
    setLayout(layout);
    
    embed();
}

void TitleBar::embed(void)
{
    WId window_id = m_client->windowId();
    assert(window_id);

    WId current = window_id;
    while (true)
    {
        WId root, parent = 0;
        WId* children = NULL;
        uint child_count = 0;
        XQueryTree(QX11Info::display(), current, &root, &parent, &children, &child_count);
        if (parent && parent != root && parent != current)
            current = parent;
        else
            break;
    }
    XReparentWindow(QX11Info::display(), winId(), current, 0, 0);
}

void TitleBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QColor bgColor = m_client->options()->color(ChromiClient::ColorTitleBar, m_client->isActive());
    painter.fillRect(rect(), bgColor);

    // The first widget is a spacer. Draw the window title over it.
    QLayoutItem* spacer = layout()->itemAt(0);
    QRect r = spacer->geometry();
    painter.drawText(r, Qt::AlignLeft, m_client->caption());
}

}
