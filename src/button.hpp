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

class Client;

class Button : public QAbstractButton
{
    Q_OBJECT
public:
    enum {
        MenuButton = 'M',
        OnAllDesktopsButton = 'S',
        HelpButton = 'H',
        MinButton = 'I',
        MaxButton = 'A',
        CloseButton = 'X',
        AboveButton = 'F',
        BelowButton = 'B',
        ShadeButton = 'L',
        ResizeButton = 'R',
        ExplicitSpacer = '_'
    };
    
    Button(char type, Client* client, QWidget* parent);
    bool isValid() const;
protected:
    /*override*/ void paintEvent(QPaintEvent* event);
    /*override*/ void enterEvent(QEvent *event);
    /*override*/ void leaveEvent(QEvent *event);
    /*override*/ void mousePressEvent(QMouseEvent* e);
    /*override*/ void mouseReleaseEvent(QMouseEvent* e);
protected slots:
    void maximize();
private:
    char m_type;
    Client* m_client;
    QString m_buttonName;
    Qt::MouseButtons m_lastMouse;
    int m_realizeButtons;
};

}

#endif
