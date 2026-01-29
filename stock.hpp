// stock.hpp

#ifndef STOCK_HPP
#define STOCK_HPP

#include <iostream>
#include <pqxx/pqxx>
#include <string>

extern std::string connection_str;

void get_latest_stock(const std::string& symbol);
void get_all_latest_stocks();
void view_stock_by_interval(const std::string &symbol, const std::string &interval);
void view_stock_by_date_range(const std::string &symbol, const std::string &from_date, const std::string &to_date);
void recordDailyStockInfo(const std::string &symbol, const std::string &timestamp, 
                            double open, double high, double low, double close, int volume);
double calculateCovariance(const std::string &symbol1, const std::string &symbol2);
double calculateBeta(const std::string& symbol);
double calculateCOV(const std::string& symbol);

#endif // STOCK_HPP
