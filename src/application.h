#ifndef APPLICATION_H
#define APPLICATION_H

#include <QCoreApplication>

class NotificationListModel;
class QCommandLineParser;

class Application : public QCoreApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);
    ~Application();

private:
    void list(const QString format);
    QString parseVariantHash(const QVariantHash hash);
    QString parseStringList(const QStringList list);

signals:

public slots:
    void run();

private:
    NotificationListModel *model;

};

#endif // APPLICATION_H
