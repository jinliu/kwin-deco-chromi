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

#ifndef CHROMI_FACTORY_H
#define CHROMI_FACTORY_H

#include <kdecorationfactory.h>
#include <KDE/Plasma/FrameSvg>
#include <QHash>

namespace Chromi
{

class Factory : public QObject, public KDecorationFactory
{
    Q_OBJECT
public:
    Factory();
    /*override*/ KDecoration* createDecoration(KDecorationBridge* bridge);
    /*override*/ bool supports(Ability ability) const;

    void initButtonFrame(const QString& button);
    Plasma::FrameSvg* frame();
    Plasma::FrameSvg* button(const QString& b);
private:
    Plasma::FrameSvg m_frame;
    QHash<QString, Plasma::FrameSvg*> m_buttons;
};

}

#endif
