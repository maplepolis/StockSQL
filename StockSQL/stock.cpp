#include <iostream>
#include <pqxx/pqxx>
#include <chrono>
#include <ctime>
#include <string>
#include <cmath>
#include "stock.hpp"

// Function to get the latest stock data for a given symbol
void get_latest_stock(const std::string& symbol) {
    try {
        pqxx::connection C(connection_str);

        if (C.is_open()) {
            pqxx::nontransaction N(C);
            std::string sql = "SELECT timestamp, open, high, low, close, volume, symbol "
                              "FROM current_stocks "
                              "WHERE symbol = " + N.quote(symbol) + 
                              " ORDER BY timestamp DESC LIMIT 1;";

            pqxx::result R(N.exec(sql));

            if (R.empty()) {
                std::cout << "No data found for symbol: " << symbol << std::endl;
            } else {
                std::cout << "Latest stock data for " << symbol << ":" << std::endl;
                std::cout << "Timestamp: " << R[0][0].as<std::string>() << std::endl;
                std::cout << "Open: " << R[0][1].as<float>() << std::endl;
                std::cout << "High: " << R[0][2].as<float>() << std::endl;
                std::cout << "Low: " << R[0][3].as<float>() << std::endl;
                std::cout << "Close: " << R[0][4].as<float>() << std::endl;
                std::cout << "Volume: " << R[0][5].as<int>() << std::endl;
                std::cout << "Symbol: " << R[0][6].as<std::string>() << std::endl;
            }

            C.disconnect();

        } else {
            std::cout << "Can't open the database." << std::endl;
        }
        std::cout << "\n";

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

// Function to get the latest stock data for all symbols
void get_all_latest_stocks() {
    try {
        pqxx::connection C(connection_str);
        pqxx::nontransaction N(C);

        std::string sql = "SELECT DISTINCT ON (symbol) timestamp, open, high, low, close, volume, symbol "
                          "FROM current_stocks "
                          "ORDER BY symbol, timestamp DESC;";

        pqxx::result R(N.exec(sql));

        if (R.empty()) {
            std::cout << "No data found for any symbols." << std::endl;
        } else {
            std::cout << "Latest stock data for all symbols:" << std::endl;
            for (auto row : R) {
                std::cout << "Symbol: " << row["symbol"].c_str() << std::endl;
                std::cout << "Timestamp: " << row["timestamp"].c_str() << std::endl;
                std::cout << "Open: " << row["open"].as<float>() << std::endl;
                std::cout << "High: " << row["high"].as<float>() << std::endl;
                std::cout << "Low: " << row["low"].as<float>() << std::endl;
                std::cout << "Close: " << row["close"].as<float>() << std::endl;
                std::cout << "Volume: " << row["volume"].as<int>() << std::endl;
                std::cout << std::endl;
            }
        }

        C.disconnect();

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

// Function to record daily stock information in the database
void recordDailyStockInfo(const std::string &symbol, const std::string &timestamp, 
                            double open, double high, double low, double close, int volume) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        std::string checkQuery = "SELECT * FROM current_stocks WHERE symbol = " + W.quote(symbol) + 
                                 " AND timestamp = " + W.quote(timestamp) + ";";
        pqxx::result checkResult = W.exec(checkQuery);

        std::string query;

        if (checkResult.empty()) {
            query = 
                "INSERT INTO current_stocks (symbol, timestamp, open, high, low, close, volume) VALUES (" +
                W.quote(symbol) + ", " + W.quote(timestamp) + ", " + W.quote(open) + ", " + W.quote(high) + ", " +
                W.quote(low) + ", " + W.quote(close) + ", " + W.quote(volume) + ");";
        } else {
            query = 
                "UPDATE current_stocks SET open = " + W.quote(open) + 
                ", high = " + W.quote(high) + 
                ", low = " + W.quote(low) + 
                ", close = " + W.quote(close) + 
                ", volume = " + W.quote(volume) + 
                " WHERE symbol = " + W.quote(symbol) + 
                " AND timestamp = " + W.quote(timestamp) + ";";
        }

        W.exec(query);
        W.commit();

        std::cout << "Added or updated stock information for " << symbol << " on " << timestamp << " in current_stocks." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error adding or updating stock information: " << e.what() << std::endl;
    }
}


std::string calculateStartDate(const std::string &interval) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm *now_tm = std::localtime(&now_c);

    if (interval == "week") {
        now_tm->tm_mday -= 7;
    } else if (interval == "month") {
        now_tm->tm_mon -= 1;
    } else if (interval == "quarter") {
        now_tm->tm_mon -= 3;
    } else if (interval == "year") {
        now_tm->tm_year -= 1;
    } else if (interval == "5 years") {
        now_tm->tm_year -= 5;
    } else {
        std::cerr << "Invalid interval specified. Defaulting to one month." << std::endl;
        now_tm->tm_mon -= 1;
    }

    std::mktime(now_tm);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", now_tm);
    return std::string(buffer);
}

// Function to view stock data for a given symbol within a specified interval
void view_stock_by_interval(const std::string &symbol, const std::string &interval) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        std::string start_date = calculateStartDate(interval);

        std::string query = 
            "SELECT timestamp, close FROM ("
            "  SELECT timestamp, close FROM stocks WHERE symbol = " + W.quote(symbol) + 
            "  UNION ALL "
            "  SELECT timestamp, close FROM current_stocks WHERE symbol = " + W.quote(symbol) + 
            ") AS combined_data "
            "WHERE timestamp >= " + W.quote(start_date) + " ORDER BY timestamp ASC;";

        pqxx::result R = W.exec(query);

        if (R.size() > 0) {
            std::cout << "Close prices for " << symbol << " from " << start_date << " to today:" << std::endl;
            for (auto row : R) {
                std::cout << "Date: " << row["timestamp"].c_str() << ", Close Price: $" << row["close"].as<double>() << std::endl;
            }
        } else {
            std::cout << "No data found for " << symbol << " in the specified interval." << std::endl;
        }

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error viewing stock by interval: " << e.what() << std::endl;
    }
}

