#include "mainwindowwidget.h"
#include "ui_mainwindowwidget.h"
#include "insights.h"
#include "blockchain.h"

MainWindowWidget::MainWindowWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowWidget),
    manager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    // set the desired window size
    setFixedSize(679, 434);

    // set the window icon
    this->setWindowIcon(QIcon(":/icons/icon.png"));

    // set custom window title
    setWindowTitle("BudgetBeam");

    // always begin at main page
    ui->stackedWidget->setCurrentIndex(0);

    // set up contact page info
    populateDeveloperTable();

    // Connect the finished signal of the QNetworkAccessManager to a custom slot
    connect(manager, &QNetworkAccessManager::finished, this, &MainWindowWidget::onRequestFinished);

    // set up ledger table
    QString homePath = QDir::homePath() + "/BudgetBeam/.config";
    QString csvFilePath = homePath + "/financial_data.csv";

    loadBlocksFromCSV(csvFilePath);
}


MainWindowWidget::~MainWindowWidget()
{
    delete ui;
    delete manager;
}

// contact page
void MainWindowWidget::populateDeveloperTable() {
    // Create the table widget with the predefined number of rows and columns
    QTableWidget *table = new QTableWidget;
    table->setRowCount(4); // Set the number of developers
    table->setColumnCount(3); // For "Name", "Batch", "Email"
    table->setHorizontalHeaderLabels({"Name", "Batch", "Email"});

    // Set table properties
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    table->horizontalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "background-color: #EFEFEF;"
        "padding: 4px;"
        "border: 1px solid #DFDFDF;"
        "font-size: 10pt;"
        "font-weight: bold;"
        "text-align: center;"
        "}"
        );
    table->setStyleSheet(
        "QTableWidget {"
        "font-size: 10pt;"
        "}"
        "QTableWidget::item {"
        "padding: 5px;"
        "}"
        "QHeaderView::section {"
        "background-color: #EFEFEF;"
        "padding: 4px;"
        "border: 1px solid #DFDFDF;"
        "font-size: 10pt;"
        "font-weight: bold;"
        "text-align: center;"
        "}"
        );

    table->setAlternatingRowColors(true);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Add developer data to the table
    const QVector<QVector<QString>> developersData = {
        {"Aman Kediyal", "CSE AI-ML (NH) B2", "500091754@stu.upes.ac.in"},
        {"Baikunth Sharma", "CSE AI-ML (NH) B4", "500095004@stu.upes.ac.in"},
        {"Divye Mediratta", "CSE CCVT (NH) B3", "500093029@stu.upes.ac.in"},
        {"Manav Sharma", "CSE AI-ML (NH) B2", "500091978@stu.upes.ac.in"}
    };

    for (int row = 0; row < developersData.size(); ++row) {
        for (int col = 0; col < developersData[row].size(); ++col) {
            table->setItem(row, col, new QTableWidgetItem(developersData[row][col]));
        }
    }

    // Find the layout and add the table to it
    QVBoxLayout *layout = ui->contactPageLayout; // make sure this matches the objectName in Qt Designer
    layout->addWidget(table); // Add the table to the layout

    // Optionally, you can add a stretch to push everything to the top
    layout->addStretch(1);
}


// Function to read quotes from a file and return them as a QStringList
QStringList readQuotesFromFile(const QString &fileName) {
    QFile file(fileName);
    qDebug() << "Current working directory:" << QDir::currentPath();

    QStringList quotes;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            quotes << line;
            qDebug() << "Loaded quote:" << line;
        }
        file.close();
    } else {
        qDebug() << "Failed to open file:" << fileName;
    }
    return quotes;
}

