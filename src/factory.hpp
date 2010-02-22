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

#include "themeconfig.h"

#include <kdecorationfactory.h>
#include <KDE/Plasma/FrameSvg>
#include <QHash>

#include <memory>

class KConfig;
class KConfigGroup;

namespace Chromi
{

using Aurorae::ThemeConfig;

class Factory : public QObject, public KDecorationFactory
{
    Q_OBJECT
public:
    Factory();
    /*override*/ KDecoration* createDecoration(KDecorationBridge* bridge);
    /*override*/ bool reset(unsigned long changed);
    /*override*/ bool supports(Ability ability) const;

    Plasma::FrameSvg* frame() { return &m_frame; }
    Plasma::FrameSvg* button(const QString& b);
    bool hasButton(const QString& button) const { return m_buttons.contains(button); }

    int getMinimalWindowWidth() const { return m_minimalWindowWidth; }
    const ThemeConfig& themeConfig() const { return m_themeConfig; }
    int getTitlebarWidth(const QString& key) const;
    void setTitlebarWidth(const QString& key, int width);
public slots:
    void writeConfig();
private:
    void init();
    void initButtonFrame(const QString& button);

    Plasma::FrameSvg m_frame;
    QHash<QString, Plasma::FrameSvg*> m_buttons;

    std::auto_ptr<KConfig> m_config;
    int m_defaultTitlebarWidth;
    int m_minimalWindowWidth;

    std::auto_ptr<KConfigGroup> m_windowConfigGroup;
    QTimer* m_writeConfigTimer;
    
    QString m_themeName;
    ThemeConfig m_themeConfig;
};

}

#endif
