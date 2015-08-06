#ifndef VPNCOMPONENT_H
#define VPNCOMPONENT_H

#include <QObject>
#include <QList>

#include "dbus/dbusnetwork.h"
#include "plugincomponentinterface.h"

struct VPN {
    QString id;
    QString uuid;
    bool connected;
};

class QLabel;
class VPNComponent : public QObject, public PluginComponentInterface
{
    Q_OBJECT
public:
    friend class VPNApplet;

    VPNComponent(QObject *parent = 0);

    QString getId() Q_DECL_OVERRIDE;
    QString getName() Q_DECL_OVERRIDE;
    QString getTitle() Q_DECL_OVERRIDE;
    QString getCommand() Q_DECL_OVERRIDE;
    QWidget * getItem() Q_DECL_OVERRIDE;
    void retainItem() Q_DECL_OVERRIDE;
    QWidget * getApplet() Q_DECL_OVERRIDE;

    QString getMenuContent() Q_DECL_OVERRIDE;
    void invokeMenuItem(QString itemId, bool checked) Q_DECL_OVERRIDE;

signals:
    void vpnStatesChanged();

private:
    QLabel * m_item;
    QList<VPN> m_vpns;
    bool m_enabled;

    com::deepin::daemon::Network * m_dbusNetwork;

    void updateVPNs();
    bool isVPNConnected(QString path);
    bool isAnyVPNConnected();

private slots:
    void updateItem();
};

#endif // VPNCOMPONENT_H