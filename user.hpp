#ifndef USER_HPP
#define USER_HPP

#include <string>

extern std::string username;
extern std::string user_id;
extern std::string connection_str;

void register_user(const std::string& username, const std::string& email, const std::string& password);
bool login_user(const std::string& email, const std::string& password);
void print_all_users();

#endif
