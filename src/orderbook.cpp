/**
 * @file order.cpp
 * @brief This file contains the implementation of the Orderbook class.
 */

#include <iostream>
#include <chrono>
#include <stdlib.h>
#include <map>
#include <thread>
#include <iomanip>
#include <memory>
#include <deque>

#include "../include/order.hpp"
#include "../include/orderbook.hpp"

using namespace std;

void Orderbook::add_order(int qty, double price, BookSide side) {
    auto order = std::make_unique<Order>(qty, price, side);
    uint64_t order_id = order->id;
    if (side == BookSide::bid) {
        m_bids[price].push_back(std::move(order));
        m_order_metadata[order_id] = make_pair(BookSide::bid, price); // cache
    } else {
        m_asks[price].push_back(std::move(order));
        m_order_metadata[order_id] = make_pair(BookSide::ask, price); // cache
    }
}

Orderbook::Orderbook(bool generate_dummies) {
    // seed RNG (using fixed seed for reproducibility)
    srand(12);

    if (generate_dummies) {
        // Add some dummy bid orders
        for (int i = 0; i < 3; i++) {
            double random_price = 90.0 + (rand() % 1001) / 100.0;
            int random_qty = rand() % 100 + 1;
            int random_qty2 = rand() % 100 + 1;
            
            add_order(random_qty, random_price, BookSide::bid);
            this_thread::sleep_for(chrono::milliseconds(1)); // ensure different timestamps
            add_order(random_qty2, random_price, BookSide::bid);
        }
        // Add some dummy ask orders
        for (int i = 0; i < 3; i++) {
            double random_price = 100.0 + (rand() % 1001) / 100.0;
            int random_qty = rand() % 100 + 1;
            int random_qty2 = rand() % 100 + 1;
            
            add_order(random_qty, random_price, BookSide::ask);
            this_thread::sleep_for(chrono::milliseconds(1));
            add_order(random_qty2, random_price, BookSide::ask);
        }
    }
}

// Template function to fill orders from the offers (deque) at each price level
template <typename T>
std::pair<int, double> Orderbook::fill_order(map<double, deque<unique_ptr<Order>>, T>& offers, 
                                               const OrderType type, const Side side, int& order_quantity,
                                               const double price, int& units_transacted, double& total_value) {
    // Iterate over the price levels (best prices first)
    auto rit = offers.begin();
    while(rit != offers.end()) {
        const double price_level = rit->first;
        auto& orders = rit->second;

        // For a limit order, ensure the price level is acceptable
        // market order always acceptable price
        bool can_transact = true;
        if (type == OrderType::limit) {
            if (side == Side::buy && price_level > price) {
                can_transact = false;
            } else if (side == Side::sell && price_level < price) {
                can_transact = false;
            }
        }

        if (can_transact) {
            // Process orders at this price level while there are orders and the incoming order is not fully filled
            while (!orders.empty() && order_quantity > 0) {
                auto& current_order = orders.front();
                const u_int64_t order_id = current_order->id;
                int current_qty = current_order->quantity;
                double current_price = current_order->price;

                if (current_qty > order_quantity) { // Partial fill
                    units_transacted += order_quantity;
                    total_value += order_quantity * current_price;
                    current_order->quantity = current_qty - order_quantity;
                    order_quantity = 0;
                    break; // Incoming order fully filled
                } else { // Full fill
                    units_transacted += current_qty;
                    total_value += current_qty * current_price;
                    order_quantity -= current_qty;
                    orders.pop_front();
                    // clean cache
                    m_order_metadata.erase(order_id);
                }
            }
            
            // remove map entry if we wiped all the orders 
            if (orders.empty()){
                rit = offers.erase(rit);
            }else{
                ++rit;
            }
        }else{
            // Prices will only get worse, break
            break;
        }
    }
    
    return std::make_pair(units_transacted, total_value);
}

