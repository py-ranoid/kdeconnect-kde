/**
 * Copyright 2013 Albert Vaca <albertvaka@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "batteryplugin.h"

#include <QDebug>
#include <KNotification>
#include <KIcon>

#include "batterydbusinterface.h"

K_PLUGIN_FACTORY( KdeConnectPluginFactory, registerPlugin< BatteryPlugin >(); )
K_EXPORT_PLUGIN( KdeConnectPluginFactory("kdeconnect_battery", "kdeconnect_battery") )

BatteryPlugin::BatteryPlugin(QObject *parent, const QVariantList &args)
    : KdeConnectPlugin(parent, args)
{
    batteryDbusInterface = new BatteryDbusInterface(parent);

    NetworkPackage np(PACKAGE_TYPE_BATTERY);
    np.set("request",true);
    device()->sendPackage(np);

}

BatteryPlugin::~BatteryPlugin()
{
    batteryDbusInterface->deleteLater();
}

bool BatteryPlugin::receivePackage(const NetworkPackage& np)
{

    if (np.type() != PACKAGE_TYPE_BATTERY) return false;

    bool isCharging = np.get<bool>("isCharging");
    int currentCharge = np.get<int>("currentCharge");

    if (batteryDbusInterface->isCharging() != currentCharge || batteryDbusInterface->isCharging() != isCharging) {

        batteryDbusInterface->updateValues(isCharging, currentCharge);

        if (currentCharge == 14 && !isCharging) {
            KNotification* notification = new KNotification("battery100");
            notification->setPixmap(KIcon("battery-040").pixmap(48, 48));
            notification->setComponentData(KComponentData("kdeconnect", "kdeconnect"));
            notification->setTitle(device()->name() + ": low battery");
            notification->setText("Battery at 14%");
        }

    }

    return true;

}
