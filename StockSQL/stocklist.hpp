#ifndef STOCKLIST_HPP
#define STOCKLIST_HPP

#include <string>
#include <pqxx/pqxx>

extern std::string connection_str;


void create_stocklist(int creator_id, const std::string &name, const std::string &category, const std::string &access);
void change_stocklist_access(int stocklist_id, const std::string &access);
void delete_stocklist(int stocklist_id, int user_id);
void share_stocklist(int stocklist_id, int shared_user_id);
void view_accessible_stocklists(int user_id);
void view_own_stocklists(int user_id);
void view_stocklist_holdings(int stocklist_id);
void add_stock_to_stocklist(int stocklist_id, const std::string &symbol, int quantity, int user_id);
void delete_stock_from_stocklist(int stocklist_id, const std::string &symbol, int user_id);

#endif // STOCKLIST_HPP