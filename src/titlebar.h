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

#ifndef CHROMI_TITLEBAR_H
#define CHROMI_TITLEBAR_H

#include <QWidget>

namespace Chromi
{

class ChromiClient;

class TitleBar : public QWidget
{
    //TODO: If the following line is enabled, this is not painted at
    //all. Don't know why.
    //Q_OBJECT
public:
    TitleBar(ChromiClient* client);
protected:
    /*override*/ void paintEvent(QPaintEvent* event);
private:
    // Embed this title bar into the application window.
    void embed();
    
    ChromiClient* m_client;
};

}

#endif
