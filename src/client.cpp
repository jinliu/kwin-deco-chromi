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
#include <QCursor>
#include <QCoreApplication>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QWidget>
#include <QX11Info>
#include <X11/Xlib.h>

namespace Chromi
{


const int CORNER_SIZE = 20;
const int TITLE_BAR_WIDTH = 250;


Client::Client(KDecorationBridge* bridge, Factory* factory)
    : KDecorationUnstable(bridge, factory),
      m_isFullWidth(false),
      m_titleBar(NULL),
      m_previewWidget(NULL),
      m_activeButton(-1),
      m_hoverButton(-1)
{}


Client::~Client()
{
    delete m_titleBar;
}


void Client::init()
{
    // Fall back to traditional full-width titlebar for all except
    // top-level windows
    if (isModal() || windowType(NET::AllTypesMask) != NET::Normal)
        m_isFullWidth = true;

    createMainWidget();
    widget()->setAttribute(Qt::WA_NoSystemBackground);
    widget()->installEventFilter(this);

    m_titleBar = new QWidget();
    m_titleBar->setAttribute(Qt::WA_NoSystemBackground);
    if (isPreview()) {
        m_previewWidget = new QLabel("<center><b>Chromi preview</b></center>", widget());
        m_previewWidget->setAutoFillBackground(true);
        m_titleBar->setParent(m_previewWidget);
    } else if (m_isFullWidth)
        m_titleBar->setParent(widget());
    else {
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
        m_titleBar->show();
    }
    m_titleBar->installEventFilter(this);
    m_titleBar->setMouseTracking(true); // need this for the hover effect
}


Factory* Client::factory() const
{
    return static_cast<Factory*>(KDecorationUnstable::factory());
}


Client::Position Client::mousePosition(const QPoint& p) const
{
// KDecoration::mousePosition is buggy on top and left border, so have
// to implement our own version.
    if (isMaximized())
        return PositionCenter;

    const ThemeConfig& conf = factory()->themeConfig();
    int x=p.x()-conf.paddingLeft();
    int y=p.y()-conf.paddingTop();
    int w=widget()->width()-conf.paddingLeft()-conf.paddingRight();
    int h=widget()->height()-conf.paddingTop()-conf.paddingBottom();

    Position pos = PositionCenter;
    if (x<0 || x>=w || y<0 || y>=w) {
    } else if (x>=0 && x<conf.borderLeft()) { // left
        if (y>=0 && y<CORNER_SIZE)
            pos = PositionTopLeft;
        else if (y>=h-CORNER_SIZE && y<h)
            pos = PositionBottomLeft;
        else if (y>=CORNER_SIZE && y<h-CORNER_SIZE)
            pos = PositionLeft;
    } else if (x>=w-conf.borderRight() && x<w) { // right
        if (y>=0 && y<CORNER_SIZE)
            pos = PositionTopRight;
        else if (y>=h-CORNER_SIZE && y<h)
            pos = PositionBottomRight;
        else
            pos = PositionRight;
    } else if (y>=0 && y<conf.titleEdgeTop()) { // top
        if (x>=0 && x<CORNER_SIZE)
            pos = PositionTopLeft;
        else if (x>=w-CORNER_SIZE && x<w)
            pos = PositionTopRight;
        else
            pos = PositionTop;
    } else if (y>=h-conf.borderBottom() && y<h) { // bottom
        if (x>=0 && x<CORNER_SIZE)
            pos = PositionBottomLeft;
        else if (x>=w-CORNER_SIZE && x<w)
            pos = PositionBottomRight;
        else
            pos = PositionBottom;
    }

    return pos;
}


void Client::borders(int& left, int& right, int& top, int& bottom) const
{
    const ThemeConfig& conf = factory()->themeConfig();
    if (isMaximized()) {
        left = right = top = bottom = 0;
    } else {
        left = conf.borderLeft();
        right = conf.borderRight();
        bottom = conf.borderBottom();
        top = conf.titleEdgeTop();
    }
    if (isPreview() || m_isFullWidth)
        top += conf.titleHeight()+conf.titleEdgeBottom();
}


void Client::padding(int& left, int& right, int& top, int& bottom) const
{
    const ThemeConfig& conf = factory()->themeConfig();
    if (isMaximized()) {
        left = right = top = bottom = 0;
    } else {
        left = conf.paddingLeft();
        right = conf.paddingRight();
        bottom = conf.paddingBottom();
        top = conf.paddingTop();
    }
}


void Client::resize(const QSize& s)
{
    widget()->resize(s);
    updateWindowShape();
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
    updateTitleBar();
}


void Client::captionChange()
{
    updateTitleBar();
}

void Client::iconChange()
{}


void Client::maximizeChange()
{
    layoutTitleBar();
    updateTitleBar();
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
        {
            if (titleBarMouseEvent(static_cast<QMouseEvent*>(e)))
                return true;
            if (m_isFullWidth)
                return false;
            // Pass mouse event to widget() where moving/window menu
            // is handled. Because titlebar is not child of widget(),
            // we need to translate the mouse pos.
            QMouseEvent& oldEvent = *static_cast<QMouseEvent*>(e);
            QMouseEvent newEvent(oldEvent.type(),
                                 widget()->mapFromGlobal(oldEvent.globalPos()),
                                 oldEvent.globalPos(),
                                 oldEvent.button(),
                                 oldEvent.buttons(),
                                 oldEvent.modifiers());
            return qApp->sendEvent(widget(), &newEvent);
        }
        case QEvent::Wheel:
            titlebarMouseWheelOperation(static_cast<QWheelEvent*>(e)->delta());
            return true;
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
    const ThemeConfig& conf = factory()->themeConfig();

    Plasma::FrameSvg* frame = factory()->frame();
    frame->setElementPrefix("decoration");
    if (!isActive() && frame->hasElementPrefix("decoration-inactive"))
        frame->setElementPrefix("decoration-inactive");

    // restrict painting on the decoration - no need to paint behind the window
    painter.setClipRect(0, 0, conf.borderLeft() + conf.paddingLeft(), widget()->height(), Qt::ReplaceClip);
    painter.setClipRect(0, 0, widget()->width(), conf.titleEdgeTop() + conf.paddingTop(), Qt::UniteClip);
    painter.setClipRect(widget()->width() - conf.borderRight() - conf.paddingRight(), 0,
                        conf.borderRight() + conf.paddingRight(), widget()->height(),
                        Qt::UniteClip);
    painter.setClipRect(0, widget()->height() - conf.borderBottom() - conf.paddingBottom(),
                        widget()->width(), conf.borderBottom() + conf.paddingBottom(),
                        Qt::UniteClip);

    QRectF r(widget()->rect());
    frame->resizeFrame(r.size());
    frame->paintFrame(&painter, r);
}


void Client::frameResizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    const ThemeConfig& conf = factory()->themeConfig();

