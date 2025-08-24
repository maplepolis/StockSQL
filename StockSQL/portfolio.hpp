#ifndef PORTFOLIO_HPP
#define PORTFOLIO_HPP

#include <string>
#include <pqxx/pqxx>

extern std::string connection_str;

void createPortfolio(int user_id, const std::string &portfolio_name);
void deletePortfolio(int portfolio_id);
void depositCash(int portfolio_id, double amount);
void withdrawCash(int portfolio_id, double amount);
void recordStockPurchase(int portfolio_id, const std::string &symbol, int quantity);
void recordStockSale(int portfolio_id, const std::string &symbol, int quantity);
void recordDailyStockInfo(const std::string &symbol, const std::string &date, double open, double high, double low, double close, int volume);
void viewPortfolios(int user_id);
void viewPortfolio(int portfolio_id);
void viewPortfolioStocks(int portfolio_id);
void calculateRiskProfile(int portfolio_id);

#endif // PORTFOLIO_HPP
