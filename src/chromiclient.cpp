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

#include "chromiclient.h"

namespace Chromi
{

ChromiClient::ChromiClient(KDecorationBridge* bridge, KDecorationFactory* factory)
    : KDecoration(bridge, factory)
{}

void ChromiClient::init()
{}

ChromiClient::Position ChromiClient::mousePosition(const QPoint& p) const
{
    return PositionCenter;
}

void ChromiClient::borders(int& left, int& right, int& top, int& bottom) const
{}

void ChromiClient::resize( const QSize& s )
{}

QSize ChromiClient::minimumSize() const
{
    return QSize();
}

void ChromiClient::activeChange()
{}

void ChromiClient::captionChange()
{}

void ChromiClient::iconChange()
{}

void ChromiClient::maximizeChange()
{}

void ChromiClient::desktopChange()
{}

void ChromiClient::shadeChange()
{}

} // namespace Chromi

#include "chromiclient.moc"