    QRect r(widget()->rect());
    if (!isMaximized())
        r.adjust(conf.borderLeft()+conf.paddingLeft(), conf.titleEdgeTop()+conf.paddingTop(),
                 -(conf.borderRight()+conf.paddingRight()), -(conf.borderBottom()+conf.paddingBottom()));

    if (m_previewWidget) {
        m_previewWidget->setGeometry(r);
        r.moveTo(0, 0);
    } else if (!isMaximized() && !m_isFullWidth) {
        r.translate(-conf.paddingLeft(), -conf.paddingTop());
    }

    if (!m_isFullWidth && r.width() > TITLE_BAR_WIDTH)
        r.setLeft(r.left()+r.width()-TITLE_BAR_WIDTH);
    int titleBarHeight = conf.titleHeight() + conf.titleEdgeBottom();
    if (r.height() > titleBarHeight)
        r.setHeight(titleBarHeight);
    m_titleBar->setGeometry(r);

    if (!m_isFullWidth) {
        // Shape the left edge of titlebar
        int w=r.width(), h=r.height();
        QPolygon p;
        p.putPoints(0, 4, 0,0, w,0, w,h, h/2,h);
        m_titleBar->setMask(p);
    }

    layoutTitleBar();
}


void Client::titleBarPaintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPixmap buffer(m_titleBar->size());
    QPainter painter(&buffer);
    const ThemeConfig& conf = factory()->themeConfig();

    // background
    Plasma::FrameSvg* frame = factory()->frame();
    if (!isActive() && frame->hasElementPrefix("decoration-inactive"))
        frame->setElementPrefix("decoration-inactive");
    else
        frame->setElementPrefix("decoration");

    QRectF r(widget()->rect());
    QRectF t(buffer.rect());
    // Resizing frame to window size makes window resizing very slow,
    // so let's resize to a fixed size as an performance workaround.
    // frame->resizeFrame(r.adjusted(-conf.paddingLeft(), -conf.paddingTop(),
    //                                conf.paddingRight(), conf.paddingBottom()).size());
    // frame->paintFrame(&painter, t,
    //                   t.translated(conf.paddingLeft()+r.width()-conf.borderRight()-m_titleBar->width(),
    //                                conf.paddingTop()+conf.titleEdgeTop()));
    frame->resizeFrame(t.adjusted(-(conf.borderLeft()+conf.paddingLeft()),
                                  -(conf.titleEdgeTop()+conf.paddingTop()),
                                  conf.borderRight()+conf.paddingRight(),
                                  conf.borderBottom()+conf.paddingBottom()).size());
    frame->paintFrame(&painter, t,
                      t.translated(conf.paddingLeft()+conf.borderLeft(),
                                   conf.paddingTop()+conf.titleEdgeTop()));

    // buttons
    for (int i=0; i<3; ++i) {
        Plasma::FrameSvg* frame = factory()->button(m_button[i].name);

        QString prefix = "active";
        if (!isActive() && frame->hasElementPrefix("inactive"))
            prefix = "inactive";
        if (m_hoverButton == i) {
            if (frame->hasElementPrefix("hover"))
                prefix = "hover";
            if (!isActive() && frame->hasElementPrefix("hover-inactive"))
                prefix = "hover-inactive";
        }
        if (m_activeButton == i) {
            if (frame->hasElementPrefix("pressed"))
                prefix = "pressed";
            if (!isActive() && frame->hasElementPrefix("pressed-inactive"))
                prefix = "pressed-inactive";
        }
        if (!m_button[i].enabled) {
            if (frame->hasElementPrefix("deactivated"))
                prefix = "deactivated";
            if (!isActive() && frame->hasElementPrefix("deactivated-inactive"))
                prefix = "deactivated-inactive";
        }
        frame->setElementPrefix(prefix);

        frame->resizeFrame(m_button[i].paintRect.size());
        frame->paintFrame(&painter, m_button[i].paintRect.topLeft());
    }

    // caption
    if (m_isFullWidth)
        r.setLeft(conf.titleBorderLeft());
    r.setRight(m_button[0].paintRect.left()-conf.titleBorderRight());
    r.setHeight(conf.titleHeight()+conf.titleEdgeBottom());
    painter.setFont(options()->font(isActive()));
    int textOpt = conf.verticalAlignment()|Qt::TextSingleLine;
    if (m_isFullWidth)
        textOpt |= conf.alignment();
    else
        textOpt |= Qt::AlignRight;
    if (conf.useTextShadow()) {
        // shadow code is inspired by Qt FAQ: How can I draw shadows behind text?
        // see http://www.qtsoftware.com/developer/faqs/faq.2007-07-27.3052836051
        painter.save();
        if (isActive())
            painter.setPen(conf.activeTextShadowColor());
        else
            painter.setPen(conf.inactiveTextShadowColor());
        int dx = conf.textShadowOffsetX();
        int dy = conf.textShadowOffsetY();
        painter.setOpacity(0.5);
        painter.drawText(r.translated(dx, dy), textOpt, caption());
        painter.setOpacity(0.2);
        painter.drawText(r.translated(dx+1, dy), textOpt, caption());
        painter.drawText(r.translated(dx-1, dy), textOpt, caption());
        painter.drawText(r.translated(dx, dy+1), textOpt, caption());
        painter.drawText(r.translated(dx, dy-1), textOpt, caption());
        painter.restore();
    }
    if (isActive())
        painter.setPen(conf.activeTextColor());
    else
        painter.setPen(conf.inactiveTextColor());
    painter.drawText(r, textOpt, caption());

    // blt to the real surface
    QPainter(m_titleBar).drawPixmap(0, 0, buffer);
}


