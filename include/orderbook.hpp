/**
 * @file orderbook.hpp
 * @brief This file contains the declaration of the Orderbook class.
 * 
 * The Orderbook class represents an order book, which is a collection of buy and sell orders.
 * It provides functionality to add orders, execute orders, and retrieve the best quote.
 * The order book is implemented using two maps, one for buy orders (bids) and one for sell orders (asks).
 * Each map is sorted based on the price of the orders.
 * The Orderbook class also provides methods to clean up empty keys and print the order book.
 */

#pragma once

#include <deque>
#include <map>
#include <unordered_map>
#include <memory>
#include "enums.hpp"
#include "order.hpp"

class Orderbook {
private:
    std::map<double, std::deque<std::unique_ptr<Order>>, std::greater<double>> m_bids;
    std::map<double, std::deque<std::unique_ptr<Order>>, std::less<double>> m_asks;
    
    // Cache for modify/delete
    std::unordered_map<uint64_t, std::pair<BookSide, double>> m_order_metadata;
public:
    Orderbook(bool generate_dummies);

    void add_order(int qty, double price, BookSide side);
    std::pair<int, double> handle_order(OrderType type, int order_quantity, Side side, double price = 0);

    bool modify_order(uint64_t id, int new_qty);
    bool delete_order(uint64_t id);

    template <typename T>
    std::pair<int, double> fill_order(std::map<double, std::deque<std::unique_ptr<Order>>, T>& offers,
                                      const OrderType type, const Side side, int& order_quantity,
                                      double price, int& units_transacted, double& total_value);

    double best_quote(BookSide side);

    const auto& get_bids() { return m_bids; }
    const auto& get_asks() { return m_asks; }

    template<typename T>
    void print_leg(std::map<double, std::deque<std::unique_ptr<Order>>, T>& orders, BookSide side);

    void print();
};
