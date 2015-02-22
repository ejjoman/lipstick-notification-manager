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

void Application::listPlaceholders()
{
    QTextStream cout(stdout);

    QMap<QString, QString> placeholders = this->getPlaceholders();
    QMapIterator<QString, QString> iterator(placeholders);

    cout << "Available placeholders:" << endl;

    int maxPlayceholderWidth = 0;

    while (iterator.hasNext()) {
        iterator.next();

        int width = iterator.key().length();

        if (width > maxPlayceholderWidth)
            maxPlayceholderWidth = width;
    }

    iterator.toFront();

    while (iterator.hasNext()) {
        iterator.next();

        QString placeHolder = iterator.key();
        QString fieldName = iterator.value();

        cout << "  " << QString(placeHolder + ":").leftJustified(maxPlayceholderWidth + 2) << fieldName << endl;
    }
}

uint Application::getNotificationId(const LipstickNotification *notification) const
{
    QList<uint> ids = NotificationManager::instance()->notificationIds();

    foreach(uint id, ids) {
        LipstickNotification *n = NotificationManager::instance()->notification(id);

        if (n == notification)
            return id;
    }

    return -1;
}

QMap<QString, QString> Application::getPlaceholders() const
{
    QMap<QString, QString> placeholders;

    placeholders["an"] = "appName";
    placeholders["s"] = "summary";
    placeholders["b"] = "body";
    placeholders["cnt"] = "itemCount";
    placeholders["id"] = "id";
    placeholders["ps"] = "previewSummary";
    placeholders["pb"] = "previewBody";
    placeholders["e"] = "expireTimeout";
    placeholders["ts"] = "timestamp";
    placeholders["u"] = "urgency";
    placeholders["py"] = "priority";
    placeholders["cat"] = "category";

    return placeholders;
}

QString Application::parseStringList(const QStringList list) const
{
    QString result = QStringLiteral("[");

    foreach(QString item, list)
        result += "\n\t" + item + ",";

    if (list.count() > 0)
        result += "\n";

    result += "]";

    return result;
}

QString Application::parseVariantHash(const QVariantHash hash) const
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

void Application::listNotifications(QString format)
{
    QTextStream cout(stdout);

    QList<LipstickNotification*> *list = this->model->getList<LipstickNotification>();

    for (int i=0; i<list->count(); i++) {
        LipstickNotification *notification = list->at(i);
        QString notificationText = format;

        QMap<QString, QString> placeholders = this->getPlaceholders();
        QMapIterator<QString, QString> iterator(placeholders);

        while (iterator.hasNext()) {
            iterator.next();

            QString placeHolder = iterator.key();
            QString fieldName = iterator.value();

            QVariant fieldValue;

            if (fieldName == "id")
                fieldValue = QVariant(this->getNotificationId(notification));
            else
                fieldValue = notification->property(fieldName.toLocal8Bit().constData());

            bool hasValue = fieldValue.isValid();

            if (hasValue) {
                switch (fieldValue.type()) {
                case QVariant::String:
                    hasValue = !fieldValue.value<QString>().isEmpty();
                    break;

                case QVariant::Int:
                    hasValue = fieldValue.value<int>() != 0;
                    break;

                case QVariant::UInt:
                    hasValue = fieldValue.value<uint>() != 0;
                    break;

                default:
                    // hide warnings
                    break;
                }
            }

            notificationText.replace("%" + placeHolder + "?", hasValue ? fieldValue.toString() : "");
            notificationText.replace("%" + placeHolder, fieldValue.toString());
        }

        notificationText.replace("%h", this->parseVariantHash(notification->hints()));
        notificationText.replace("%act", this->parseStringList(notification->actions()));

        cout << notificationText;
    }
}

void Application::run()
{
    QCommandLineParser p;
    p.setApplicationDescription(QStringLiteral("Manager for lipstick notifications.\nMuch notification. Many disturb. Wow."));
    p.addHelpOption();
    p.addVersionOption();

    p.addPositionalArgument("command", "command to execute:\n   list, add*, delete*\n   if not specified, 'list' is used\n   * not implemented yet");

    QCommandLineOption formatOption(QStringList() << "f" << "format", "Specifies the format of the output when using the list-command.", "format", "%an - %cnt %s\n%b\n\n");
    p.addOption(formatOption);

    QCommandLineOption rawOption("raw", "Prints all fields");
    p.addOption(rawOption);

    QCommandLineOption idOption("id", "Set the notification id", "id");
    p.addOption(idOption);

    p.process(*this);

    QStringList availableCommands;
    availableCommands << "list" << "add" << "delete" << "placeholders";

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
                    "id:\t\t%id\n"
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

        // somewhere control characters are escaped... so hack this...
        format = format.replace("\\n", QChar(QChar::LineFeed));
        format = format.replace("\\t", QChar(QChar::Tabulation));

        this->listNotifications(format);
    } else if (command == "placeholders") {
        this->listPlaceholders();
    } else {
        QTextStream(stderr) << "command '" << command << "' is not implemeneted yet." << endl;

        this->exit(1);
        return;
    }

    this->exit();
}
