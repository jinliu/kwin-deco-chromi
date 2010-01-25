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

#include "client.hpp"
#include "factory.hpp"

#include <KDebug>
#include <QLabel>
#include <QPainter>
#include <QWidget>
#include <QX11Info>
#include <X11/Xlib.h>

namespace Chromi
{

const int BORDER_SIZE = 4;
const int CORNER_SIZE = 20;
const int TITLE_BAR_WIDTH = 250;
const int TITLE_BAR_HEIGHT = 20;
const int BUTTON_WIDTH = 24;
const int BUTTON_HEIGHT = 18;
const int BUTTON_SPACING = 2;
const int PADDING_LEFT = 9;
const int PADDING_TOP = 8;
const int PADDING_RIGHT = 9;
const int PADDING_BOTTOM = 9;
const int TITLE_SPACING_LEFT = 12;
const int TITLE_SPACING_RIGHT = 8;

const char* buttonNames[] = {"minimize", "maximize", "close"};

Client::Client(KDecorationBridge* bridge, Factory* factory)
    : KDecoration(bridge, factory),
      m_factory(factory),
      m_titleBar(NULL),
      m_previewWidget(NULL),
      m_activeButton(-1),
      m_hoverButton(-1)
{}

void Client::init()
{
    createMainWidget();
    widget()->setAttribute(Qt::WA_NoSystemBackground);
    widget()->installEventFilter(this);

    m_titleBar = new QWidget(widget());
    m_titleBar->setAttribute(Qt::WA_NoSystemBackground);
    m_titleBar->installEventFilter(this);

    if (isPreview()) {
        m_previewWidget = new QLabel("<center><b>Chromi preview</b></center>", widget());
        m_previewWidget->setAutoFillBackground(true);
        m_titleBar->setParent(m_previewWidget);
    } else {    
        // Reparent the title bar to the application window, so we can
        // draw over it.
        WId current = windowId();
        for (;;)
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
        XReparentWindow(QX11Info::display(), m_titleBar->winId(), current, 0, 0);
    }
}

Client::Position Client::mousePosition(const QPoint& p) const
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

void Client::borders(int& left, int& right, int& top, int& bottom) const
{
    left = right = top = bottom = (isMaximized() ? 0 : BORDER_SIZE);
}

void Client::resize(const QSize& s)
{
    widget()->resize(s);
}

QSize Client::minimumSize() const
{
    int left, right, top, bottom;
    borders(left, right, top, bottom);
    return QSize(left+right, top+bottom);
}

void Client::activeChange()
{
    widget()->update();
    m_titleBar->update();
}

void Client::captionChange()
{
    m_titleBar->update();
}

void Client::iconChange()
{}

void Client::maximizeChange()
{
    m_titleBar->update();
}

void Client::desktopChange()
{}

void Client::shadeChange()
{}

bool Client::eventFilter(QObject* o, QEvent* e)
{
    if (o == widget())
        switch(e->type())
        {
        case QEvent::Paint:
            framePaintEvent(static_cast<QPaintEvent*>(e));
            return true;
        case QEvent::Resize:
            frameResizeEvent(static_cast<QResizeEvent*>(e));
            return true;
        case QEvent::MouseButtonPress:
            processMousePressEvent(static_cast<QMouseEvent*>(e));
            return true;
        default:
            return false;
        }
    else if (o == m_titleBar)
        switch(e->type())
        {
        case QEvent::Paint:
            titleBarPaintEvent(static_cast<QPaintEvent*>(e));
            return true;
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
            return titleBarMouseEvent(static_cast<QMouseEvent*>(e));
        default:
            return false;
        }
    else
        return false;
}

void Client::framePaintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    // No border when maximized
    if (isMaximized())
        return;
    
    QPainter painter(widget());

    Plasma::FrameSvg* frame = m_factory->frame();
    if (isActive())
        frame->setElementPrefix("decoration");
    else
        frame->setElementPrefix("decoration-inactive");

    // restrict painting on the decoration - no need to paint behind the window
    int left, right, top, bottom;
    borders(left, right, top, bottom);
    painter.setClipping(true);
    painter.setClipRect(0, 0, left, height(), Qt::ReplaceClip);
    painter.setClipRect(0, 0, width(), top, Qt::UniteClip);
    painter.setClipRect(width()-right, 0, right, height(), Qt::UniteClip);
    painter.setClipRect(0, height()-bottom, width(), bottom, Qt::UniteClip);
    
