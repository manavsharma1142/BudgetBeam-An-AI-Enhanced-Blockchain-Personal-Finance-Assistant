#ifndef MAINWINDOWWIDGET_H
#define MAINWINDOWWIDGET_H

#include <QMainWindow>

// include additional headers
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollbar>

#include "insights.h"
#include "blockchain.h"

namespace Ui {
class MainWindowWidget;
}

class MainWindowWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowWidget(QWidget *parent = nullptr);
    ~MainWindowWidget();

    void populateDeveloperTable();
    void populateLedgerTable();

    void loadFinancialData(const QString& csvFilePath);
    void setRequestUrl(const QString &question);
    void setRequestUrl(const QString &date, const QString &amount, const QString &category, const QString &description);

    void onRequestFinished(QNetworkReply *reply);
    void onSyncRequestFinished(QNetworkReply *reply);

    void loadBlocksFromCSV(const QString& filePath);

private slots:
    void on_finspireButton_clicked();

    void on_previousButton_clicked();

    void on_previousButtonContact_clicked();

    void on_contactButton_clicked();

    void on_expensesPreviousButton_clicked();

    void on_expensesButton_clicked();

    void on_saveButton_clicked();

    void on_insightsButton_clicked();

    void on_insightsPreviousButton_clicked();

    // api_networking component

    void on_askButton_clicked();

    void on_refreshButton_clicked();

    void on_morePower_stateChanged(int arg1);

    void on_maxPower_stateChanged(int arg1);

    // ledger components

    void on_ledgerButton_clicked();

    void on_ledgerPreviousButton_clicked();

    void on_settingsButton_clicked();

    void on_settingsPreviousButton_clicked();

    void on_deleteDataButton_clicked();

    void on_dashboardButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindowWidget *ui;
    QNetworkAccessManager *manager;
    Blockchain blockchain;

};

#endif // MAINWINDOWWIDGET_H
