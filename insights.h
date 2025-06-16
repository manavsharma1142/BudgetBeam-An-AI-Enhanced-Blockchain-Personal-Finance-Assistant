// insights.h

#ifndef INSIGHTS_H
#define INSIGHTS_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>
#include <QTimer>
#include <QMessageBox>

class MainWindowWidget; // Forward declaration

void setRequestUrl(MainWindowWidget *mainWidget, const QString &question);
void onRequestFinished(MainWindowWidget *mainWidget, QNetworkReply *reply);

#endif // INSIGHTS_H
