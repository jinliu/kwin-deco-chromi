/********************************************************************
Copyright (C) 2010 Jin Liu <m.liu.jin@gmail.com>

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

#include "chromiclient.h"
#include "titlebar.h"

#include <QPainter>
#include <QWidget>

namespace Chromi
{

const int BORDER_SIZE = 4;
const int CORNER_SIZE = 20;
const int TITLE_BAR_WIDTH = 200;
const int TITLE_BAR_HEIGHT = 27;

ChromiClient::ChromiClient(KDecorationBridge* bridge, KDecorationFactory* factory)
    : KDecoration(bridge, factory),
      m_titleBar(NULL)
{}

void ChromiClient::init()
{
    createMainWidget();
    widget()->setAttribute(Qt::WA_NoSystemBackground);
    widget()->installEventFilter(this);

    m_titleBar = new TitleBar(this);
}

ChromiClient::Position ChromiClient::mousePosition(const QPoint& p) const
{
// KDecoration::mousePosition is buggy on top and left border, so have
// to implement our own version.
    int x=p.x(), y=p.y();
    int width=widget()->size().width(), height=widget()->size().height();
    int left, right, top, bottom;
    borders(left, right, top, bottom);

    Position pos = PositionCenter;
    if (x>=0 && x<left) { // left
        if (y>=0 && y<CORNER_SIZE)
            pos = PositionTopLeft;
        else if (y>=height-CORNER_SIZE && y<height)
            pos = PositionBottomLeft;
        else
            pos = PositionLeft;
    } else if (x>=width-right && x<width) { // right
        if (y>=0 && y<CORNER_SIZE)
            pos = PositionTopRight;
        else if (y>=height-CORNER_SIZE && y<height)
            pos = PositionBottomRight;
        else
            pos = PositionRight;
    } else if (y>=0 && y<top) { // top
        if (x>=0 && x<CORNER_SIZE)
            pos = PositionTopLeft;
        else if (x>=width-CORNER_SIZE && x<width)
            pos = PositionTopRight;
        else
            pos = PositionTop;
    } else if (y>=height-bottom && y<height) { // bottom
        if (x>=0 && x<CORNER_SIZE)
            pos = PositionBottomLeft;
        else if (x>=width-CORNER_SIZE && x<width)
            pos = PositionBottomRight;
        else
            pos = PositionBottom;
    }
    
    return pos;
}

void ChromiClient::borders(int& left, int& right, int& top, int& bottom) const
{
    left = right = top = bottom = (isMaximized() ? 0 : BORDER_SIZE);
}

void ChromiClient::resize(const QSize& s)
{
    widget()->resize(s);
}

QSize ChromiClient::minimumSize() const
{
    int left, right, top, bottom;
    borders(left, right, top, bottom);
    return QSize(left+right, top+bottom);
}

void ChromiClient::activeChange()
{
    m_titleBar->update();
}

void ChromiClient::captionChange()
{
    m_titleBar->update();
}

void ChromiClient::iconChange()
{}

void ChromiClient::maximizeChange()
{}

void ChromiClient::desktopChange()
{}

void ChromiClient::shadeChange()
{}

bool ChromiClient::eventFilter(QObject* o, QEvent* e)
{
    if (o != widget())
        return false;
    switch(e->type())
    {
    case QEvent::Paint:
	paintEvent(static_cast<QPaintEvent*>(e));
	return true;
    case QEvent::Resize:
	resizeEvent(static_cast<QResizeEvent*>(e));
	return true;
    case QEvent::MouseButtonPress:
        processMousePressEvent(static_cast<QMouseEvent*>(e));
        return true;
    default:
        return false;
    }
}

void ChromiClient::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(widget());
    painter.setClipRegion(event->region());
}

void ChromiClient::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    
    QRect r = widget()->rect();
    int left, right, top, bottom;
    borders(left, right, top, bottom);
    r.adjust(left, top, -right, -bottom);
    if (r.width() > TITLE_BAR_WIDTH)
        r.setLeft(r.left()+r.width()-TITLE_BAR_WIDTH);
    if (r.height() > TITLE_BAR_HEIGHT)
        r.setBottom(r.top()+TITLE_BAR_HEIGHT);

    m_titleBar->setGeometry(r);
}

bool ChromiClient::isMaximized() const
{
    return maximizeMode()==MaximizeFull && !options()->moveResizeMaximizedWindows();
}

} // namespace Chromi

#include "chromiclient.moc"