bool Client::titleBarMouseEvent(QMouseEvent* event)
{
    QEvent::Type type = event->type();
    QPoint pos = event->pos();
    Qt::MouseButton button = event->button();

    // on buttons?
    for (int i=0; i<3; ++i) {
        if (m_button[i].mouseRect.contains(pos)) { // hit button#i
            switch (type) {
            case QEvent::MouseButtonPress:
                if (m_button[i].enabled) {
                    m_activeButton = i;
                    updateTitleBar();
                }
                return true;
            case QEvent::MouseButtonRelease:
                if (m_button[i].enabled && m_activeButton == i)
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
                updateTitleBar();
                return true;
            case QEvent::MouseMove:
                if (m_hoverButton != i) {
                    m_hoverButton = i;
                    updateTitleBar();
                }
                return false;
            default:
                return false;
            }
        }
    }

    // doesn't hit any button

    // double click on title
    if (type==QEvent::MouseButtonDblClick && button==Qt::LeftButton) {
        titlebarDblClickOperation();
        return true;
    }

    // clear pressed/hover image
    if (m_activeButton!=-1 || m_hoverButton!=-1) {
        m_activeButton = m_hoverButton = -1;
        updateTitleBar();
    }

    return false;
}


bool Client::isMaximized() const
{
    return maximizeMode()==MaximizeFull && !options()->moveResizeMaximizedWindows();
}


