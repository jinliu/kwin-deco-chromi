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

#include "factory.hpp"
#include "client.hpp"

#include <KDebug>
#include <KGlobal>
#include <KStandardDirs>

extern "C"
{
KDE_EXPORT KDecorationFactory* create_factory()
{
    return new Chromi::Factory();
}
}

namespace Chromi
{

Factory::Factory()
{
    QString path = KGlobal::dirs()->findResource("data", "chromi/decoration.svgz");
    if (path.isEmpty()) {
        kDebug(1216) << "Could not find decoration svg: aborting";
        abort();
    }
    m_frame.setImagePath(path);
    m_frame.setCacheAllRenderedFrames(true);
    m_frame.setEnabledBorders(Plasma::FrameSvg::AllBorders);

    initButtonFrame("minimize");
    initButtonFrame("maximize");
    initButtonFrame("close");    
}

void Factory::initButtonFrame(const QString& button)
{
    QString path = KGlobal::dirs()->findResource("data", "chromi/"+button+".svgz");
    if (path.isEmpty()) {
        kDebug(1216) << "Could not find "<<button<<" svg: aborting";
        abort();
    }
    Plasma::FrameSvg *frame = new Plasma::FrameSvg(this);
    frame->setImagePath(path);
    frame->setCacheAllRenderedFrames(true);
    frame->setEnabledBorders(Plasma::FrameSvg::NoBorder);
    m_buttons[button] = frame;
}

KDecoration* Factory::createDecoration(KDecorationBridge* bridge)
{
    return new Client(bridge, this);
}

bool Factory::supports(Ability ability) const
{
    switch( ability ) {
        // announce
    case AbilityAnnounceButtons:
    case AbilityAnnounceColors:
        // buttons
    case AbilityButtonMinimize:
    case AbilityButtonMaximize:
    case AbilityButtonClose:
        return true;
    default:
        return false;
    }
}

Plasma::FrameSvg* Factory::frame()
{
    return &m_frame;
}

Plasma::FrameSvg* Factory::button(const QString& b)
{
    return m_buttons[b];
}

} // namespace Chromi

#include "factory.moc"
