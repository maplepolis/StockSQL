#include <pqxx/pqxx>
#include "user.hpp"
#include "stock.hpp"
#include "portfolio.hpp"
#include "friend.hpp"
#include <iostream>

// Create a new stocklist
void create_stocklist(int creator_id, const std::string &name, const std::string &category, const std::string &access) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        std::string query = "INSERT INTO Stocklists (creator_id, name, category, risk_profile, access) VALUES (" +
                            W.quote(creator_id) + ", " + W.quote(name) + ", " + W.quote(category) + ", 'Low', " + W.quote(access) + ");";

        W.exec(query);
        W.commit();

        std::cout << "Stocklist '" << name << "' created successfully with " << access << " access." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error creating stocklist: " << e.what() << std::endl;
    }
}

// Change the access level of a stocklist
void change_stocklist_access(int stocklist_id, const std::string &access) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        std::string query = "UPDATE Stocklists SET access = " + W.quote(access) + " WHERE stocklist_id = " + W.quote(stocklist_id) + ";";
        W.exec(query);
        W.commit();

        std::cout << "Stocklist ID " << stocklist_id << " access changed to " << access << "." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error changing stocklist access: " << e.what() << std::endl;
    }
}

// Delete a stocklist
void delete_stocklist(int stocklist_id, int user_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        // Check if the user is the creator of the stock list
        std::string checkQuery = "SELECT creator_id FROM Stocklists WHERE stocklist_id = " + W.quote(stocklist_id) + ";";
        pqxx::result checkResult = W.exec(checkQuery);

        if (checkResult.empty() || checkResult[0]["creator_id"].as<int>() != user_id) {
            std::cerr << "You do not have permission to delete this stocklist." << std::endl;
            return;
        }

        // Delete the stocklist
        std::string query = "DELETE FROM Stocklists WHERE stocklist_id = " + W.quote(stocklist_id) + ";";
        W.exec(query);
        W.commit();

        std::cout << "Stocklist ID " << stocklist_id << " deleted successfully." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error deleting stocklist: " << e.what() << std::endl;
    }
}

// Add a stock to a stocklist
void add_stock_to_stocklist(int stocklist_id, const std::string &symbol, int quantity, int user_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        std::string checkQuery = "SELECT creator_id FROM Stocklists WHERE stocklist_id = " + W.quote(stocklist_id) + ";";
        pqxx::result checkResult = W.exec(checkQuery);

        if (checkResult.empty() || checkResult[0]["creator_id"].as<int>() != user_id) {
            std::cerr << "You do not have permission to add to this stocklist." << std::endl;
            return;
        }

        double cov = calculateCOV(symbol);
        double beta = calculateBeta(symbol);

        std::string query = "INSERT INTO Stocklist_holdings (stocklist_id, symbol, quantity, cov, beta) VALUES (" +
                            W.quote(stocklist_id) + ", " + W.quote(symbol) + ", " + W.quote(quantity) + ", " + W.quote(cov) + ", " + W.quote(beta) + ");";

        W.exec(query);
        W.commit();

        std::cout << "Added " << quantity << " shares of " << symbol << " to stocklist ID " << stocklist_id << "." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error adding stock to stocklist: " << e.what() << std::endl;
    }
}

// Delete a stock from a stocklist
void delete_stock_from_stocklist(int stocklist_id, const std::string &symbol, int user_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        std::string checkQuery = "SELECT creator_id FROM Stocklists WHERE stocklist_id = " + W.quote(stocklist_id) + ";";
        pqxx::result checkResult = W.exec(checkQuery);

        if (checkResult.empty() || checkResult[0]["creator_id"].as<int>() != user_id) {
            std::cerr << "You do not have permission to delete this stocks from this stocklist." << std::endl;
            return;
        }

        std::string query = "DELETE FROM Stocklist_holdings WHERE stocklist_id = " + W.quote(stocklist_id) + " AND symbol = " + W.quote(symbol) + ";";
        W.exec(query);
        W.commit();

        std::cout << "Deleted " << symbol << " from stocklist ID " << stocklist_id << "." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error deleting stock from stocklist: " << e.what() << std::endl;
    }
}

// Share a stocklist with another user
void share_stocklist(int stocklist_id, int shared_user_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        std::string query = "INSERT INTO Stocklist_access (stocklist_id, shared_user_id) VALUES (" +
                            W.quote(stocklist_id) + ", " + W.quote(shared_user_id) + ");";

        W.exec(query);
        W.commit();

        std::cout << "Stocklist ID " << stocklist_id << " shared with user ID " << shared_user_id << "." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error sharing stocklist: " << e.what() << std::endl;
    }
}

// View stocklists that are accessible to the user
void view_accessible_stocklists(int user_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::nontransaction N(C);

        std::string query = "SELECT S.stocklist_id, S.name, S.category, S.risk_profile, S.access FROM Stocklists S "
                            "LEFT JOIN Stocklist_access SA ON S.stocklist_id = SA.stocklist_id "
                            "WHERE S.creator_id = " + N.quote(user_id) + 
                            " OR S.access = 'public' "
                            " OR SA.shared_user_id = " + N.quote(user_id) + ";";

        pqxx::result R = N.exec(query);

        if (R.size() > 0) {
            std::cout << "Accessible Stocklists:" << std::endl;
            for (auto row : R) {
                std::cout << "Stocklist ID: " << row["stocklist_id"].as<int>()
                          << ", Name: " << row["name"].c_str()
                          << ", Category: " << row["category"].c_str()
                          << ", Risk Profile: " << row["risk_profile"].c_str()
                          << ", Access: " << row["access"].c_str() << std::endl;
            }
        } else {
            std::cout << "No accessible stocklists found." << std::endl;
        }

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error viewing accessible stocklists: " << e.what() << std::endl;
    }
}

// View stocklists created by the user
void view_own_stocklists(int user_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::nontransaction N(C);

        std::string query = "SELECT stocklist_id, name, category, risk_profile, access FROM Stocklists WHERE creator_id = " + N.quote(user_id) + ";";
        pqxx::result R = N.exec(query);

        if (R.size() > 0) {
            std::cout << "Your Stocklists:" << std::endl;
            for (auto row : R) {
                std::cout << "Stocklist ID: " << row["stocklist_id"].as<int>()
                          << ", Name: " << row["name"].c_str()
                          << ", Category: " << row["category"].c_str()
                          << ", Risk Profile: " << row["risk_profile"].c_str()
                          << ", Access: " << row["access"].c_str() << std::endl;
            }
        } else {
            std::cout << "You have no stocklists created." << std::endl;
        }

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error viewing own stocklists: " << e.what() << std::endl;
    }
}

// View the holdings of a stocklist
void view_stocklist_holdings(int stocklist_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::nontransaction N(C);

        std::string query = "SELECT symbol, quantity, cov, beta FROM Stocklist_holdings WHERE stocklist_id = " + N.quote(stocklist_id) + ";";
        pqxx::result R = N.exec(query);

        if (R.size() > 0) {
            std::cout << "Stocklist ID " << stocklist_id << " Holdings:" << std::endl;
            for (auto row : R) {
                std::cout << "Symbol: " << row["symbol"].c_str()
                          << ", Quantity: " << row["quantity"].as<int>()
                          << ", COV: " << row["cov"].as<double>()
                          << ", Beta: " << row["beta"].as<double>() << std::endl;
            }
        } else {
            std::cout << "No holdings found for Stocklist ID " << stocklist_id << "." << std::endl;
        }

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error viewing stocklist holdings: " << e.what() << std::endl;
    }
}