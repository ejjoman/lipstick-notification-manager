#ifndef APPLICATION_H
#define APPLICATION_H

#include <QCoreApplication>
#include <QMap>

class LipstickNotification;
class NotificationListModel;
class QCommandLineParser;

class Application : public QCoreApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);
    ~Application();

private:
    void listNotifications(const QString format);
    void listPlaceholders();

    QString parseVariantHash(const QVariantHash hash) const;
    QString parseStringList(const QStringList list) const;

    QMap<QString, QString> getPlaceholders() const;

    uint getNotificationId(const LipstickNotification *notification) const;

signals:

public slots:
    void run();

private:
    NotificationListModel *model;

};

#endif // APPLICATION_H
