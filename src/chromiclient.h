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

#ifndef CHROMICLIENT_H
#define CHROMICLIENT_H

#include <kdecoration.h>

namespace Chromi
{

class TitleBar;

class ChromiClient : public KDecoration
{
    Q_OBJECT
public:
    ChromiClient(KDecorationBridge* bridge, KDecorationFactory* factory);
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
    
    /*override*/ bool eventFilter(QObject* o, QEvent* e);
protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    bool isMaximized() const;
private:
    TitleBar* m_titleBar;
};

}

#endif
