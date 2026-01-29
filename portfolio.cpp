#include <pqxx/pqxx>
#include <string>
#include <iostream>
#include "portfolio.hpp"
#include "stock.hpp"

// Function to create a new portfolio for a user
void createPortfolio(int user_id, const std::string &portfolio_name) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        std::string query = "INSERT INTO portfolios (user_id, name, total_cash, risk_profile) VALUES (" +
                            W.quote(user_id) + ", " + W.quote(portfolio_name) + ", 0.0, 'Low');";

        W.exec(query);
        W.commit();

        std::cout << "Portfolio '" << portfolio_name << "' created successfully." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error creating portfolio: " << e.what() << std::endl;
    }
}

// Function to delete an existing portfolio
void deletePortfolio(int portfolio_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        // delete any associated stock holdings
        std::string deleteStocksQuery = "DELETE FROM portfolio_stocks WHERE portfolio_id = " + W.quote(portfolio_id) + ";";
        W.exec(deleteStocksQuery);

        // delete portfolio
        std::string deletePortfolioQuery = "DELETE FROM portfolios WHERE portfolio_id = " + W.quote(portfolio_id) + ";";
        W.exec(deletePortfolioQuery);

        W.commit();

        std::cout << "Portfolio with ID " << portfolio_id << " deleted successfully." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error deleting portfolio: " << e.what() << std::endl;
    }
}

// Function to print the details of a portfolio
void viewPortfolio(int portfolio_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::nontransaction N(C);

        std::string query = "SELECT name, total_cash, risk_profile FROM portfolios WHERE portfolio_id = " + N.quote(portfolio_id) + ";";
        pqxx::result R = N.exec(query);

        // Check if the portfolio exists
        if (R.size() == 1) {
            std::cout << "Portfolio Details:" << std::endl;
            std::cout << "Name: " << R[0]["name"].c_str() << std::endl;
            std::cout << "Total Cash: $" << R[0]["total_cash"].as<double>() << std::endl;
            std::cout << "Risk Profile: " << R[0]["risk_profile"].c_str() << std::endl;
        } else {
            std::cout << "Portfolio not found." << std::endl;
        }

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error viewing portfolio: " << e.what() << std::endl;
    }
}

// Function to view all portfolios for a user
void viewPortfolios(int user_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::nontransaction N(C);

        std::string query = "SELECT portfolio_id, name AS portfolio_name, risk_profile FROM portfolios WHERE user_id = " + N.quote(user_id) + ";";
        pqxx::result R = N.exec(query);

        if (R.size() > 0) {
            for (auto row : R) {
                std::cout << "Portfolio ID: " << row["portfolio_id"].as<int>() 
                          << ", Portfolio Name: " << row["portfolio_name"].c_str() 
                          << ", Risk Profile: " << row["risk_profile"].c_str()
                          << std::endl;
            }
        } else {
            std::cout << "No portfolios found." << std::endl;
        }

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error viewing portfolios: " << e.what() << std::endl;
    }
}

// Function to view all stocks in a portfolio
void viewPortfolioStocks(int portfolio_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::nontransaction N(C);

        // SQL query to retrieve the stocks in the specified portfolio
        std::string query = "SELECT symbol, quantity, price, cov, beta FROM portfolio_stocks WHERE portfolio_id = " + N.quote(portfolio_id) + ";";
        pqxx::result R = N.exec(query);

        // Check if any stocks are found in the portfolio
        if (R.size() > 0) {
            std::cout << "Stocks in Portfolio ID " << portfolio_id << ":" << std::endl;
            for (auto row : R) {
                std::string symbol = row["symbol"].c_str();
                int quantity = row["quantity"].as<int>();
                double price = row["price"].as<double>();
                double cov = row["cov"].as<double>();
                double beta = row["beta"].as<double>();

                std::cout << "Stock Symbol: " << symbol 
                          << ", Quantity: " << quantity 
                          << ", Price: $" << price 
                          << ", Beta: " << beta 
                          << ", COV: " << cov << std::endl;
            }
        } else {
            std::cout << "No stocks found in this portfolio." << std::endl;
        }


        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error viewing portfolio stocks: " << e.what() << std::endl;
    }
}


