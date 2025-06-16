/'#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <openssl/sha.h>
#include <QString>
#include <QDateTime>
#include <QAtomicInt>

QString generateTransactionID();

struct Expense {
    QString date;
    double amount;
    QString category;
    QString description;
    QString hash;
    QString previousHash;

    QString transactionID = generateTransactionID();
};

class Blockchain {
private:
    std::vector<Expense> chain;

public:
    void addExpense(const Expense& expense);
    size_t getSize() const;
    void printExpenses() const;

    const std::vector<Expense>& getChain() const { return chain; }
    const Expense& operator[](size_t index) const {
        // Add bounds checking if necessary
        if (index < chain.size()) {
            return chain[index];
        } else {
            // Handle out-of-bounds error, throw an exception, or return a default value
            throw std::out_of_range("Index out of bounds");
        }
    }

private:
    QString generateHash(const Expense& expense) const;
};

#endif // BLOCKCHAIN_H
