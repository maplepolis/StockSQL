#ifndef FRIEND_HPP
#define FRIEND_HPP

#include <string>
#include <pqxx/pqxx>

extern std::string connection_str;

void send_friend_request(int sender_id, int receiver_id);
void respond_to_friend_request(int request_id, const std::string &response);
void view_friend_requests(int user_id);
void view_friends(int user_id);

void write_review(int stocklist_id, int creator_id, int commentor_id, const std::string &description, int rating);
void view_reviews(int stocklist_id, int user_id);
void delete_review(int review_id, int user_id);

#endif