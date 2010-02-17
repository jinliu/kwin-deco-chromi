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

#ifndef CHROMI_CLIENT_H
#define CHROMI_CLIENT_H

#include <kdecoration.h>

class QWidget;

namespace Chromi
{

class Factory;

class Client : public KDecorationUnstable
{
    Q_OBJECT
public:
    Client(KDecorationBridge* bridge, Factory* factory);
    ~Client();

    /*override*/ void init();
    /*override*/ Position mousePosition(const QPoint& p) const;
    /*override*/ void borders(int& left, int& right, int& top, int& bottom) const;
    /*override*/ void resize(const QSize& s);
    /*override*/ QSize minimumSize() const;
    /*override*/ void activeChange();
    /*override*/ void captionChange();
    /*override*/ void iconChange();
    /*override*/ void maximizeChange();
    /*override*/ void desktopChange();
    /*override*/ void shadeChange();
    
    /*override*/ void padding(int& left, int& right, int& top, int& bottom) const;
    /*override*/ bool eventFilter(QObject* o, QEvent* e);
    /*override*/ Factory* factory() const;
protected:
    void framePaintEvent();
    void frameResizeEvent();
    void titlebarPaintEvent();
    void titlebarResizeEvent();
    bool titlebarMouseEvent(QMouseEvent* event);

    bool isMaximized() const;
    void initTitlebar();
    void updateTitlebar();
    void updateWindowShape();
private:
    bool m_isFullWidth;
    QWidget* m_titlebar;
    QWidget* m_previewWidget;
    int m_activeButton;
    int m_hoverButton;

    struct ButtonInfo
    {
        const char* name;
        bool enabled;
        // Where to paint the button image
        QRect paintRect;
        // Where mouse click is effective. Can be larger than
        // paintRect, e.g. when maximized.
        QRect mouseRect;
    };
    ButtonInfo m_button[3];
};

}

#endif
