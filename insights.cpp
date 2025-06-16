#include "insights.h"
#include "mainwindowwidget.h"
#include "ui_mainwindowwidget.h"

// API Networking
void MainWindowWidget::setRequestUrl(const QString &question)
{
    // Get the Azure Function App URL and access key from environment variables
    QByteArray azureFunctionAppUrl = qgetenv("AZURE_FUNCTION_APP_URL");
    QByteArray azureFunctionAppAccessKey = qgetenv("AZURE_FUNCTION_APP_ACCESS_KEY");

    // Convert QByteArray to QString
    QString urlStr = QString(azureFunctionAppUrl.constData());
    QString accessKeyStr = QString(azureFunctionAppAccessKey.constData());

    // Set the request URL and query parameters
    QUrl url(urlStr);

//    QUrl url("https://localhost:7071/api/qna-answering");

    QUrlQuery query;
    query.addQueryItem("question", question);
    query.addQueryItem("code", accessKeyStr);

    // Set the model parameter based on the checkbox selections
    QString model = "davinci";
    if (ui->morePower->isChecked()) {
        model = "gpt-3.5-turbo";
        qDebug() << "Using gpt-3.5-turbo";
    } else if (ui->maxPower->isChecked()) {
        model = "gpt-4-turbo";
        qDebug() << "Using gpt-4-turbo";
    } else {
        qDebug() << "Using davinci";
    }

    query.addQueryItem("model", model);
    url.setQuery(query);

    // Create a QNetworkRequest object and set the URL
    QNetworkRequest request(url);


//    // Display the answer in the GUI
    ui->fixedOne->setText("Your finance buddy is thinking... using 1 million IQ. 🤔");
    ui->insightsLabel->setText("");

    // Send the network request
    manager->get(request);
}

// an overloaded setRequestUrl function for handling real-time syncin
void MainWindowWidget::setRequestUrl(const QString &date, const QString &amount, const QString &category, const QString &description)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    // Connect the finished signal of the QNetworkAccessManager to a custom slot
    connect(manager, &QNetworkAccessManager::finished, this, &MainWindowWidget::onSyncRequestFinished);

    // Set the request URL and query parameters
    // QUrl url("http://localhost:7071/api/transaction");

    // Get the Azure Function App URL and access key from environment variables
    QByteArray azureFunctionAppUrl = qgetenv("AZURE_FUNCTION_APP_URL");
    QByteArray azureFunctionAppAccessKey = qgetenv("AZURE_TRANSACTION_KEY");

    // Convert QByteArray to QString
    QString urlStr = QString(azureFunctionAppUrl.constData());
    QString accessKeyStr = QString(azureFunctionAppAccessKey.constData());

    // Set the request URL and query parameters
    QUrl url(urlStr);

    QUrlQuery query;
    query.addQueryItem("code", accessKeyStr);
    query.addQueryItem("date", date);
    query.addQueryItem("amount", amount);
    query.addQueryItem("category", category);
    query.addQueryItem("description", description);

    url.setQuery(query);

    // Create a QNetworkRequest object and set the URL
    QNetworkRequest request(url);

    // Send the network request
    manager->get(request);
}

void MainWindowWidget::onSyncRequestFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        // The sync was successful
        qDebug() << "Sync successful";

        // Parse the JSON
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
        QJsonObject jsonObject = document.object();

        // Print the JSON object for debugging
        qDebug() << "JSON Object: " << document.toJson(QJsonDocument::Indented);

    } else {
        // There was an error with the sync
        qDebug() << "Sync error: " << reply->errorString();
    }

    reply->deleteLater();
}

void MainWindowWidget::onRequestFinished(QNetworkReply *reply)
{
    // Check if the request was successful
    if (reply->error() == QNetworkReply::NoError)
    {
        // Parse the JSON data
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
        QJsonObject jsonObject = document.object();

        // Print the JSON object for debugging
        qDebug() << "JSON Object: " << document.toJson(QJsonDocument::Indented);

        // Extract the answer from the JSON object
        QString answer = jsonObject["answer"].toString();
        QString question = jsonObject["question"].toString();

        // Display the answer in the GUI
        //        ui->fixedOne->setText("Your finance buddy is thinking... using 1 million IQ. 🤔");
        //        ui->insightsLabel->setText("");

        // Create a single-shot QTimer that will trigger after a delay
        QTimer::singleShot(3000, this, [=]()
                           {
                               // Set the font color to dark blue, the font size to 20px, and add a light gray border
                               ui->insightsLabel->setStyleSheet("QLabel { color : #00008B; font-size: 20px; border: 1px solid #D3D3D3; padding: 10px; }");
                               ui->insightsLabel->setText(answer);
                               ui->fixedOne->setText(question);
                               ui->lineEdit->setText("");
                               ui->maxPower->setDisabled(true);
                               ui->morePower->setDisabled(true);
                               ui->askButton->setEnabled(true);
                           });

        // Print the answer in the console
        qDebug() << "The answer to that question is " << answer;
    }
    else
    {
        ui->insightsLabel->setStyleSheet("QLabel { color : red; font-size:20px;border: 1px solid #D3D3D3; padding: 10px; }");

        // Inform the user that the request timed out
        ui->insightsLabel->setText("Getting rate limited. Wait 20s before asking another question, or use a paid account.");

        // Print the answer in the console
        qDebug() << "Page timed out.";
    }

    // Delete the QNetworkReply object
    reply->deleteLater();
}
