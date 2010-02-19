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

#include <KConfig>
#include <KConfigGroup>
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
    init();
}


void Factory::init()
{
    KConfig conf("chromirc");
    KConfigGroup group(&conf, "Engine");

    m_themeName = group.readEntry("ThemeName", "chrome");

    QString file("aurorae/themes/" + m_themeName + "/decoration.svg");
    QString path = KGlobal::dirs()->findResource("data", file);
    if (path.isEmpty()) {
        file += 'z';
        path = KGlobal::dirs()->findResource("data", file);
    }
    if (path.isEmpty()) {
        kDebug(1216) << "Could not find decoration svg: aborting";
        abort();
    }
    m_frame.setImagePath(path);
    m_frame.setCacheAllRenderedFrames(true);
    m_frame.setEnabledBorders(Plasma::FrameSvg::AllBorders);

    // load the buttons
    initButtonFrame("minimize");
    initButtonFrame("maximize");
    initButtonFrame("restore");
    initButtonFrame("close");
    initButtonFrame("alldesktops");
    initButtonFrame("keepabove");
    initButtonFrame("keepbelow");
    initButtonFrame("shade");
    initButtonFrame("help");

    KConfig themeConf("aurorae/themes/" + m_themeName + '/' + m_themeName + "rc", KConfig::FullConfig, "data");
    m_themeConfig.load(&themeConf);    
}


void Factory::initButtonFrame(const QString& button)
{
    QString file("aurorae/themes/" + m_themeName + '/' + button + ".svg");
    QString path = KGlobal::dirs()->findResource("data", file);
    if (path.isEmpty()) {
        // let's look for svgz
        file.append("z");
        path = KGlobal::dirs()->findResource("data", file);
    }
    if (!path.isEmpty()) {
        Plasma::FrameSvg *frame = new Plasma::FrameSvg(this);
        frame->setImagePath(path);
        frame->setCacheAllRenderedFrames(true);
        frame->setEnabledBorders(Plasma::FrameSvg::NoBorder);
        m_buttons[ button ] = frame;
    } else {
        kDebug(1216) << "No button for: " << button;
    }
}


KDecoration* Factory::createDecoration(KDecorationBridge* bridge)
{
    return new Client(bridge, this);
}


bool Factory::reset(unsigned long changed)
{
    Q_UNUSED(changed);
    
    m_frame.clearCache();
    m_buttons.clear();
    init();
    return true; // Recreate all decorations
}


bool Factory::supports(Ability ability) const
{
    switch( ability ) {
        // announce
    case AbilityAnnounceButtons:
    case AbilityAnnounceColors:
        // buttons
    case AbilityButtonSpacer:
        return true;
    case AbilityButtonMinimize:
        return m_buttons.contains("minimize");
    case AbilityButtonMaximize:
        return m_buttons.contains("maximize");
    case AbilityButtonClose:
        return m_buttons.contains("close");
    case AbilityButtonAboveOthers:
        return m_buttons.contains("keepabove");
    case AbilityButtonBelowOthers:
        return m_buttons.contains("keepbelow");
    case AbilityButtonShade:
        return m_buttons.contains("shade");
    case AbilityButtonOnAllDesktops:
        return m_buttons.contains("alldesktops");
    case AbilityButtonHelp:
        return m_buttons.contains("help");
        // composition
    case AbilityProvidesShadow:
        return m_themeConfig.shadow();
    case AbilityUsesAlphaChannel:
        return true;
    default:
        return false;
    }
}


Plasma::FrameSvg* Factory::button(const QString& b)
{
    if (hasButton(b))
        return m_buttons[b];
    else
        return NULL;
}


} // namespace Chromi

#include "factory.moc"