// Handles market and limit orders, returning the total units transacted and total value
std::pair<int, double> Orderbook::handle_order(OrderType type, int order_quantity, Side side, double price) {
    int units_transacted = 0;
    double total_value = 0;

    if (type == OrderType::market) {
        if (side == Side::sell) {
            return fill_order(m_bids, OrderType::market, Side::sell, order_quantity, price, units_transacted, total_value);
        } else if (side == Side::buy) {
            return fill_order(m_asks, OrderType::market, Side::buy, order_quantity, price, units_transacted, total_value);
        }
    } else if (type == OrderType::limit) {
        if (side == Side::buy) {
            if (best_quote(BookSide::ask) <= price) {
                auto fill = fill_order(m_asks, OrderType::limit, Side::buy, order_quantity, price, units_transacted, total_value);
                if (order_quantity > 0)
                    add_order(order_quantity, price, BookSide::bid);
                return fill;
            } else {
                add_order(order_quantity, price, BookSide::bid);
                return std::make_pair(units_transacted, total_value);
            }
        } else { // Side::sell
            if (best_quote(BookSide::bid) >= price) {
                auto fill = fill_order(m_bids, OrderType::limit, Side::sell, order_quantity, price, units_transacted, total_value);
                if (order_quantity > 0)
                    add_order(order_quantity, price, BookSide::ask);
                return fill;
            } else {
                add_order(order_quantity, price, BookSide::ask);
                return std::make_pair(units_transacted, total_value);
            }
        }
    } else {
        throw std::runtime_error("Invalid order type encountered");
    }
    return std::make_pair(units_transacted, total_value);
}

// Returns the best quote (price) for the given book side
double Orderbook::best_quote(BookSide side) {
    if (side == BookSide::bid) {
        return m_bids.begin()->first;
    } else if (side == BookSide::ask) {
        return m_asks.begin()->first;
    } else {
        return 0.0;
    }
}

// Search through whole book and modify the target order
bool Orderbook::modify_order(uint64_t id, int new_qty) {
    auto [side, price] = m_order_metadata[id];

    auto modify_order_in_map = [&](auto& orders_map)->bool{
        for (auto& o:orders_map[price]) {
            if(o->id == id){
                o->quantity = new_qty;
                return true;
            }
        }
        return false;
    };

    if (side==BookSide::ask){
        return modify_order_in_map(m_asks);
    }else if (side==BookSide::bid){
        return modify_order_in_map(m_bids);
    }else{
        return false;
    }
}

// Sweep through the book 
bool Orderbook::delete_order(uint64_t id) {
    auto [side, price] = m_order_metadata[id];
    m_order_metadata.erase(id); // clean cache

    auto remove_from_map = [&](auto& orders_map) -> bool {
        // Iterate through orders of price level 
        auto& orders = orders_map[price]; 
        bool removed = false;

        for(auto qit = orders.begin(); qit!=orders.end(); ){
            if ((*qit)->id == id){
                orders.erase(qit);
                removed = true;
                break;
            }
            qit++;
        }

        // Check if we removed the last value in the queue
        if(orders.empty()){
            orders_map.erase(price);
        }
        return removed;

    };
    
    if (side==BookSide::bid){
        return remove_from_map(m_bids);         
    }else if(side==BookSide::ask){
        return remove_from_map(m_asks);
    }else{
        return false;
    }
}

// Template function to print a leg (bid or ask) of the order book.
template<typename T>
void Orderbook::print_leg(map<double, deque<unique_ptr<Order>>, T>& hashmap, BookSide side) {
    if (side == BookSide::ask) {
        for (auto it = hashmap.rbegin(); it != hashmap.rend(); ++it) { // iterate over price levels
            int size_sum = 0;
            for (auto& order : it->second) {
                size_sum += order->quantity;
            }
            string color = "31"; // red for asks
            cout << "\t\033[1;" << color << "m" << "$" << setw(6) << fixed << setprecision(2)
                 << it->first << setw(5) << size_sum << "\033[0m ";
            for (int i = 0; i < size_sum / 10; i++) {
                cout << "█";
            }
            cout << "\n";
        }
    } else if (side == BookSide::bid) {
        for (auto it = hashmap.begin(); it != hashmap.end(); ++it) {
            int size_sum = 0;
            for (auto& order : it->second) {
                size_sum += order->quantity;
            }
            string color = "32"; // green for bids
            cout << "\t\033[1;" << color << "m" << "$" << setw(6) << fixed << setprecision(2)
                 << it->first << setw(5) << size_sum << "\033[0m ";
            for (int i = 0; i < size_sum / 10; i++) {
                cout << "█";
            }
            cout << "\n";
        }
    }
}

void Orderbook::print() {
    cout << "========== Orderbook =========" << "\n";
    print_leg(m_asks, BookSide::ask);

    // Print bid-ask spread (in basis points)
    double best_ask = best_quote(BookSide::ask);
    double best_bid = best_quote(BookSide::bid);
    cout << "\n\033[1;33m" << "======  " << 10000 * (best_ask - best_bid) / best_bid << "bps  ======\033[0m\n\n";

    print_leg(m_bids, BookSide::bid);
    cout << "==============================\n\n\n";
}
