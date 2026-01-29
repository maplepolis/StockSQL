#include "user.hpp"
#include <iostream>
#include <pqxx/pqxx>

// Function to register a new user
void register_user(const std::string& username, const std::string& email, const std::string& password) {
    try {
        pqxx::connection C(connection_str);

        std::cout << "Connected to the database successfully: " << C.dbname() << std::endl;
        pqxx::work W(C);

        std::string sql = "INSERT INTO users (username, email, password) VALUES (" +
                            W.quote(username) + ", " +
                            W.quote(email) + ", " +
                            W.quote(password) + ");";

        W.exec(sql);
        W.commit();

        std::cout << "User registered successfully!" << std::endl;

        C.disconnect();

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

// Function to log in a user
bool login_user(const std::string& email, const std::string& password) {
    try {
        pqxx::connection C(connection_str);

        pqxx::nontransaction N(C);

        std::string sql = "SELECT user_id, username FROM users WHERE email = " + N.quote(email) +
                            " AND password = " + N.quote(password) + ";";

        pqxx::result R(N.exec(sql));

        if (R.size() == 1) {
            user_id = R[0][0].as<std::string>();
            username = R[0][1].as<std::string>();
            std::cout << "Login successful! Welcome, " << username << "." << std::endl;
            return true;
        } else {
            std::cout << "Login failed: Invalid email or password." << std::endl;
            return false;
        }

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

// Function to print all users
void print_all_users() {
    try {
        pqxx::connection C(connection_str);
        pqxx::nontransaction N(C);

        std::string query = "SELECT user_id, username FROM users;";
        pqxx::result R = N.exec(query);

        if (R.size() > 0) {
            std::cout << "All Users:" << std::endl;
            for (auto row : R) {
                std::cout << "User ID: " << row["user_id"].as<int>() 
                          << ", Username: " << row["username"].c_str() 
                          << std::endl;
            }
        } else {
            std::cout << "No users found." << std::endl;
        }

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error retrieving users: " << e.what() << std::endl;
    }
}
       