// Function to view stock data for a given symbol within a date range
void view_stock_by_date_range(const std::string &symbol, const std::string &from_date, const std::string &to_date) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        std::string query = 
            "SELECT timestamp, close FROM ("
            "  SELECT timestamp, close FROM Stocks WHERE symbol = " + W.quote(symbol) + 
            "  UNION ALL "
            "  SELECT timestamp, close FROM current_stocks WHERE symbol = " + W.quote(symbol) + 
            ") AS combined_data "
            "WHERE timestamp BETWEEN " + W.quote(from_date) + " AND " + W.quote(to_date) + 
            " ORDER BY timestamp ASC;";

        pqxx::result R = W.exec(query);

        // Check if any records were returned
        if (R.size() > 0) {
            std::cout << "Close prices for " << symbol << " from " << from_date << " to " << to_date << ":" << std::endl;
            for (auto row : R) {
                std::cout << "Date: " << row["timestamp"].c_str() << ", Close Price: $" << row["close"].as<double>() << std::endl;
            }
        } else {
            std::cout << "No data found for " << symbol << " in the specified date range." << std::endl;
        }

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error viewing stock by date range: " << e.what() << std::endl;
    }
}

// Function to calculate the covariance between two symbols
double calculateCovariance(const std::string &symbol1, const std::string &symbol2) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        // SQL query to retrieve close prices for both symbols along with timestamps
        std::string query = 
            "SELECT t1.timestamp, t1.close AS close1, t2.close AS close2 "
            "FROM ("
            "  SELECT timestamp, close FROM Stocks WHERE symbol = " + W.quote(symbol1) + 
            "  UNION ALL "
            "  SELECT timestamp, close FROM current_stocks WHERE symbol = " + W.quote(symbol1) + 
            ") AS t1 "
            "JOIN ("
            "  SELECT timestamp, close FROM Stocks WHERE symbol = " + W.quote(symbol2) + 
            "  UNION ALL "
            "  SELECT timestamp, close FROM current_stocks WHERE symbol = " + W.quote(symbol2) + 
            ") AS t2 "
            "ON t1.timestamp = t2.timestamp "
            "ORDER BY t1.timestamp ASC;";

        pqxx::result R = W.exec(query);

        int numReturns = R.size() - 1;
        if (numReturns < 1) {
            std::cerr << "Not enough overlapping data to calculate covariance between " << symbol1 << " and " << symbol2 << std::endl;
            return -1.0;
        }

        double sumReturns1 = 0.0;
        double sumReturns2 = 0.0;

        for (int i = 1; i <= numReturns; i++) {
            double currentPrice1 = R[i]["close1"].as<double>();
            double previousPrice1 = R[i-1]["close1"].as<double>();
            double return1 = (currentPrice1 - previousPrice1);
            sumReturns1 += return1;


            double currentPrice2 = R[i]["close2"].as<double>();
            double previousPrice2 = R[i-1]["close2"].as<double>();
            double return2 = (currentPrice2 - previousPrice2);
            sumReturns2 += return2;
        }

        double mean1 = sumReturns1 / numReturns;
        double mean2 = sumReturns2 / numReturns;

        double sumCovariance = 0.0;

        for (int i = 1; i <= numReturns; i++) {
            double currentPrice1 = R[i]["close1"].as<double>();
            double previousPrice1 = R[i-1]["close1"].as<double>();
            double return1 = (currentPrice1 - previousPrice1);

            double currentPrice2 = R[i]["close2"].as<double>();
            double previousPrice2 = R[i-1]["close2"].as<double>();
            double return2 = (currentPrice2 - previousPrice2);

            sumCovariance += (return1 - mean1) * (return2 - mean2);
        }

        double covariance = sumCovariance / numReturns;

        C.disconnect();
        return covariance;
    } catch (const std::exception &e) {
        std::cerr << "Error calculating covariance: " << e.what() << std::endl;
        return -1.0;
    }
}


