#include "application.h"

#include <QTextStream>
#include <QDebug>
#include <QCommandLineParser>

#include "notificationmanager.h"
#include "notificationlistmodel.h"

Application::Application(int &argc, char **argv) :
    QCoreApplication(argc, argv)
{
    this->model = new NotificationListModel(this);
}

Application::~Application()
{
    delete(this->model);
}

QString Application::parseStringList(const QStringList list)
{
    QString result = QStringLiteral("[");

    foreach(QString item, list)
        result += "\n\t" + item + ",";

    if (list.count() > 0)
        result += "\n";

    result += "]";

    return result;
}

QString Application::parseVariantHash(const QVariantHash hash)
{
    QString result = QStringLiteral("[");

    QVariantHash::const_iterator i;
    for (i = hash.begin(); i != hash.end(); ++i)
         result += "\n\t" + i.key() + " : " + i.value().toString() + ",";

    if (hash.count() > 0)
        result += "\n";

    result += "]";

    return result;
}

void Application::list(QString format)
{
    // somewhere control characters are escaped... so hack this...
    format = format.replace("\\n", QChar(QChar::LineFeed));
    format = format.replace("\\t", QChar(QChar::Tabulation));

    QHash<QString, QString> placeholders;
    placeholders["an"] = "appName";
    placeholders["s"] = "summary";
    placeholders["b"] = "body";
    placeholders["cnt"] = "itemCount";
    placeholders["i"] = "id";
    placeholders["ps"] = "previewSummary";
    placeholders["pb"] = "previewBody";
    placeholders["e"] = "expireTimeout";
    placeholders["ts"] = "timestamp";
    placeholders["u"] = "urgency";
    placeholders["py"] = "priority";
    placeholders["cat"] = "category";

    //NotificationManager *m = NotificationManager::instance();
    //NotificationListModel *model = new NotificationListModel();


    //QList<uint> ids = m->notificationIds();

    QTextStream cout(stdout);

    QList<LipstickNotification*> *list = this->model->getList<LipstickNotification>();

    qDebug() << "Count Model:" << this->model->itemCount();
    qDebug() << "Count QObject:" << this->model->getList()->count();
    qDebug() << "Count LipstickNotification:" << list->count();

    //foreach (LipstickNotification &notification, list) {
    for (int i=0; i<list->count(); i++) {
        QString notificationText = format;

        LipstickNotification *notification = list->at(i);
        //QMetaObject metaObject = notification->metaObject();

        QHashIterator<QString, QString> i(placeholders);
        while (i.hasNext()) {
            i.next();

            QString placeHolder = i.key();
            QString fieldName = i.value();

            QVariant fieldValue = notification->property(fieldName.toLocal8Bit().constData());

            bool hasValue = fieldValue.isValid();

            if (hasValue) {
                switch (fieldValue.type()) {
                case QVariant::String:
                    hasValue = !fieldValue.value<QString>().isEmpty();
                    break;

                case QVariant::Int:
                    hasValue = fieldValue.value<int>() != 0;
                    break;
                }
            }

            notificationText = notificationText.replace("%" + placeHolder + "?", hasValue ? fieldValue.toString() : "");
            notificationText = notificationText.replace("%" + placeHolder, fieldValue.toString());
        }

        notificationText = notificationText.replace("%h", this->parseVariantHash(notification->hints()));
        notificationText = notificationText.replace("%act", this->parseStringList(notification->actions()));

        cout << notificationText;
    }
}

void Application::run()
{
    QCommandLineParser p;
    p.setApplicationDescription(QStringLiteral("Manager for lipstick notifications.\nMuch notification. Many disturb. Wow."));
    p.addHelpOption();
    p.addVersionOption();

    p.addPositionalArgument("command", "command to execute:\n   list, notify*, delete*\n   if not specified, 'list' is used\n   * not implemented yet");

    QCommandLineOption formatOption(QStringList() << "f" << "format", "Specifies the format of the output when using the list-command.", "format", "%an - %cnt %s\n%b\n\n");
    p.addOption(formatOption);

    QCommandLineOption rawOption(QStringList() << "a" << "all", "Prints all fields");
    p.addOption(rawOption);

    p.process(*this);

    QStringList availableCommands;
    availableCommands << "list" << "notify" << "delete";

    QString command = p.positionalArguments().value(0).trimmed().toLower();

    if (command.isEmpty())
        command = "list";

    if (!availableCommands.contains(command))
        p.showHelp(1);

    if (command == "list") {
        QString format;

        if (p.isSet(rawOption))
            format =
                    "appName:\t\t%an\n"
                    "summary:\t\t%s\n"
                    "body:\t\t\t%b\n"
                    "itemCount:\t\t%cnt\n"
                    "id:\t\t%i\n"
                    "previewSummary:\t\t%ps\n"
                    "previewBody:\t\t%pb\n"
                    "expireTimeout:\t\t%e\n"
                    "timestamp:\t\t%ts\n"
                    "urgency:\t\t%u\n"
                    "priority:\t\t%py\n"
                    "category:\t\t%cat\n"
                    "actions:   %act\n"
                    "hints:     %h\n\n\n";

        else
            format = p.value(formatOption);

        this->list(format);
        this->exit();
    } else {
        QTextStream(stderr) << "command '" << command << "' is not implemeneted yet." << endl;
        this->exit(1);
    }
}