// expenses page
void MainWindowWidget::loadFinancialData(const QString& csvFilePath) {
    QFile file(csvFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open CSV file for reading";
        return;
    }


    QTextStream stream(&file);

    // Make sure to initialize this table in your constructor or setup function
    QTableWidget *table = ui->expensesTable; // Reference the existing table widget
    table->setWordWrap(true);

    // Clear existing data in the table
    table->clear(); // This clears the entire table, including headers
    table->setRowCount(0);

    // Set up the table properties here (column count, headers, etc.)
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    table->setAlternatingRowColors(true);

    int singleStep = ui->expensesTable->rowHeight(0); // Assuming all rows have the same height
    ui->expensesTable->verticalScrollBar()->setSingleStep(singleStep);


    // Set the style sheet for the table
    QString styleSheet = R"(
    /* General style for the table */
    QTableWidget {
        border: 1px solid #d6d6d6;
        gridline-color: #d6d6d6;
        background-color: #f0f0f0;
    }

    /* Header style */
    QHeaderView::section {
        background-color: #e0e0e0;
        padding: 4px;
        border: 1px solid #d6d6d6;
        font-size: 10pt;
        font-weight: bold;
        text-align: center;
    }

    /* Alternating row colors */
    QTableWidget::item:alternate {
        background-color: #e8e8e8;
    }

    /* Vertical scrollbar styling */
    QScrollBar:vertical {
        border: none;
        background: white;
        width: 10px;
    }

    QScrollBar::handle:vertical {
        background-color: #626568;
        min-height: 20px; /* minimum handle height */
    }

    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
        background: none;
    }
)";

    table->setStyleSheet(styleSheet);


    // Read the first line to use as headers
    QStringList headers = stream.readLine().split(',');
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);

    // Read the rest of the file for data
    while (!stream.atEnd()) {
        QStringList rowData = stream.readLine().split(',');
        int currentRow = table->rowCount();
        table->insertRow(currentRow);
        for (int column = 0; column < rowData.size(); ++column) {
            table->setItem(currentRow, column, new QTableWidgetItem(rowData[column].trimmed()));
        }
    }

    file.close();
}

void MainWindowWidget::on_finspireButton_clicked()
{
    static QStringList quotes = readQuotesFromFile(":/data/src/financial_quotes.txt");
    if (!quotes.isEmpty()) {
        // Select a random quote
        int randomIndex = QRandomGenerator::global()->bounded(quotes.size());
        QString quoteLine = quotes.at(randomIndex);

        // Split the line into quote and author
        QStringList quoteParts = quoteLine.split('|');
        if (quoteParts.size() == 2) {
            QString quote = quoteParts.at(0).trimmed();
            QString author = quoteParts.at(1).trimmed();

            // Format the quote and author
            QString formattedText = QString("<p style='text-align: center; font-size: 14pt;'>"
                                            "<i>\"%1\"</i><br><br>"
                                            "<span style='font-size: 14pt; font-weight: bold;'>- %2</span>"
                                            "</p>")
                                        .arg(quote, author);

            // Find and set the label text
            QLabel *quoteLabel = ui->stackedWidget->widget(1)->findChild<QLabel *>();
            quoteLabel->setWordWrap(true);
            if (quoteLabel) {
                quoteLabel->setText(formattedText);
            }

            // Switch to the quote view in the stacked widget
            ui->stackedWidget->setCurrentIndex(1);
        } else {
            // Handle the unexpected format
            qDebug() << "Quote format not recognized. Ensure it is in 'quote | author' format.";
        }
    }
}

/**
 * page index reference:
 *	- mainpage = 0
 *  - finspire = 1
 *  - contact = 2
 *  - expenses = 3
 *  - insights = 4
 */

// finspirePrevious button
void MainWindowWidget::on_previousButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

// contactPage back button
void MainWindowWidget::on_previousButtonContact_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

// contact devs button
void MainWindowWidget::on_contactButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

// previous button for expenses page
void MainWindowWidget::on_expensesPreviousButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

}

