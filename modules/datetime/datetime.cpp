#include "datetime.h"
#include "timewidget.h"
#include "moduleheader.h"
#include "timezonewidget.h"
#include "timezoneitemwidget.h"
#include "dbus/dbustimedate.h"

#include <QDebug>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QDesktopWidget>
#include <QDateTime>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dimagebutton.h>
#include <libdui/dapplication.h>

DUI_USE_NAMESPACE

Datetime::Datetime() :
    QObject(),
    m_frame(new QFrame),
    m_dbusInter(m_frame),
    m_timezoneListWidget(new SearchList),
    m_refershClockTimer(new QTimer(m_frame)),
    m_refershCalendarTimer(new QTimer(m_frame)),
    // config manager
    m_settings(new QSettings("deepin", "dde-control-center-datetime", this))
{
    QT_TR_NOOP("Date and Time");

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    loadZoneList();

    m_frame->installEventFilter(this);

    ModuleHeader *header = new ModuleHeader(tr("Date and Time"), false);

    m_dateCtrlWidget = new DateControlWidget;

    m_dateSeparator = new DSeparatorHorizontal;
    m_dateHeaderLine = new DHeaderLine;
    m_dateHeaderLine->setTitle(tr("Date"));
    m_dateHeaderLine->setContent(m_dateCtrlWidget);

    m_timezoneCtrlWidget = new TimezoneCtrlWidget;
    m_timezoneListWidget->setItemSize(310, 50);
    m_timezoneListWidget->setFixedWidth(310);

    m_timezoneHeaderLine = new DHeaderLine;
    m_timezoneHeaderLine->setTitle(tr("Time Zone"));
    m_timezoneHeaderLine->setContent(m_timezoneCtrlWidget);

    m_syncSeparator = new DSeparatorHorizontal;
    m_autoSyncSwitcher = new DSwitchButton;
    m_autoSyncSwitcher->setChecked(m_dbusInter.nTP());
    m_syncHeaderLine = new DHeaderLine;
    m_syncHeaderLine->setTitle(tr("Sync Automaticly"));
    m_syncHeaderLine->setContent(m_autoSyncSwitcher);

    m_clockSeparator = new DSeparatorHorizontal;
    m_clockFormatSwitcher = new DSwitchButton;
    m_clockFormatSwitcher->setChecked(m_dbusInter.use24HourFormat());
    m_clockHeaderLine = new DHeaderLine;
    m_clockHeaderLine->setTitle(tr("Use 24-hour clock"));
    m_clockHeaderLine->setContent(m_clockFormatSwitcher);

    m_calendarSeparator = new DSeparatorHorizontal;
    m_calendar = new DCalendar(m_frame);
    m_calendar->setFixedHeight(330);
    // if NOT zh_CN or zh_TW, hide lunar
    m_calendar->setLunarVisible(QLocale::system().name().contains("zh"));

    m_timeWidget = new TimeWidget;
    m_timeWidget->setEditable(!m_dbusInter.nTP());
    m_timeWidget->setIs24HourFormat(m_dbusInter.use24HourFormat());
    m_timeWidget->installEventFilter(this);

    showSelectedTimezoneList();

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addWidget(header);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(m_timeWidget);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(m_syncHeaderLine);
    centeralLayout->addWidget(m_syncSeparator);
    centeralLayout->addWidget(m_clockHeaderLine);
    centeralLayout->addWidget(m_clockSeparator);
    centeralLayout->addWidget(m_timezoneHeaderLine);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(m_timezoneListWidget);
    //centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(m_dateHeaderLine);
    centeralLayout->addWidget(m_dateSeparator);
    centeralLayout->addWidget(m_calendar);
    centeralLayout->addWidget(m_calendarSeparator);
    centeralLayout->addStretch(1);
    centeralLayout->setSpacing(0);
    centeralLayout->setMargin(0);

    m_frame->setLayout(centeralLayout);
    m_dateCtrlWidget->setVisible(!m_dbusInter.nTP());

    m_refershClockTimer->setInterval(1 * 1000);
    m_refershClockTimer->start();
    m_refershCalendarTimer->setInterval(15 * 1000);
    m_refershCalendarTimer->start();

    connect(m_refershClockTimer, &QTimer::timeout, m_timeWidget, &TimeWidget::updateTime);
    connect(&m_dbusInter, &DBusTimedate::TimezoneChanged, m_timeWidget, &TimeWidget::updateTime);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::addTimezoneAccept, this, &Datetime::addUserTimeZone);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::addTimezoneAccept, this, &Datetime::showSelectedTimezoneList);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::addTimezoneCancel, this, &Datetime::showSelectedTimezoneList);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::removeAccept, this, &Datetime::showSelectedTimezoneList);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::removeTimezone, this, &Datetime::toRemoveTimezoneMode);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::addTimezone, this, &Datetime::showTimezoneList);
    connect(m_clockFormatSwitcher, &DSwitchButton::checkedChanged, &m_dbusInter, &DBusTimedate::setUse24HourFormat);
    connect(&m_dbusInter, &DBusTimedate::Use24HourFormatChanged, [this] {m_timeWidget->setIs24HourFormat(m_dbusInter.use24HourFormat());});
    connect(&m_dbusInter, &DBusTimedate::Use24HourFormatChanged, m_timeWidget, &TimeWidget::updateTime);
    connect(&m_dbusInter, &DBusTimedate::NTPChanged, [this] {m_timeWidget->setEditable(!m_dbusInter.nTP());});
    connect(&m_dbusInter, &DBusTimedate::NTPChanged, [this] () -> void {m_dateCtrlWidget->setVisible(!m_dbusInter.nTP());});
    connect(&m_dbusInter, &DBusTimedate::TimezoneChanged, this, &Datetime::showSelectedTimezoneList);
    connect(m_refershCalendarTimer, &QTimer::timeout, m_timeWidget, [this] {
        if (QDate::currentDate() != m_calendar->getCurrentDate())
            m_calendar->setCurrentDate(QDate::currentDate());
    });
    connect(m_autoSyncSwitcher, &DSwitchButton::checkedChanged, [this] {
        QDBusPendingReply<> reply = m_dbusInter.SetNTP(m_autoSyncSwitcher->checked());
        reply.waitForFinished();
        if (reply.isError())
            m_autoSyncSwitcher->setChecked(m_dbusInter.nTP());
    });
    connect(m_timeWidget, &TimeWidget::applyTime, [this] (const QDateTime & time) -> void {
        qDebug() << "set time: " << time << time.currentMSecsSinceEpoch();
        m_dbusInter.SetDate(time.date().year(), time.date().month(), time.date().day(), time.time().hour(), time.time().minute(), time.time().second(), time.time().msec()).waitForFinished();
    });
    connect(m_dateCtrlWidget, &DateControlWidget::applyDate, [this] () -> void {
        const QDate date = m_calendar->getSelectDate();
        const QTime time = QTime::currentTime();
        qDebug() << "set date: " << date << time;

        QDBusPendingReply<> reply = m_dbusInter.SetDate(date.year(), date.month(), date.day(), time.hour(), time.minute(), time.second(), time.msec());
        reply.waitForFinished();
        if (!reply.isError())
            m_calendar->setCurrentDate(date);
    });
}

