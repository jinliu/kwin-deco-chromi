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
#include <QTimer>

const int DEFAULT_TITLEBAR_WIDTH = 250;
const int DEFAULT_MINIMAL_WINDOW_WIDTH = DEFAULT_TITLEBAR_WIDTH * 2;
const int DEFAULT_WRITE_CONFIG_DELAY = 30;

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
    m_config.reset(new KConfig("chromirc"));
    KConfigGroup group(m_config.get(), "Engine");

    m_themeName = group.readEntry("ThemeName", "chrome");
    m_defaultTitlebarWidth = group.readEntry("DefaultTitlebarWidth", DEFAULT_TITLEBAR_WIDTH);
    m_minimalWindowWidth = group.readEntry("MinimalWindowWidth", DEFAULT_MINIMAL_WINDOW_WIDTH);

    m_windowConfigGroup.reset(new KConfigGroup(m_config.get(), "Window"));
    m_writeConfigTimer = new QTimer(this);
    m_writeConfigTimer->setInterval(group.readEntry("DefaultWriteConfigDelay", DEFAULT_WRITE_CONFIG_DELAY) * 1000);
    m_writeConfigTimer->setSingleShot(true);
    connect(
        m_writeConfigTimer, SIGNAL(timeout()),
        this, SLOT(writeConfig()));

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
    case AbilityButtonMenu:
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


int Factory::getTitlebarWidth(const QString& key) const
{
    return m_windowConfigGroup->readEntry(key, m_defaultTitlebarWidth);
}


void Factory::setTitlebarWidth(const QString& key, int width)
{
    m_windowConfigGroup->writeEntry(key, width);
    if (!m_writeConfigTimer->isActive())
        m_writeConfigTimer->start();
}


void Factory::writeConfig()
{
    m_windowConfigGroup->sync();
}

} // namespace Chromi

#include "factory.moc"
