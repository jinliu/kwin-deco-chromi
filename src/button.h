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

#ifndef CHROMI_BUTTON_H
#define CHROMI_BUTTON_H

#include <QAbstractButton>

namespace Chromi
{

class ChromiClient;

class Button : public QAbstractButton
{
    Q_OBJECT
public:
    enum {
        MINIMIZE = 'I',
        MAXIMIZE = 'A',
        CLOSE = 'X'
    };
    
    Button(char type, ChromiClient* client, QWidget* parent);

    /*override*/ QSize sizeHint() const;
public slots:
    void slotMaximize();
protected:
    /*override*/ void paintEvent(QPaintEvent* event);
    /*override*/ void mousePressEvent(QMouseEvent *e);
    /*override*/ void mouseReleaseEvent(QMouseEvent *e);
private:
    char m_type;
    ChromiClient* m_client;
    Qt::MouseButtons m_lastMouseButton;
    int m_realizeButtons;
};

}

#endif