// main page -> expenses
void MainWindowWidget::on_expensesButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

    // set the date field to current date
    ui->dateEdit->setDate(QDate::currentDate());

    // Load and display the financial data
    QString dirPath = QDir::homePath() + "/BudgetBeam/.config";
    QString filePath = dirPath + "/financial_data.csv";

    // Construct the file path relative to the application directory
    QFile file(filePath);

    // Check if the directory exists, if not, create it
    QDir dir(dirPath);
    if (!dir.exists()) {
        qDebug() << "Generating the user data directory in the home directory.";
        dir.mkpath(".");
    }

    if (!file.exists()) {
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Error", "Could not create the file.");
            return;
        }
        // Add the headers to the file when it's first created
        QTextStream stream(&file);
        stream << "Date,Amount,Category,Description\n";

        file.close();
    }

    loadFinancialData(filePath);
}

void MainWindowWidget::on_saveButton_clicked()
{
    // Collect data from input widgets
    QString date = ui->dateEdit->date().toString("dd-MM-yyyy");
    QString amount = ui->amountLineEdit->text();  // Replace with your actual object names
    QString category = ui->categoryComboBox->currentText();  // Replace with your actual object names
    QString description = ui->descriptionLineEdit->text();  // Replace with your actual object names

    // Validate amount - it should be a valid floating-point number
    bool ok;
    double amountValue = amount.toDouble(&ok);
    if (!ok) {
        // Show error message if amount is not a valid float
        QMessageBox::warning(this, "Invalid Entry", "Please enter a valid amount.");
        return;
    }

    // Format the data into a CSV string
    QStringList entryList = { date, amount, category, description };
    QString csvLine = entryList.join(",") + "\n";


    QString dirPath = QDir::homePath() + "/BudgetBeam/.config";
    QString filePath = dirPath + "/financial_data.csv";

    // Construct the file path relative to the application directory
    QFile file(filePath);

    if (!file.exists()) {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Error", "Could not find the file.");
            return;
        }
        file.close();
    }

    // Now you can open the file in append mode and write to it
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open the file for writing.");
        return;
    }

    QTextStream stream(&file);
    stream << csvLine;
    file.close();

    // Refresh the table
    loadFinancialData(filePath);

    // Optionally, clear the input fields after adding the entry
    ui->dateEdit->setDate(QDate::currentDate());
    ui->amountLineEdit->setText("");
    ui->categoryComboBox->setCurrentIndex(0);
    ui->descriptionLineEdit->setText("");

    qDebug() << "Saved: " << csvLine;

    // sync latest data with server
    setRequestUrl(date, amount, category, description);
    qDebug() << "Attempting to sync the data with the server...";
}


void MainWindowWidget::on_insightsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}


void MainWindowWidget::on_insightsPreviousButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}



// insights page api_networking

void MainWindowWidget::on_askButton_clicked()
{
    QString question = ui->lineEdit->text();

    // Check if the QLineEdit is empty
    if (question.trimmed().isEmpty())
    {
        // Show a message to the user
        QMessageBox::information(this, tr("Error"), tr("Please enter a question."));
        return;
    }

    ui->askButton->setDisabled(true);

    setRequestUrl(question);
}


void MainWindowWidget::on_refreshButton_clicked()
{
    ui->fixedOne->setText("What do you want to ask? :)");
    ui->insightsLabel->setText("");

    ui->morePower->setEnabled(true);
    ui->maxPower->setEnabled(true);

    ui->maxPower->setChecked(false); ;
    ui->morePower->setChecked(false);
    ui->lineEdit->setText("");
}


void MainWindowWidget::on_morePower_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        ui->maxPower->setDisabled(true);
    } else {
        ui->maxPower->setEnabled(true);
    }
}


void MainWindowWidget::on_maxPower_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        ui->morePower->setDisabled(true);
    } else {
        ui->morePower->setEnabled(true);
    }
}

void MainWindowWidget::on_ledgerButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

void MainWindowWidget::on_ledgerPreviousButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindowWidget::on_settingsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}

