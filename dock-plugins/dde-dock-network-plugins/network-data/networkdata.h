#ifndef NETWORKDATA
#define NETWORKDATA
#include <QString>
#include "dbus/dbusnetwork.h"

#define ASYN_CALL(Fun, Code, captured...) { \
    QDBusPendingCallWatcher * watcher = new QDBusPendingCallWatcher(Fun, this); \
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [watcher, captured]{ \
        const QVariantList & args = watcher->reply().arguments(); \
        Code \
        watcher->deleteLater(); \
    }); }

namespace NetworkPlugin {

struct DeviceInfo {
    bool managed;
    bool usbDevice;
    int state;
    QString path;
    QString hwAddress;
    QString vendor;
    QString uniqueUnid;
    QString activeAp;
    QString mobileNetworkType;
    QString mobileSignalQuality;
};

struct ActiveConnectionInfo {
    bool isPrimaryConnection;
    QString connectionType;
    QString connectionName;
    QString mobileNetworkType;
    QString security;
    QString deviceType;
    QString deviceInterface;
    QString hwAddress;
    QString speed;
    QJsonObject ip4;
    QJsonObject ip6;
};

// device state
enum DeviceState {
    DeviceStateUnknown = 0,
    DeviceStateUnmanaged = 10,
    DeviceStateUnavailable = 20,
    DeviceStateDisconnected = 30,
    DeviceStatePrepare = 40,
    DeviceStateConfig = 50,
    DeviceStateNeedAuth = 60,
    DeviceStateIpConfig = 70,
    DeviceStateIpCheck = 80,
    DeviceStateSecondaries = 90,
    DeviceStateActivated = 100,
    DeviceStateDeactivating = 110,
    DeviceStateFailed = 120
};

// active connection state
enum ConnectionState {
    ActiveConnectionStateUnknown = 0,
    ActiveConnectionStateActivating = 1,
    ActiveConnectionStateActivated = 2,
    ActiveConnectionStateDeactivating = 3,
    ActiveConnectionStateDeactivate = 4
};

// connection type
const QString ConnectionTypeWired = "wired";
const QString ConnectionTypeWireless = "wireless";
const QString ConnectionTypeWirelessAdhoc = "wireless-adhoc";
const QString ConnectionTypeWirelessHotspot = "wireless-hotspot";
const QString ConnectionTypePppoe = "pppoe";
const QString ConnectionTypeMobile = "mobile";
const QString ConnectionTypeMobileGsm = "mobile-gsm";
const QString ConnectionTypeMobileCdma = "mobile-cdma";
const QString ConnectionTypeVpn = "vpn";
const QString ConnectionTypeVpnL2tp = "vpn-l2tp";
const QString ConnectionTypeVpnPptp = "vpn-pptp";
const QString ConnectionTypeVpnVpnc = "vpn-vpnc";
const QString ConnectionTypeVpnOpenvpn = "vpn-openvpn";
const QString ConnectionTypeVpnOpenconnect = "vpn-openconnect";

//public methods
QJsonArray deviceArray(const QString &deviceType, DBusNetwork *dbusNetwork);

QJsonArray connectiosArray(const QString &deviceType, DBusNetwork *dbusNetwork);

QString getApUuidBySsid(const QString &ssid, DBusNetwork *dbusNetwork);
ConnectionState getActiveConnectionsStateByUuid(const QString &uuid, DBusNetwork *dbusNetwork);
QStringList getActiveConnectionDevices(DBusNetwork *dbusNetwork);

DeviceInfo getDeviceInfoById(const QString &uuid, DBusNetwork *dbusNetwork);
QList<ActiveConnectionInfo> getActiveConnectionsInfo(DBusNetwork *dbusNetwork);

}

#endif // NETWORKDATA
