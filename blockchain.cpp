#include "blockchain.h"
#include <QFile>
#include <QDir>

void Blockchain::addExpense(const Expense& expense) {
    // Generate hash for the new expense entry
    QString expenseHash = generateHash(expense);

    // Create a new expense object with the updated hash and previous hash
    Expense newExpense = expense;
    newExpense.hash = expenseHash;
    if (!chain.empty()) {
        newExpense.previousHash = chain.back().hash;
    }

    // Add the new expense entry to the blockchain
    chain.push_back(newExpense);

    // Save the blockchain to the file
    QString homePath = QDir::homePath() + "/BudgetBeam/.config";
    QString filePath = homePath + "/blockchain.txt";
}


size_t Blockchain::getSize() const {
    return chain.size();
}

void Blockchain::printExpenses() const {
    for (const auto& expense : chain) {
        std::cout << "Date: " << expense.date.toStdString() << std::endl;
        std::cout << "Amount: " << expense.amount << std::endl;
        std::cout << "Category: " << expense.category.toStdString() << std::endl;
        std::cout << "Description: " << expense.description.toStdString() << std::endl;
        std::cout << "Hash: " << expense.hash.toStdString() << std::endl;
        std::cout << "Previous Hash: " << expense.previousHash.toStdString() << std::endl;
        std::cout << "-------------------------" << std::endl;
    }
}

QString Blockchain::generateHash(const Expense& expense) const {
    std::stringstream ss;
    ss << expense.date.toStdString() << expense.amount << expense.category.toStdString() << expense.description.toStdString() << expense.previousHash.toStdString();

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(ss.str().c_str()), ss.str().length(), hash);

    std::stringstream hashStream;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        hashStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return QString::fromStdString(hashStream.str());
}

