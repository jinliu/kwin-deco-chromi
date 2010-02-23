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

#include "button.hpp"
#include "client.hpp"
#include "factory.hpp"

#include <QPainter>

#include <stdexcept>

namespace Chromi
{


Button::Button(char type, Client* client, QWidget* parent)
    : QAbstractButton(parent),
      m_type(type),
      m_client(client),
      m_realizeButtons(Qt::LeftButton)
{
    setAttribute(Qt::WA_NoSystemBackground);

    const ThemeConfig& conf = m_client->factory()->themeConfig();
    int width;
    switch (m_type) {
    case MinButton:
        m_buttonName = "minimize";
        width = conf.buttonWidthMinimize();
        setEnabled(m_client->isMinimizable());
        connect(this, SIGNAL(clicked()), m_client, SLOT(minimize()));
        break;
    case MaxButton:
        m_buttonName = "maximize";
        width = conf.buttonWidthMaximizeRestore();
        setEnabled(m_client->isMaximizable());
        m_realizeButtons |= Qt::MidButton | Qt::RightButton;
        connect(this, SIGNAL(clicked()), this, SLOT(maximize()));
        break;
    case CloseButton:
        m_buttonName = "close";
        width = conf.buttonWidthClose();
        setEnabled(m_client->isCloseable());
        connect(this, SIGNAL(clicked()), m_client, SLOT(closeWindow()));
        break;
    case OnAllDesktopsButton:
        m_buttonName = "alldesktops";
        width = conf.buttonWidthAllDesktops();
        setCheckable(true);
        setChecked(m_client->isOnAllDesktops());
        connect(this, SIGNAL(clicked()), m_client, SLOT(toggleOnAllDesktops()));
        break;
    case AboveButton:
        m_buttonName = "keepabove";
        width = conf.buttonWidthKeepAbove();
        setCheckable(true);
        setChecked(m_client->keepAbove());        
        connect(this, SIGNAL(clicked()), m_client, SLOT(toggleKeepAbove()));
        break;
    case BelowButton:
        m_buttonName = "keepbelow";
        width = conf.buttonWidthKeepBelow();
        setCheckable(true);
        setChecked(m_client->keepBelow());        
        connect(this, SIGNAL(clicked()), m_client, SLOT(toggleKeepBelow()));
        break;
    case HelpButton:
        m_buttonName = "help";
        width = conf.buttonWidthHelp();
        connect(this, SIGNAL(clicked()), m_client, SLOT(showContextHelp()));
        break;
    default:
        return;
    }
    setFixedSize(width, conf.buttonHeight());
}


bool Button::isValid() const
{
    return m_client->factory()->hasButton(m_buttonName);
}


void Button::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QString buttonName = m_buttonName;
    if (m_type==MaxButton && m_client->isMaximized() && m_client->factory()->hasButton("restore"))
        buttonName = "restore";
    Plasma::FrameSvg* frame = m_client->factory()->button(buttonName);

    QString prefix = "active";
    bool isActive = m_client->isActive();
    if (!isActive && frame->hasElementPrefix("inactive"))
        prefix = "inactive";
    if (isChecked() || underMouse()) {
        if (frame->hasElementPrefix("hover"))
            prefix = "hover";
        if (!isActive && frame->hasElementPrefix("hover-inactive"))
                prefix = "hover-inactive";
    }
    if (isDown()) {
        if (frame->hasElementPrefix("pressed"))
            prefix = "pressed";
        if (!isActive && frame->hasElementPrefix("pressed-inactive"))
            prefix = "pressed-inactive";
    }
    if (!isEnabled()) {
        if (frame->hasElementPrefix("deactivated"))
                prefix = "deactivated";
        if (!isActive && frame->hasElementPrefix("deactivated-inactive"))
            prefix = "deactivated-inactive";
    }
    frame->setElementPrefix(prefix);
    
    frame->resizeFrame(size());
    frame->paintFrame(&painter);
}


void Button::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    update();
}


void Button::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    update();
}


void Button::mousePressEvent(QMouseEvent* e)
{
    if (!(e->button() & m_realizeButtons))
        return;
    
    m_lastMouse = e->button();
    // pass on event after changing button to LeftButton
    QMouseEvent me(e->type(), e->pos(), Qt::LeftButton, e->buttons(), e->modifiers());
    QAbstractButton::mousePressEvent(&me);
}


void Button::mouseReleaseEvent(QMouseEvent* e)
{
    if (!(e->button()&m_realizeButtons))
        return;
    
    m_lastMouse = e->button();
    // pass on event after changing button to LeftButton
    QMouseEvent me(e->type(), e->pos(), Qt::LeftButton, e->buttons(), e->modifiers());
    QAbstractButton::mouseReleaseEvent(&me);
}


void Button::maximize()
{
    m_client->maximize(m_lastMouse);
}


} // namespace Chromi

#include "button.moc"