Datetime::~Datetime()
{
    qDebug() << "~Datetime";

    m_frame->setParent(nullptr);
    m_frame->deleteLater();
}

QFrame* Datetime::getContent()
{
    return m_frame;
}

const QString Datetime::getUTCOffset(int offset)
{
    const QString offsetHour = QString::number(abs(offset) / 3600);
    const QString offsetMinute = QString::number((abs(offset) % 3600) / 60);

    return std::move(QString("UTC%1%2:%3").arg(offset >= 0 ? '+' : '-')
                                          .arg(offsetHour, 2, '0')
                                          .arg(offsetMinute, 2, '0'));
}

const QString Datetime::getZoneCityListByOffset(int zoneOffset)
{
    QStringList list;
    for (const ZoneInfo & zone : *m_zoneInfoList)
        if (zone.getUTCOffset() == zoneOffset)
            if (zone.getZoneCity() == "Asia/Shanghai")
                list.append("Beijing");
            else
                list.append(zone.getZoneCity());
        else if (zone.getUTCOffset() > zoneOffset)
            break;

    return std::move(list.join(", "));
}

const ZoneInfo &Datetime::getZoneInfoByName(const QString &zoneName) const
{
    for (const ZoneInfo & zone : *m_zoneInfoList)
        if (zone.getZoneName() == zoneName)
            return zone;

    qWarning() << zoneName << "not in Timezone list!!!";

    // for default
    return m_zoneInfoList->first();
}

