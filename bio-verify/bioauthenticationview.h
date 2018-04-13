#ifndef BIOAUTHENTICATIONVIEW_H
#define BIOAUTHENTICATIONVIEW_H

#include <QWidget>
#include "biocustomtype.h"

class QLabel;
class QPushButton;
class QMovie;
class BioAuthenticationView : public QWidget
{
    Q_OBJECT
public:
    explicit BioAuthenticationView(QWidget *parent = nullptr);
    void initUI();
    void startVerification();
    void stopVerification();
    void setUid(qint32 uid);
    void setDeviceInfo(const DeviceInfo& deviceInfo);
    void setMovie();

protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);

signals:
    void back();
    void messageNotify(const QString &message);
    void authenticationResult(bool result);

public slots:
    void onDeviceSearchResult(QDBusPendingCallWatcher *watcher);
    void onDeviceStatusChanged(int deviceId, int statusType);
    void onBackButtonClicked();

private:
    QLabel              *movieLabel;
    QLabel              *notificationLabel;
    QPushButton         *backButton;
    QTimer              *timer;
    QDBusInterface      *serviceInterface;
    QMovie              *movie;

    DeviceInfo          deviceInfo;
    qint32              uid;
    bool                stopped;
};

#endif // BIOAUTHENTICATIONVIEW_H
