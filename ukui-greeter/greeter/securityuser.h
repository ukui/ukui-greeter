#ifndef SECURITYUSER_H
#define SECURITYUSER_H

#include <QStringList>
#include <QString>

enum SECURITYTYPE
{
    NORMAL,
    WHITELIST,
    BLACKLIST
};

class SecurityUser
{
public:
    SecurityUser();
    int getMode();
    QStringList getBlackList();
    QStringList getWhiteList();
    bool isSecrityUser(QString username);
    static SecurityUser *instance();

private:
    int mode;
    QStringList blacklist;
    QStringList whitelist;

    void loadJsonFile();

    static SecurityUser    *instance_;
};

#endif // SECURITYUSER_H