// Deposit cash into the portfolio's cash account
void depositCash(int portfolio_id, double amount) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);
        
        std::string query = "UPDATE portfolios SET total_cash = total_cash + " + W.quote(amount) +
                            " WHERE portfolio_id = " + W.quote(portfolio_id) + ";";
        W.exec(query);
        W.commit();
        
        std::cout << "Deposited " << amount << " to portfolio " << portfolio_id << std::endl;
        
        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

// Withdraw cash from the portfolio's cash account
void withdrawCash(int portfolio_id, double amount) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);
        
        // Attempt to withdraw the specified amount
        std::string query = "UPDATE portfolios SET total_cash = total_cash - " + W.quote(amount) +
                            " WHERE portfolio_id = " + W.quote(portfolio_id) + " AND total_cash >= " + W.quote(amount) + ";";
        pqxx::result R = W.exec(query);
        
        // Check if the withdrawal was successful
        if (R.affected_rows() == 0) {
            std::cout << "Cannot withdraw " << amount << " from portfolio " << portfolio_id << " because the total cash is insufficient." << std::endl;
        } else {
            W.commit();
            std::cout << "Withdrew " << amount << " from portfolio " << portfolio_id << std::endl;
        }

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void recordStockPurchase(int portfolio_id, const std::string &symbol, int quantity) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        // Get the latest stock price from the current_stocks table
        std::string priceQuery = "SELECT close FROM current_stocks WHERE symbol = " + W.quote(symbol) + " ORDER BY timestamp DESC LIMIT 1;";
        pqxx::result priceResult = W.exec(priceQuery);

        if (priceResult.empty()) {
            std::cerr << "No price data available for symbol: " << symbol << std::endl;
            return;
        }

        double price = priceResult[0]["close"].as<double>();
        double total_cost = quantity * price;

        // Check if the portfolio has enough cash
        std::string cashQuery = "SELECT total_cash FROM portfolios WHERE portfolio_id = " + W.quote(portfolio_id) + ";";
        pqxx::result cashResult = W.exec(cashQuery);

        if (cashResult.empty() || cashResult[0]["total_cash"].as<double>() < total_cost) {
            std::cerr << "Not enough cash available in portfolio to make the purchase." << std::endl;
            return;
        }

        // Update the cash balance
        withdrawCash(portfolio_id, total_cost);


        double cov = static_cast<float>(calculateCOV(symbol));
        double beta = static_cast<float>(calculateBeta(symbol));


        std::string checkQuery = "SELECT quantity, price FROM portfolio_stocks WHERE portfolio_id = " + W.quote(portfolio_id) + " AND symbol = " + W.quote(symbol) + ";";
        pqxx::result R = W.exec(checkQuery);

        if (R.size() == 1) {

            int existing_quantity = R[0]["quantity"].as<int>();
            double existing_price = R[0]["price"].as<float>();

            int new_quantity = existing_quantity + quantity;
            double new_price = ((existing_quantity * existing_price) + (quantity * price)) / new_quantity;


            std::string updateQuery = "UPDATE portfolio_stocks SET quantity = " + W.quote(new_quantity) +
                                      ", price = " + W.quote(new_price) +
                                      ", cov = " + W.quote(cov) +
                                      ", beta = " + W.quote(beta) +
                                      " WHERE portfolio_id = " + W.quote(portfolio_id) + " AND symbol = " + W.quote(symbol) + ";";
            W.exec(updateQuery);
        } else {

            std::string insertQuery = "INSERT INTO portfolio_stocks (portfolio_id, symbol, quantity, price, cov, beta) VALUES (" +
                                      W.quote(portfolio_id) + ", " + W.quote(symbol) + ", " + W.quote(quantity) + ", " + W.quote(price) + 
                                      ", " + W.quote(cov) + ", " + W.quote(beta) + ");";
            W.exec(insertQuery);
        }

        W.commit();

        std::cout << "Recorded purchase of " << quantity << " shares of " << symbol << " at " << price << " each." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}