void Datetime::showSelectedTimezoneList()
{
    m_timezoneListWidget->clear();

    if (!m_clockHeaderLine->isVisible())
    {
        m_clockHeaderLine->show();
        m_syncHeaderLine->show();
        m_dateHeaderLine->show();
        m_calendar->show();
        m_clockSeparator->show();
        m_syncSeparator->show();
        m_calendarSeparator->show();
        m_dateSeparator->show();
    }

    const QString userZone = m_dbusInter.timezone();
    QStringList zoneList = m_dbusInter.userTimezones();
    int zoneNums = 0;

    for (const QString & zone : zoneList)
    {
        // pass default Timezone
        if (zone == "Etc/UTC")
            continue;

        ++zoneNums;

        const ZoneInfo & zoneInfo = getZoneInfoByName(zone);

        TimezoneWidget *zoneWidget = new TimezoneWidget(&zoneInfo);
        zoneWidget->setZoneCities(getZoneCityListByOffset(zoneInfo.getUTCOffset()));
        zoneWidget->setZoneUTCOffset(getUTCOffset(zoneInfo.getUTCOffset()));
        zoneWidget->setSelected(userZone == zoneInfo.getZoneName());

        connect(zoneWidget, &TimezoneWidget::clicked, [this, zoneWidget] () -> void {toggleTimeZone(zoneWidget);});

        m_timezoneListWidget->addItem(zoneWidget);
    }

    adjustItemHeight();

    // when list item count <= 1, control widget should hide "delete" button
    m_timezoneCtrlWidget->setListNums(zoneNums);
}

void Datetime::showTimezoneList()
{
    m_timezoneListWidget->clear();
    m_choosedZoneList.clear();

    m_clockHeaderLine->hide();
    m_syncHeaderLine->hide();
    m_dateHeaderLine->hide();
    m_calendar->hide();
    m_clockSeparator->hide();
    m_syncSeparator->hide();
    m_calendarSeparator->hide();
    m_dateSeparator->hide();

    QStringList userZoneList = m_dbusInter.userTimezones();
    int zoneNums = 0;
    int lastUTCOffset = -1;

    // m_zoneInfoList is sorted list.
    for (const ZoneInfo & zone : *m_zoneInfoList)
    {
        // skip repeat timezone
        if (zone.getUTCOffset() == lastUTCOffset)
            continue;
        lastUTCOffset = zone.getUTCOffset();

        // skip exist timezone
        if (userZoneList.contains(zone.getZoneName()))
            continue;

        ++zoneNums;

        TimezoneItemWidget *itemWidget = new TimezoneItemWidget(&zone);
        itemWidget->setZones(getZoneCityListByOffset(zone.getUTCOffset()));
        itemWidget->setUTCOffset(getUTCOffset(zone.getUTCOffset()));

        connect(itemWidget, &TimezoneItemWidget::clicked, this, &Datetime::timezoneItemChoosed);

        m_timezoneListWidget->addItem(itemWidget);
    }

    // 50 = module header + DSeparatorHorizontal
    int maxHeight = DApplication::desktop()->height();
    maxHeight -= m_timezoneHeaderLine->height() + 2 + m_timeWidget->height() + 2 + 50;

    m_timezoneListWidget->setFixedHeight(qMin(maxHeight, 50 * zoneNums));
}

void Datetime::toRemoveTimezoneMode()
{
    // update zone list.
    showSelectedTimezoneList();

    for (int i = 0; i != m_timezoneListWidget->count(); ++i)
    {
        TimezoneWidget *widget = qobject_cast<TimezoneWidget *>(m_timezoneListWidget->getItem(i)->widget());
        if (!widget)
            continue;

        widget->removeMode();
        widget->disconnect();

        connect(widget, &TimezoneWidget::clicked, [this, widget] () -> void {removeTimeZone(widget);});
    }
}