    QRectF r(widget()->rect());
    frame->resizeFrame(r.adjusted(-9, -8, 9, 9).size());
    frame->paintFrame(&painter, r, r.translated(9, 8));
}

void Client::frameResizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    
    QRect r = widget()->rect();
    int left, right, top, bottom;
    borders(left, right, top, bottom);
    r.adjust(left, top, -right, -bottom);
    if (m_previewWidget)
        m_previewWidget->setGeometry(r);
    
    if (r.width() > TITLE_BAR_WIDTH)
        r.setLeft(r.left()+r.width()-TITLE_BAR_WIDTH);
    if (r.height() > TITLE_BAR_HEIGHT)
        r.setBottom(r.top()+TITLE_BAR_HEIGHT);
    m_titleBar->setGeometry(r);

    int w=r.width(), h=r.height();
    QPolygon p;
    p.putPoints(0, 4, 0,0, w,0, w,h, h/2,h);
    m_titleBar->setMask(p);
}

void Client::titleBarPaintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(m_titleBar);

    // background
    Plasma::FrameSvg* frame = m_factory->frame();
    if (isActive())
        frame->setElementPrefix("decoration");
    else
        frame->setElementPrefix("decoration-inactive");
    
    QRectF r(m_titleBar->rect());
    frame->resizeFrame(r.adjusted(-(PADDING_LEFT+BORDER_SIZE),
                                  -(PADDING_TOP+BORDER_SIZE),
                                  PADDING_RIGHT+BORDER_SIZE,
                                  PADDING_BOTTOM+BORDER_SIZE)
                                  .size());
    frame->paintFrame(&painter, r, r.translated(PADDING_LEFT+BORDER_SIZE, PADDING_TOP+BORDER_SIZE));

    // buttons
    int x = m_titleBar->width();
    for (int i=2; i>=0; --i) { // layout right->left
        x -= BUTTON_WIDTH + BUTTON_SPACING;
        Plasma::FrameSvg* frame = m_factory->button(buttonNames[i]);
        frame->setElementPrefix("active");
        frame->resizeFrame(QSizeF(BUTTON_WIDTH, BUTTON_HEIGHT));
        frame->paintFrame(&painter, QPointF(x, 0));
    }

    // caption
    r.setLeft(TITLE_SPACING_LEFT);
    r.setRight(x-TITLE_SPACING_RIGHT);
    r.setHeight(BUTTON_HEIGHT);
    painter.setFont(options()->font(isActive()));
    if (isActive())
        painter.setPen(QColor(255, 255, 255, 255));
    else
        painter.setPen(QColor(255, 255, 255, 200));
    painter.drawText(r, Qt::AlignRight|Qt::TextSingleLine, caption());
}

bool Client::titleBarMouseEvent(QMouseEvent* event)
{
    QPoint pos = event->pos();
    Qt::MouseButton button = event->button();
    
    int x = m_titleBar->width();
    for (int i=2; i>=0; --i) {
        x -= BUTTON_WIDTH + BUTTON_SPACING;
        int w = BUTTON_WIDTH;
        if (i==2 && isMaximized())
            w += BUTTON_SPACING;
        QRect r(x, 0, w, BUTTON_HEIGHT);
        if (r.contains(pos)) { // hit button#i
            switch (event->type()) {
            case QEvent::MouseButtonPress:
                m_activeButton = i;
                return true;
            case QEvent::MouseButtonRelease:
                if (m_activeButton == i)
                    switch (i) {
                    case 0: // minimize
                        if (button == Qt::LeftButton)
                            minimize();
                        break;
                    case 1: // maximize
                        maximize(button);
                        break;
                    case 2: // close
                        if (button == Qt::LeftButton)
                            closeWindow();
                        break;
                    }
                m_activeButton = -1;
                break;
            case QEvent::MouseMove:
                m_hoverButton = i;
                return false;
            default:
                return false;
            }
        }
    }
    
    // doesn't hit any button
    m_activeButton = m_hoverButton = -1;
    if (event->type()==QEvent::MouseButtonDblClick
        && event->button()==Qt::LeftButton)
        maximize(Qt::LeftButton);
    return false;
}

bool Client::isMaximized() const
{
    return maximizeMode()==MaximizeFull && !options()->moveResizeMaximizedWindows();
}

} // namespace Chromi

#include "client.moc"
