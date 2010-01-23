/********************************************************************
Copyright (C) 2009 Jin Liu <m.liu.jin@gmail.com>

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

#include "chromi.h"
#include "chromiclient.h"

extern "C"
{
KDE_EXPORT KDecorationFactory* create_factory()
{
    return new Chromi::ChromiFactory();
}
}

namespace Chromi
{

KDecoration* ChromiFactory::createDecoration(KDecorationBridge* bridge)
{
    return new ChromiClient(bridge, this);
}

bool ChromiFactory::supports(Ability ability) const
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

} // namespace Chromi

#include "chromi.moc"
