#include "button.h"
#include "chromiclient.h"

#include <QPainter>

namespace Chromi
{

const int PREFERRED_WIDTH = 24;
const int PREFERRED_HEIGHT = 18;

Button::Button(char type, ChromiClient* client, QWidget* parent)
    : QAbstractButton(parent),
      m_type(type),
      m_client(client),
      m_lastMouseButton(Qt::NoButton),
      m_realizeButtons(Qt::LeftButton)
{
    switch (type) {
    case MINIMIZE:
        connect(this, SIGNAL(clicked()), client, SLOT(minimize()));
        break;
    case MAXIMIZE:
        m_realizeButtons |= Qt::MidButton|Qt::RightButton;
        connect(this, SIGNAL(clicked()), this, SLOT(slotMaximize()));
        break;
    case CLOSE:
        connect(this, SIGNAL(clicked()), client, SLOT(closeWindow()));
        break;
    }
}

QSize Button::sizeHint() const
{
    return QSize(PREFERRED_WIDTH, PREFERRED_HEIGHT);
}

void Button::slotMaximize()
{
    m_client->maximize(m_lastMouseButton);
}

void Button::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QRect r = rect();
    painter.setPen(Qt::black);
    painter.drawRect(r);
}

void Button::mousePressEvent(QMouseEvent* e)
{
    m_lastMouseButton = e->button();
    // pass on event after changing button to LeftButton
    QMouseEvent me(e->type(), e->pos(), (e->button()&m_realizeButtons) ? Qt::LeftButton : Qt::NoButton, e->buttons(), e->modifiers());
    QAbstractButton::mousePressEvent(&me);
}

void Button::mouseReleaseEvent(QMouseEvent* e)
{
    m_lastMouseButton = e->button();
    // pass on event after changing button to LeftButton
    QMouseEvent me(e->type(), e->pos(), (e->button()&m_realizeButtons) ? Qt::LeftButton : Qt::NoButton, e->buttons(), e->modifiers());
    QAbstractButton::mouseReleaseEvent(&me);
}

}

#include "button.moc"