void MainWindowWidget::on_settingsPreviousButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindowWidget::on_deleteDataButton_clicked()
{
    // Get the home directory path
    QString homeDir = QDir::homePath();

    // Append the BudgetBeam directory to the home directory path
    QString budgetBeamDir = homeDir + "/BudgetBeam";

    // Create a QDir object for the BudgetBeam directory
    QDir dir(budgetBeamDir);

    // Check if the directory exists
    if(dir.exists())
    {
        // Show a message box asking the user to confirm the deletion
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Data", "Are you sure you want to delete all data?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            // Delete the directory and its contents
            dir.removeRecursively();
        }
    }
}

// Blockchain
void MainWindowWidget::populateLedgerTable() {
    // Assuming ledgerTable is a QTableWidget
    ui->ledgerTable->clearContents();

    // Set column headers
    ui->ledgerTable->setRowCount(static_cast<int>(blockchain.getSize()));
    ui->ledgerTable->setColumnCount(5);
    ui->ledgerTable->setHorizontalHeaderLabels({"TransactionID", "Date", "Amount", "Previous Hash", "Hash"});
    ui->ledgerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->ledgerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);


    // Populate the table with blockchain data
    for (size_t i = 0; i < blockchain.getSize(); ++i) {
        const Expense& expense = blockchain[i];

        QTableWidgetItem* transactionIDItem = new QTableWidgetItem(expense.transactionID); // Assuming expense has a member 'transactionID'

        QTableWidgetItem* dateItem = new QTableWidgetItem(expense.date);
        QTableWidgetItem* amountItem = new QTableWidgetItem(QString::number(expense.amount));  // Convert amount to QString
        QTableWidgetItem* previousHashItem = new QTableWidgetItem(expense.previousHash);
        QTableWidgetItem* hashItem = new QTableWidgetItem(expense.hash);

        ui->ledgerTable->setItem(i, 0, transactionIDItem);
        ui->ledgerTable->setItem(i, 1, dateItem);
        ui->ledgerTable->setItem(i, 2, amountItem);
        ui->ledgerTable->setItem(i, 3, previousHashItem);
        ui->ledgerTable->setItem(i, 4, hashItem);
    }

    qDebug() << "Blockchain size: " << blockchain.getSize();
}


Expense createExpense(const QString& date, double amount, const QString& category, const QString& description) {
    Expense expense;
    expense.date = date;
    expense.amount = amount;
    expense.category = category;
    expense.description = description;

    return expense;
}

QString generateTransactionID() {
    // Use a monotonically increasing counter
    static QAtomicInt counter(0);

    // Use current date and time to create a unique ID
    QDateTime currentDateTime = QDateTime::currentDateTime();

    // Use only the current hour in the unique ID
    QString uniqueID = currentDateTime.toString("yyyyMMddhh") + QString::number(counter.fetchAndAddOrdered(1));

    return uniqueID;
}

void MainWindowWidget::loadBlocksFromCSV(const QString& filePath) {
    blockchain = Blockchain();  // Clear existing blockchain data if needed

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);

        // Skip the header line
        in.readLine();

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(',');

            // Ensure there are enough fields to extract
            if (fields.size() >= 4) {
                QString date = fields[0].trimmed();
                double amount = fields[1].toDouble();
                QString category = fields[2].trimmed();
                QString description = fields.mid(3).join(',').trimmed();

                Expense expense = createExpense(date, amount, category, description);

                // Add the expense to the blockchain
                qDebug() << "Loaded block of expense: " << expense.date << expense.amount << expense.category << expense.description;
                blockchain.addExpense(expense);
            } else {
                qDebug() << "Could not load load the entry due to invalid count.";
            }
        }

        file.close();

        // After loading, update the ledger table
        populateLedgerTable();
    } else {
        // Handle file open error
    }
}


void MainWindowWidget::on_dashboardButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
}


void MainWindowWidget::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