void Client::layoutTitleBar()
{
    m_button[0].name = "minimize";
    m_button[0].enabled = isMinimizable();
    m_button[1].name = isMaximized()&&factory()->hasButton("restore")?"restore":"maximize";
    m_button[1].enabled = isMaximizable();
    m_button[2].name = "close";
    m_button[2].enabled = isCloseable();

    const ThemeConfig& conf = factory()->themeConfig();
    QRect r(m_titleBar->width()-(conf.buttonWidth()+conf.buttonSpacing()),
            conf.buttonMarginTop(), conf.buttonWidth(), conf.buttonHeight());
    for (int i=2; i>=0; --i) {
        m_button[i].paintRect = m_button[i].mouseRect = r;
        if (isMaximized())
            m_button[i].mouseRect.setTop(0);
        r.translate(-(conf.buttonWidth()+conf.buttonSpacing()), 0);
    }
    if (isMaximized())
        m_button[2].mouseRect.setRight(m_titleBar->width());
}


void Client::updateTitleBar()
{
    if (m_isFullWidth)
        // If titlebar is parented to widget(), calling
        // m_titleBar->update() seems to have no effect at all. So we
        // have to update the whole decoration. This is quite
        // expensive. :-(
        widget()->update();
    else
        m_titleBar->update();
}


void Client::updateWindowShape()
{
    int w=widget()->width();
    int h=widget()->height();

    if (isMaximized() || compositingActive()) {
        QRegion mask(0,0,w,h);
        setMask(mask);
        return;
    }

    const ThemeConfig& conf = factory()->themeConfig();
    Plasma::FrameSvg* deco = factory()->frame();
    if (!deco->hasElementPrefix("decoration-opaque")) {
        // opaque element is missing: set generic mask
        w = w - conf.paddingLeft() - conf.paddingRight();
        h = h - conf.paddingTop() - conf.paddingBottom();
        QRegion mask(conf.paddingLeft(),conf.paddingTop(),w,h);
        setMask(mask);
        return;
    }
    deco->setElementPrefix("decoration-opaque");
    deco->resizeFrame(QSize(w-conf.paddingLeft()-conf.paddingRight(),
                            h-conf.paddingTop()-conf.paddingBottom()));
    QRegion mask = deco->mask().translated(conf.paddingLeft(), conf.paddingTop());
    setMask(mask);
}


} // namespace Chromi

#include "client.moc"
