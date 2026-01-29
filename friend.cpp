#include <pqxx/pqxx>
#include "user.hpp"
#include "stock.hpp"
#include "portfolio.hpp"
#include "stocklist.hpp"
#include <iostream>

void send_friend_request(int sender_id, int receiver_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        // Check if a pending request already exists
        std::string checkQuery = 
            "SELECT * FROM friend_requests WHERE sender_id = " + W.quote(sender_id) +
            " AND receiver_id = " + W.quote(receiver_id) + " AND status = 'pending';";
        pqxx::result R = W.exec(checkQuery);

        if (R.size() > 0) {
            std::cerr << "A friend request is already pending." << std::endl;
            return;
        }

        // Insert the friend request into the database
        std::string insertQuery = 
            "INSERT INTO friend_requests (sender_id, receiver_id) VALUES (" + 
            W.quote(sender_id) + ", " + W.quote(receiver_id) + ");";
        W.exec(insertQuery);
        W.commit();

        std::cout << "Friend request sent from user " << sender_id << " to user " << receiver_id << "." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error sending friend request: " << e.what() << std::endl;
    }
}

void respond_to_friend_request(int request_id, const std::string &response) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        // Update the status of the friend request
        std::string updateQuery = 
            "UPDATE friend_requests SET status = " + W.quote(response) +
            " WHERE request_id = " + W.quote(request_id) + ";";
        W.exec(updateQuery);

        // Optionally, if accepted, you could add a record in a 'friends' table
        if (response == "accepted") {
            std::string getFriendRequestQuery = 
                "SELECT sender_id, receiver_id FROM friend_requests WHERE request_id = " + W.quote(request_id) + ";";
            pqxx::result R = W.exec(getFriendRequestQuery);
            if (R.size() > 0) {
                int sender_id = R[0]["sender_id"].as<int>();
                int receiver_id = R[0]["receiver_id"].as<int>();

                std::string insertFriendQuery = 
                    "INSERT INTO friends (user_id1, user_id2) VALUES (" + 
                    W.quote(sender_id) + ", " + W.quote(receiver_id) + ");";
                W.exec(insertFriendQuery);
            }
        }

        W.commit();
        std::cout << "Friend request " << request_id << " has been " << response << "." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error responding to friend request: " << e.what() << std::endl;
    }
}

void view_friend_requests(int user_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::nontransaction N(C);

        // SQL query to get pending friend requests along with the sender's username
        std::string query = 
            "SELECT fr.request_id, fr.sender_id, u.username FROM friend_requests fr "
            "JOIN users u ON fr.sender_id = u.user_id "
            "WHERE fr.receiver_id = " + N.quote(user_id) + " AND fr.status = 'pending';";
        pqxx::result R = N.exec(query);

        if (R.size() > 0) {
            std::cout << "Pending friend requests:" << std::endl;
            for (auto row : R) {
                std::cout << "Request ID: " << row["request_id"].as<int>()
                          << ", From User ID: " << row["sender_id"].as<int>()
                          << ", Username: " << row["username"].c_str() << std::endl;
            }
        } else {
            std::cout << "No pending friend requests." << std::endl;
        }

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error viewing friend requests: " << e.what() << std::endl;
    }
}


void view_friends(int user_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::nontransaction N(C);

        // SQL query to get all friends' ids and usernames for the given user_id
        std::string query = 
            "SELECT u.user_id, u.username FROM users u "
            "JOIN friends f ON (u.user_id = f.user_id1 OR u.user_id = f.user_id2) "
            "WHERE (f.user_id1 = " + N.quote(user_id) + " OR f.user_id2 = " + N.quote(user_id) + ") "
            "AND u.user_id != " + N.quote(user_id) + ";";
        
        pqxx::result R = N.exec(query);

        if (R.size() > 0) {
            std::cout << "Your Friends:" << std::endl;
            for (auto row : R) {
                std::cout << "Friend ID: " << row["user_id"].as<int>() 
                          << ", Username: " << row["username"].c_str() 
                          << std::endl;
            }
        } else {
            std::cout << "You have no friends added." << std::endl;
        }

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error retrieving friends: " << e.what() << std::endl;
    }
}

void write_review(int stocklist_id, int creator_id, int commentor_id, const std::string &description, int rating) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        // Check if the user already has a review for this stock list
        std::string checkQuery = "SELECT * FROM Reviews WHERE stocklist_id = " + W.quote(stocklist_id) + " AND commentor_id = " + W.quote(commentor_id) + ";";
        pqxx::result checkResult = W.exec(checkQuery);

        if (!checkResult.empty()) {
            std::cerr << "You have already reviewed this stocklist. Please edit your existing review." << std::endl;
            return;
        }

        std::string query = "INSERT INTO Reviews (stocklist_id, creator_id, commentor_id, description, rating) VALUES (" +
                            W.quote(stocklist_id) + ", " + W.quote(creator_id) + ", " + W.quote(commentor_id) + ", " + W.quote(description) + ", " + W.quote(rating) + ");";

        W.exec(query);
        W.commit();

        std::cout << "Review added successfully." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error writing review: " << e.what() << std::endl;
    }
}

void view_reviews(int stocklist_id, int user_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::nontransaction N(C);

        std::string query = "SELECT R.review_id, R.description, R.rating, U.username "
                      "FROM Reviews R "
                      "JOIN users U ON R.commentor_id = U.user_id "
                      "JOIN Stocklists S ON R.stocklist_id = S.stocklist_id "
                      "LEFT JOIN stocklist_access SA ON S.stocklist_id = SA.stocklist_id AND SA.shared_user_id = " + N.quote(user_id) + 
                      " WHERE R.stocklist_id = " + N.quote(stocklist_id) + 
                      " AND (S.creator_id = " + N.quote(user_id) + 
                      " OR S.access = 'public' " +
                      " OR SA.shared_user_id IS NOT NULL);";

        pqxx::result R = N.exec(query);

        if (R.size() > 0) {
            std::cout << "Reviews for Stocklist ID " << stocklist_id << ":" << std::endl;
            for (auto row : R) {
                std::cout << "Review ID: " << row["review_id"].as<int>()
                          << ", Username: " << row["username"].c_str()
                          << ", Rating: " << row["rating"].as<int>()
                          << ", Review: " << row["description"].c_str() << std::endl;
            }
        } else {
            std::cout << "No reviews found." << std::endl;
        }

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error viewing reviews: " << e.what() << std::endl;
    }
}


void delete_review(int review_id, int user_id) {
    try {
        pqxx::connection C(connection_str);
        pqxx::work W(C);

        // Check if the user is the creator or the reviewer of the review
        std::string checkQuery = "SELECT creator_id, commentor_id FROM Reviews WHERE review_id = " + W.quote(review_id) + ";";
        pqxx::result checkResult = W.exec(checkQuery);

        if (checkResult.empty() || (checkResult[0]["creator_id"].as<int>() != user_id && checkResult[0]["commentor_id"].as<int>() != user_id)) {
            std::cerr << "You do not have permission to delete this review." << std::endl;
            return;
        }

        // Delete the review
        std::string query = "DELETE FROM Reviews WHERE review_id = " + W.quote(review_id) + ";";
        W.exec(query);
        W.commit();

        std::cout << "Review ID " << review_id << " deleted successfully." << std::endl;

        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Error deleting review: " << e.what() << std::endl;
    }
}