// Function to calculate the beta of a stock
double calculateBeta(const std::string &symbol) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        // SQL query to retrieve close prices from both Stocks and current_stocks tables
        std::string query = 
            "SELECT close FROM ("
            "  SELECT timestamp, close FROM Stocks WHERE symbol = " + W.quote(symbol) + 
            "  UNION ALL "
            "  SELECT timestamp, close FROM current_stocks WHERE symbol = " + W.quote(symbol) + 
            ") AS combined_data "
            "ORDER BY timestamp ASC;";

        pqxx::result R = W.exec(query);

        if (R.size() < 2) {
            std::cerr << "Not enough data points to calculate Beta." << std::endl;
            return -1.0;
        }

        double sumStockReturns = 0.0;
        int numReturns = R.size() - 1;

        for (int i = 1; i <= numReturns; ++i) {
            double currentPrice = R[i]["close"].as<double>();
            double previousPrice = R[i-1]["close"].as<double>();
            double stockReturn = (currentPrice - previousPrice) / previousPrice;
            sumStockReturns += stockReturn;
        }

        double averageStockReturn = sumStockReturns / numReturns;
        double assumedMarketReturn = 0.05; // Assume 5% annual return
        double beta = averageStockReturn / assumedMarketReturn;

        C.disconnect();
        return beta;
    } catch (const std::exception &e) {
        std::cerr << "Error calculating Beta: " << e.what() << std::endl;
        return -1.0; // Return a sentinel value to indicate an error
    }
}

// Function to calculate the coefficient of variation (COV) of a stock
double calculateCOV(const std::string& symbol) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        std::string query =
            "SELECT close FROM ("
            "  SELECT timestamp, close FROM Stocks WHERE symbol = " + W.quote(symbol) +
            "  UNION ALL "
            "  SELECT timestamp, close FROM current_stocks WHERE symbol = " + W.quote(symbol) +
            ") AS combined_data "
            "ORDER BY timestamp ASC;";

        pqxx::result R = W.exec(query);

        if (R.size() < 2) {
            std::cerr << "Not enough data points to calculate COV." << std::endl;
            return -1.0;
        }

        double sum = 0.0;
        for (pqxx::result::size_type i = 0; i < R.size(); ++i) {
            sum += R[i]["close"].as<double>();
        }
        double mean = sum / R.size();

        double sumSquaredDiffs = 0.0;
        for (pqxx::result::size_type i = 0; i < R.size(); ++i) {
            sumSquaredDiffs += std::pow(R[i]["close"].as<double>() - mean, 2);
        }

        double standardDeviation = std::sqrt(sumSquaredDiffs / R.size());

        if (mean == 0) {
            std::cerr << "Mean return is zero, cannot calculate COV." << std::endl;
            return -1.0;
        }

        return standardDeviation / mean;
    } catch (const std::exception &e) {
        std::cerr << "Error calculating COV: " << e.what() << std::endl;
        return -1.0;
    }
}