void Datetime::adjustItemHeight()
{
    // m_timezoneListWidget height
    int maxHeight = DApplication::desktop()->height();
    maxHeight -= m_timezoneHeaderLine->height() + 2 + m_timeWidget->height() + 2;
    maxHeight -= (m_syncHeaderLine->height() + 2) * 4 + m_calendar->height();
    m_timezoneListWidget->setFixedHeight(qMin(maxHeight, 50 * m_timezoneListWidget->count()));
}

bool Datetime::eventFilter(QObject *o, QEvent *e)
{
    if ((o == m_frame || o == m_timeWidget) &&
        e->type() == QEvent::Resize)
        adjustItemHeight();

    return false;
}

void Datetime::toggleTimeZone(TimezoneWidget *zone)
{
    if (!zone)
        return;

    qDebug() << "toggle zone: " << zone->zoneName();
    m_dbusInter.SetTimezone(zone->zoneName()).waitForFinished();
}

void Datetime::removeTimeZone(TimezoneWidget *zone)
{
    if (!zone)
        return;

    qDebug() << "remove zone: " << zone->zoneName();
    m_dbusInter.DeleteUserTimezone(zone->zoneName()).waitForFinished();
    //toRemoveTimezoneMode();
    const int i = m_timezoneListWidget->indexOf(zone);
    m_timezoneListWidget->removeItem(i);
    adjustItemHeight();
}

void Datetime::addUserTimeZone()
{
    qDebug() << "add zone list: " << m_choosedZoneList;
    for (const QString & zone : m_choosedZoneList)
        m_dbusInter.AddUserTimezone(zone).waitForFinished();
    m_choosedZoneList.clear();
}

void Datetime::timezoneItemChoosed()
{
    const TimezoneItemWidget *item = qobject_cast<TimezoneItemWidget *>(sender());

    if (item)
    {
        if (!item->selected())
            m_choosedZoneList.append(item->zoneName());
        else
            m_choosedZoneList.removeOne(item->zoneName());
    }

    m_timezoneCtrlWidget->setAcceptOrCancel(m_choosedZoneList.isEmpty());
}

void Datetime::loadZoneList()
{
    const QString zoneInfoListKey = "zoneInfoList";

    // get timezone info list
    m_zoneInfoList = new QList<ZoneInfo>;

    // try to read config file
    QByteArray bytes = m_settings->value(zoneInfoListKey).toByteArray();
    QDataStream readStream(&bytes, QIODevice::ReadOnly);
    readStream >> *m_zoneInfoList;

    // load success
    if (!m_zoneInfoList->empty())
        return;

    qDebug() << "load zoneInfoList from d-bus";

    // load config file error, read from d-bus
    QDBusPendingReply<QStringList> list = m_dbusInter.GetZoneList();
    list.waitForFinished();
    QStringList zoneList = list.value();

    for (const QString & zone : zoneList)
    {
        QDBusPendingReply<ZoneInfo> info;
        if (zone == "Asia/Beijing")
            info = m_dbusInter.GetZoneInfo("Asia/Shanghai");
        else
            info = m_dbusInter.GetZoneInfo(zone);

        info.waitForFinished();
        m_zoneInfoList->append(info.argumentAt<0>());
    }

    // sort by utc offset ascend, if utc offset is equal, sort by city.
    std::sort(m_zoneInfoList->begin(), m_zoneInfoList->end(), [this] (const ZoneInfo & z1, const ZoneInfo & z2) -> bool {
        if (z1.getUTCOffset() == z2.getUTCOffset())
            return z1.getZoneCity() < z2.getZoneCity();
        return z1.getUTCOffset() < z2.getUTCOffset();
    });

    QByteArray writeBytes;
    QDataStream writeStream(&writeBytes, QIODevice::WriteOnly);
    writeStream << *m_zoneInfoList;

    m_settings->setValue(zoneInfoListKey, writeBytes);
}
