#include "biocustomtype.h"
#include <QDBusInterface>

QDBusArgument &operator<<(QDBusArgument &argument, const DeviceInfo &deviceInfo)
{
    argument.beginStructure();
    argument << deviceInfo.device_id << deviceInfo.device_shortname
        << deviceInfo.device_fullname << deviceInfo.driver_enable
        << deviceInfo.device_available
        << deviceInfo.biotype << deviceInfo.stotype
        << deviceInfo.eigtype << deviceInfo.vertype
        << deviceInfo.idtype << deviceInfo.bustype
        << deviceInfo.dev_status << deviceInfo.ops_status;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, DeviceInfo &deviceInfo)
{
    argument.beginStructure();
    argument >> deviceInfo.device_id >> deviceInfo.device_shortname
        >> deviceInfo.device_fullname >> deviceInfo.driver_enable
        >> deviceInfo.device_available
        >> deviceInfo.biotype >> deviceInfo.stotype
        >> deviceInfo.eigtype >> deviceInfo.vertype
        >> deviceInfo.idtype >> deviceInfo.bustype
        >> deviceInfo.dev_status >> deviceInfo.ops_status;
    argument.endStructure();
    return argument;
}
