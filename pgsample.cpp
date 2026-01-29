#include "user.hpp"
#include "portfolio.hpp"
#include "stock.hpp"
#include "friend.hpp"
#include "stocklist.hpp"

#include <iostream>
#include <pqxx/pqxx>

std::string user_id, username;

std::string connection_str = "dbname=postgres user=postgres password=postgres hostaddr=34.125.177.85 port=5432";

int main() {
    int choice;
    // User variables
    std::string email, password;

    // User variables
    std::string portfolio_name;
    int portfolio_id, portfolio_delete_id;

    // Interval menu variables
    std::string from_date, to_date;
    std::string interval;

    // Stock data variables
    std::string stock_sym, stock_sym2;
    std::string timestamp;
    double open, high, low, close;
    int volume;

    // Portfolio stock variables
    double amount;
    int quantity;

    // Social menu variables
    int request_id;
    int receiver_id;
    std::string response;

    // Stocklist menu variables
    std::string name, category, access;
    int stocklist_id;
    int stocklist_delete_id;
    int shared_user_id;

    // Review variables
    int review_id;
    int review_rating;
    std::string description;


    while (true) {
        if(!username.empty()) {
            std::cout << "User: " << username << std::endl;
        }
        std::cout << "Select an option:\n";
        std::cout << "1. Register\n";
        std::cout << "2. Login\n";
        std::cout << "3. Stock data menu\n";
        std::cout << "4. Portfolio menu\n";
        std::cout << "5. Edit stock menu\n";
        std::cout << "6. See social menu\n";
        std::cout << "7. Stocklist menu\n";
        std::cout << "8. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        // Check if the input is an integer and within the valid range
        if (std::cin.fail() || choice < 1 || choice > 8) {
            std::cin.clear(); // Clear the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            std::cout << "Invalid choice. Please enter a number between 1 and 7." << std::endl;
            continue; // Go back to the start of the loop
        }

        switch (choice) {
            case 1:
                std::cout << "Enter username: ";
                std::cin >> username;
                std::cout << "Enter email: ";
                std::cin >> email;
                std::cout << "Enter password: ";
                std::cin >> password;

                register_user(username, email, password);
                break;

            case 2:
                std::cout << "Enter email: ";
                std::cin >> email;
                std::cout << "Enter password: ";
                std::cin >> password;

                login_user(email, password);
                break;

            case 3: {
                int stock_choice;
                while (true) {  // Loop for the stock data menu
                    std::cout << "Select an option:\n";
                    std::cout << "1. Get latest stock data for a stock\n";
                    std::cout << "2. Get latest stock data for all stocks\n";
                    std::cout << "3. Get data given an interval for a stock\n";
                    std::cout << "4. Get covariance between two stocks\n";
                    std::cout << "5. Get beta for a stock\n";
                    std::cout << "6. Get coefficience of variance for a stock\n";
                    std::cout << "7. Go back to main menu\n";
                    std::cout << "Enter your choice: ";
                    std::cin >> stock_choice;

                    // Check if the input is an integer and within the valid range
                    if (std::cin.fail() || stock_choice < 1 || stock_choice > 7) {
                        std::cin.clear(); // Clear the error flag
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                        std::cout << "Invalid choice. Please enter a number between 1 and 7." << std::endl;
                        continue; // Go back to the start of the loop
                    }

                    switch (stock_choice) {
                        case 1:
                            std::cout << "Enter stock symbol: ";
                            std::cin >> stock_sym;
                            std::cout << "\n";
                            get_latest_stock(stock_sym);
                            break;

                        case 2:
                            get_all_latest_stocks();
                            break;

                        case 3:
                            int interval_choice;

                            while (true) {  // Loop for the interval menu
                                std::cout << "Select an interval:\n";
                                std::cout << "1. week\n";
                                std::cout << "2. month\n";
                                std::cout << "3. quarter\n";
                                std::cout << "4. year\n";
                                std::cout << "5. 5 years\n";
                                std::cout << "6. insert date range\n";
                                std::cout << "7. Go back to stock data menu\n";
                                std::cout << "Enter your choice: ";
                                std::cin >> interval_choice;

                                // Check if the input is an integer and within the valid range
                                if (std::cin.fail() || interval_choice < 1 || interval_choice > 7) {
                                    std::cin.clear(); // Clear the error flag
                                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                                    std::cout << "Invalid choice. Please enter a number between 1 and 9." << std::endl;
                                    continue; // Go back to the start of the loop
                                }

                                switch (interval_choice) {
                                    case 1:
                                        interval = "week";
                                        std::cout << "Enter stock symbol: ";
                                        std::cin >> stock_sym;

                                        view_stock_by_interval(stock_sym, interval);
                                        break;

                                    case 2:
                                        interval = "month";
                                        std::cout << "Enter stock symbol: ";
                                        std::cin >> stock_sym;

                                        view_stock_by_interval(stock_sym, interval);
                                        break;

                                    case 3:
                                        interval = "quarter";
                                        std::cout << "Enter stock symbol: ";
                                        std::cin >> stock_sym;
                                        view_stock_by_interval(stock_sym, interval);
                                        break;

                                    case 4:
                                        interval = "year";
                                        std::cout << "Enter stock symbol: ";
                                        std::cin >> stock_sym;
                                        view_stock_by_interval(stock_sym, interval);
                                        break;

                                    case 5:
                                        interval = "5 years";
                                        std::cout << "Enter stock symbol: ";
                                        std::cin >> stock_sym;
                                        view_stock_by_interval(stock_sym, interval);
                                        break;

                                    case 6:
                                        std::cout << "Enter the start date YYYY-MM-DD: ";
                                        std::cin >> from_date;
                                        std::cout << "Enter the end date YYYY-MM-DD: ";
                                        std::cin >> to_date;
                                        std::cout << "Enter stock symbol: ";
                                        std::cin >> stock_sym;
                                        view_stock_by_date_range(stock_sym, from_date, to_date);
                                        break;

                                    case 7:
                                        std::cout << "Returning to stock data menu." << std::endl;
                                        goto exit_stock_interval_menu; // Exit the interval menu loop
                                        break;

                                    default:
                                        std::cout << "Invalid choice. Please try again." << std::endl;
                                        break;
                                }
                            }
                            exit_stock_interval_menu:
                            break;

                        case 4:
                            std::cout << "Enter stock symbol 1: ";
                            std::cin >> stock_sym;
                            std::cout << "Enter stock symbol 2: ";
                            std::cin >> stock_sym2;
                            std::cout << "Covariance for " << stock_sym << ": " << calculateCovariance(stock_sym, stock_sym2) << std::endl;
                            break;

                        case 5:
                            std::cout << "Enter stock symbol: ";
                            std::cin >> stock_sym;
                            std::cout << "Beta for " << stock_sym << ": " << calculateBeta(stock_sym) << std::endl;
                            break;

                        case 6:
                            std::cout << "Enter stock symbol: ";
                            std::cin >> stock_sym;
                            std::cout << "Coefficient of variance for " << stock_sym << ": " << calculateCOV(stock_sym) << std::endl;
                            break;

                        case 7:
                            std::cout << "Returning to main menu." << std::endl;
                            goto exit_stock_menu; // Exit the stock menu loop
                            break;

                        default:
                            std::cout << "Invalid choice. Please try again." << std::endl;
                            break;
                    }
                }
                exit_stock_menu:
                break;
            }

            case 4: {
                if (username.empty()) {
                    std::cout << "Please login to access the portfolio menu." << std::endl;
                    break;
                }

                int portfolio_choice;
                while (true) {
                    std::cout << "Portfolios menu:\n";
                    std::cout << "1. Create a portfolio\n";
                    std::cout << "2. Delete a portfolio\n";
                    std::cout << "3. View a portfolio\n";
                    std::cout << "4. View portfolio dashboard\n";
                    std::cout << "5. Go back to main menu\n";
                    std::cout << "Enter your choice: ";
                    std::cin >> portfolio_choice;

                    if (std::cin.fail() || portfolio_choice < 1 || portfolio_choice > 5) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << "Invalid choice. Please enter a number between 1 and 5." << std::endl;
                        continue;
                    }

                    switch (portfolio_choice) {
                        case 1:
                            std::cout << "Enter portfolio name: ";
                            std::cin >> portfolio_name;
                            createPortfolio(std::stoi(user_id), portfolio_name);
                            break;

                        case 2:
                            std::cout << "Enter portfolio ID to delete: ";
                            std::cin >> portfolio_delete_id;
                            deletePortfolio(portfolio_delete_id);
                            break;

                        case 3:
                            std::cout << "Enter portfolio ID to view: ";
                            std::cin >> portfolio_id;

                            while (true) {
                                int portfolio_choice;

                                viewPortfolio(portfolio_id);
                                std::cout << "Select an option:\n";
                                std::cout << "1. View all stocks in the portfolio\n";
                                std::cout << "2. Deposit cash into the portfolio\n";
                                std::cout << "3. Withdraw cash from the portfolio\n";
                                std::cout << "4. Purchase stocks\n";
                                std::cout << "5. Sell stocks\n";
                                std::cout << "6. Go back to portfolio menu\n";
                                std::cout << "Enter your choice: ";
                                std::cin >> portfolio_choice;

                                if (std::cin.fail() || portfolio_choice < 1 || portfolio_choice > 6) {
                                    std::cin.clear();
                                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                    std::cout << "Invalid choice. Please enter a number between 1 and 6." << std::endl;
                                    continue;
                                }

                                switch (portfolio_choice) {

                                    case 1:
                                        viewPortfolioStocks(portfolio_id);
                                        break;

                                    case 2:
                                        std::cout << "Enter the amount to deposit: ";
                                        std::cin >> amount;
                                        depositCash(portfolio_id, amount);
                                    break;

                                    case 3:
                                        std::cout << "Enter the amount to withdraw: ";
                                        std::cin >> amount;
                                        withdrawCash(portfolio_id, amount);
                                        break;

                                    case 4:
                                        std::cout << "Enter the stock symbol: ";
                                        std::cin >> stock_sym;
                                        std::cout << "Enter the quantity: ";
                                        std::cin >> quantity;
                                        recordStockPurchase(portfolio_id, stock_sym, quantity);
                                        break;

                                    case 5:
                                        std::cout << "Enter the stock symbol: ";
                                        std::cin >> stock_sym;
                                        std::cout << "Enter the quantity: ";
                                        std::cin >> quantity;
                                        recordStockSale(portfolio_id, stock_sym, quantity);
                                        break;

                                    case 6:
                                        std::cout << "Returning to portfolio menu." << std::endl;
                                        goto exit_portfolio_view;
                                    
                                    default:
                                    std::cout << "Invalid choice. Please try again." << std::endl;
                                    break;
                                }
                            }
                            exit_portfolio_view:
                            break;

                        case 4:
                            std::cout << "Portfolios for User  " << username << ":\n";
                            viewPortfolios(std::stoi(user_id));
                            break;

                        case 5:
                            std::cout << "Returning to main menu." << std::endl;
                            goto exit_portfolio_menu;
                            break;

                        default:
                            std::cout << "Invalid choice. Please try again." << std::endl;
                            break;
                    }
                }
                exit_portfolio_menu:
                break;
            }

            case 5:
                int stock_edit_choice;
                while (true) {
                    std::cout << "Select an option:\n";
                    std::cout << "1. Record new daily stock information\n";
                    std::cout << "2. Go back to main menu\n";
                    std::cout << "Enter your choice: ";
                    std::cin >> stock_edit_choice;

                    if (std::cin.fail() || stock_edit_choice < 1 || stock_edit_choice > 2) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << "Invalid choice. Please enter a number between 1 and 2." << std::endl;
                        continue;
                    }

                    switch (stock_edit_choice) {
                        case 1:
                            std::cout << "Enter stock symbol: ";
                            std::cin >> stock_sym;
                            std::cout << "Enter date (YYYY-MM-DD): ";
                            std::cin >> timestamp;
                            std::cout << "Enter open price: ";
                            std::cin >> open;
                            std::cout << "Enter high price: ";
                            std::cin >> high;
                            std::cout << "Enter low price: ";
                            std::cin >> low;
                            std::cout << "Enter close price: ";
                            std::cin >> close;
                            std::cout << "Enter volume: ";
                            std::cin >> volume;
                            recordDailyStockInfo(stock_sym, timestamp, open, high, low, close, volume);
                            break;

                        case 2:
                            std::cout << "Returning to main menu." << std::endl;
                            goto exit_stock_edit_menu;
                            break;

                        default:
                            std::cout << "Invalid choice. Please try again." << std::endl;
                            break;
                    }
                }
                exit_stock_edit_menu:
                break;

            case 6:

                if (username.empty()) {
                    std::cout << "Please login to access the social menu." << std::endl;
                    break;
                }

                int social_choice;
                while (true) {
                    std::cout << "Select an option:\n";
                    std::cout << "1. Send friend request\n";
                    std::cout << "2. Respond to friend request\n";
                    std::cout << "3. View friend requests\n";
                    std::cout << "4. View users\n";
                    std::cout << "5. View friends\n";
                    std::cout << "6. Go back to main menu\n";
                    std::cout << "Enter your choice: ";
                    std::cin >> social_choice;

                    if (std::cin.fail() || social_choice < 1 || social_choice > 6) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << "Invalid choice. Please enter a number between 1 and 6." << std::endl;
                        continue;
                    }

                    switch (social_choice) {
                        case 1:
                            std::cout << "Enter receiver ID: ";
                            std::cin >> receiver_id;
                            send_friend_request(std::stoi(user_id), receiver_id);
                            break;

                        case 2:
                            std::cout << "Enter request ID: ";
                            std::cin >> request_id;
                            std::cout << "Enter response (accepted/rejected): ";
                            std::cin >> response;
                            respond_to_friend_request(request_id, response);
                            break;

                        case 3:
                            view_friend_requests(std::stoi(user_id));
                            break;

                        case 4:
                            print_all_users();
                            break;

                        case 5:
                            view_friends(std::stoi(user_id));  
                            break;

                        case 6:
                            std::cout << "Returning to main menu." << std::endl;
                            goto exit_social_menu;
                            break;

                        default:
                            std::cout << "Invalid choice. Please try again." << std::endl;
                            break;
                    }
                }
                exit_social_menu:
                break;

            case 7:
                int stocklist_choice;
                while (true) {
                    std::cout << "Select an option:\n";
                    std::cout << "1. Create a stocklist\n";
                    std::cout << "2. View own stocklists\n";
                    std::cout << "3. Change stocklist access\n";
                    std::cout << "4. Delete a stocklist\n";
                    std::cout << "5. Share a stocklist\n";
                    std::cout << "6. View accessible stocklists\n";
                    std::cout << "7. Write a review\n";
                    std::cout << "8. View reviews\n";
                    std::cout << "9. Delete a review\n";
                    std::cout << "10. View stocklist holdings\n";
                    std::cout << "11. Delete stocklist holdings\n";
                    std::cout << "12. Add stocklist holdings\n";
                    std::cout << "13. Go back to main menu\n";
                    std::cout << "Enter your choice: ";
                    std::cin >> stocklist_choice;

                    if (std::cin.fail() || stocklist_choice < 1 || stocklist_choice > 13) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << "Invalid choice. Please enter a number between 1 and 13." << std::endl;
                        continue;
                    }

                    switch (stocklist_choice) {
                        case 1:
                            std::cout << "Enter stocklist name: ";
                            std::cin >> name;
                            std::cout << "Enter stocklist category: ";
                            std::cin >> category;
                            std::cout << "Enter stocklist access (public/private): ";
                            std::cin >> access;
                            create_stocklist(std::stoi(user_id), name, category, access);
                            break;

                        case 2:
                            view_own_stocklists(std::stoi(user_id));
                            break;

                        case 3:
                            std::cout << "Enter stocklist ID: ";
                            std::cin >> stocklist_id;
                            std::cout << "Enter new access (public/private): ";
                            std::cin >> access;
                            change_stocklist_access(stocklist_id, access);
                            break;

                        case 4:
                            std::cout << "Enter stocklist ID to delete: ";
                            std::cin >> stocklist_delete_id;
                            delete_stocklist(stocklist_delete_id, std::stoi(user_id));
                            break;

                        case 5:
                            std::cout << "Enter stocklist ID: ";
                            std::cin >> stocklist_id;
                            std::cout << "Enter user ID to share with: ";
                            std::cin >> shared_user_id;
                            share_stocklist(stocklist_id, shared_user_id);
                            break;

                        case 6:
                            view_accessible_stocklists(std::stoi(user_id));
                            break;

                        case 7:
                            std::cout << "Enter stocklist ID: ";
                            std::cin >> stocklist_id;
                            std::cout << "Enter description: ";
                            std::cin >> description;
                            std::cout << "Enter rating: ";
                            std::cin >> review_rating;
                            write_review(stocklist_id, std::stoi(user_id), std::stoi(user_id), description, review_rating);
                            break;

                        case 8:
                            std::cout << "Enter stocklist ID: ";
                            std::cin >> stocklist_id;
                            view_reviews(stocklist_id, std::stoi(user_id));
                            break;

                        case 9:
                            std::cout << "Enter review ID: ";
                            std::cin >> review_id;
                            delete_review(review_id, std::stoi(user_id));
                            break;

                        case 10:
                            std::cout << "Enter stocklist ID: ";
                            std::cin >> stocklist_id;
                            view_stocklist_holdings(stocklist_id);
                            break;

                        case 11:
                            std::cout << "Enter stocklist ID: ";
                            std::cin >> stocklist_id;
                            std::cout << "Enter stock symbol: ";
                            std::cin >> stock_sym;
                            delete_stock_from_stocklist(stocklist_id, stock_sym, std::stoi(user_id));
                            break;  

                        case 12: 
                            std::cout << "Enter stocklist ID: ";
                            std::cin >> stocklist_id;
                            std::cout << "Enter stock symbol: ";
                            std::cin >> stock_sym;
                            std::cout << "Enter quantity: ";
                            std::cin >> quantity;
                            add_stock_to_stocklist(stocklist_id, stock_sym, quantity, std::stoi(user_id));
                            break;  

                        case 13:
                            std::cout << "Returning to main menu." << std::endl;
                            goto exit_stocklist_menu;
                            break;

                        default:
                            std::cout << "Invalid choice. Please try again." << std::endl;
                            break;
                    }
                }
                exit_stocklist_menu:
                break;

            case 8:
                std::cout << "Exiting the program." << std::endl;
                return 0;

            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                continue;
        }
    }

    return 0;
}