void recordStockSale(int portfolio_id, const std::string &symbol, int quantity) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        std::string priceQuery = "SELECT close FROM current_stocks WHERE symbol = " + W.quote(symbol) + " ORDER BY timestamp DESC LIMIT 1;";
        pqxx::result priceResult = W.exec(priceQuery);

        if (priceResult.empty()) {
            std::cerr << "No price data available for symbol: " << symbol << std::endl;
            return;
        }

        double price = priceResult[0]["close"].as<double>();
        double total_revenue = quantity * price;

        std::string checkQuery = "SELECT quantity FROM portfolio_stocks WHERE portfolio_id = " + W.quote(portfolio_id) + " AND symbol = " + W.quote(symbol) + ";";
        pqxx::result R = W.exec(checkQuery);

        if (R.empty() || R[0]["quantity"].as<int>() < quantity) {
            std::cerr << "Not enough shares of " << symbol << " in portfolio to make the sale." << std::endl;
            return;
        }

        int new_quantity = R[0]["quantity"].as<int>() - quantity;

        if (new_quantity > 0) {
            std::string updateQuery = "UPDATE portfolio_stocks SET quantity = " + W.quote(new_quantity) +
                                      " WHERE portfolio_id = " + W.quote(portfolio_id) + " AND symbol = " + W.quote(symbol) + ";";
            W.exec(updateQuery);
        } else {
            std::string deleteQuery = "DELETE FROM portfolio_stocks WHERE portfolio_id = " + W.quote(portfolio_id) +
                                      " AND symbol = " + W.quote(symbol) + ";";
            W.exec(deleteQuery);
        }

        // Update the cash balance
        depositCash(portfolio_id, total_revenue);

        W.commit();

        std::cout << "Recorded sale of " << quantity << " shares of " << symbol << " at " << price << " each." << std::endl;

        // Recalculate the risk profile
        calculateRiskProfile(portfolio_id);

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}


void calculateRiskProfile(int portfolio_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        // Step 1: Retrieve the list of stocks in the portfolio
        std::string query = 
            "SELECT symbol, quantity FROM portfolio_stocks WHERE portfolio_id = " + W.quote(portfolio_id) + ";";
        pqxx::result R = W.exec(query);

        if (R.empty()) {
            std::cerr << "No stocks found in portfolio ID " << portfolio_id << "." << std::endl;
            return;
        }

        double totalBeta = 0.0;
        int covarianceScore = 0;
        double totalQuantity = 0.0;

        std::vector<std::string> symbols;
        std::vector<int> quantities;

        for (const auto& row : R) {
            std::string symbol = row["symbol"].c_str();
            int quantity = row["quantity"].as<int>();

            symbols.push_back(symbol);
            quantities.push_back(quantity);

            double beta = calculateBeta(symbol);
            totalBeta += beta * quantity;

            totalQuantity += quantity;
        }

        for (size_t i = 0; i < symbols.size(); ++i) {
            for (size_t j = i + 1; j < symbols.size(); ++j) {
                double covariance = calculateCovariance(symbols[i], symbols[j]);

                if (covariance > 0) {
                    covarianceScore += 1;
                } else if (covariance < 0) {
                    covarianceScore -= 1;
                }
            }
        }

        double averageBeta = totalBeta / totalQuantity;

        std::string riskProfile;
        if (averageBeta < 0.8 && covarianceScore <= 0) {
            riskProfile = "Low";
        } else if (averageBeta < 1.2 && covarianceScore >= 0) {
            riskProfile = "Medium";
        } else {
            riskProfile = "High";
        }

        std::string updateQuery = 
            "UPDATE portfolios SET risk_profile = " + W.quote(riskProfile) + 
            " WHERE portfolio_id = " + W.quote(portfolio_id) + ";";
        W.exec(updateQuery);
        W.commit();

        std::cout << "Risk profile updated for portfolio ID " << portfolio_id << "." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error calculating risk profile: " << e.what() << std::endl;
    }
}
