/**
 * @file order.cpp
 * @brief This file contains the implementation of the Orderbook class.
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <stdlib.h> 
#include <map>
#include <thread>
#include <iomanip>
#include <memory>

#include "../include/order.hpp"
#include "../include/orderbook.hpp"

void Orderbook::add_order(int qty, double price, BookSide side){
    auto order = std::make_unique<Order>(qty, price, side);
	if(side == BookSide::bid){
		bids[price].push_back(std::move(order)); // std::move avoids copying the smart pointer
	}else{
		asks[price].push_back(std::move(order));
	}
}

Orderbook::Orderbook(bool generate_dummies){
	// seed RNG
	// srand (time(NULL)); 
    srand(12); // For reproducibility 
   
    if (generate_dummies){
        // Add some dummy orders
        for (int i=0; i<3; i++){ 
            double random_price = 90.0 + (rand() % 1001) / 100.0;
            int random_qty = rand() % 100 + 1; 
            int random_qty2 = rand() % 100 + 1;
            
            add_order(random_qty, random_price, BookSide::bid);
            this_thread::sleep_for(chrono::milliseconds(1)); // Sleep for a millisecond so the orders have different timestamps
            add_order(random_qty2, random_price, BookSide::bid);
        }
        for (int i=0; i<3; i++){
            double random_price = 100.0 + (rand() % 1001) / 100.0;
            int random_qty = rand() % 100 + 1; 
            int random_qty2 = rand() % 100 + 1; 			
            
            add_order(random_qty, random_price, BookSide::ask);
            this_thread::sleep_for(chrono::milliseconds(1)); // Sleep for a millisecond so the orders have different timestamps
            add_order(random_qty2, random_price, BookSide::ask);
        }
    }
}

// Template because we order the map differently in bids/asks
template<typename T>
void Orderbook::clean_leg(map<double, vector<unique_ptr<Order>>, T>& map){
    // Using iterator because we delete elements potentially
	for (auto it = map.begin(); it != map.end(); ) {
		if (it->second.empty()) { // Check if the vector associated with the current key is empty
			it = map.erase(it);  // Erase the key and update the iterator to the next element
		} else {
			++it; // Move to the next element
		}
	}
}

template <typename T>
std::pair<int,double> Orderbook::fill_order(map<double, vector<unique_ptr<Order>>, T>& offers, 
                                 const OrderType type, const Side side, int& order_quantity,
                                 const double price, int& units_transacted, double& total_value){
    // Iterate through bids/offers map
    for(auto rit = offers.rbegin(); rit != offers.rend();) {
        auto& pair = *rit; // Dereference iterator to get the key-value pair
        
        double price_level = pair.first;
        auto& quotes = pair.second; // Order vector


        // Ensure agreeable price for limit order
        bool can_transact = true; 
        if (type == OrderType::limit){
            if (side == Side::buy && price_level > price){
                can_transact = false;
            }else if(side == Side::sell && price_level < price){
                can_transact = false;
            }
        }
        
        // Try to transact if we can
        if (can_transact){
            // we have order_quantity, iterate quote at this price level and fill it  
            auto it = quotes.begin();
            while(it != quotes.end() && order_quantity > 0){
                int& cur_quote_qty = (*it)->quantity; // COPY
                const double cur_quote_price = (*it)->price;

                cout << "Quotes at level: " << cur_quote_qty << ", Size of order: "<<order_quantity<<"\n";
                if(cur_quote_qty > order_quantity){ // Fully fill our order on this level 
                    cout << "can fully fill here\n";
                    // Update response vars
                    units_transacted += order_quantity;
                    total_value += order_quantity * cur_quote_price;
                    
                    // Fill part of this order and break
                    cur_quote_qty = cur_quote_qty-order_quantity;
                    
                    // Update how many units left to fill
                    order_quantity = 0;

                    break; // Done, order filled
                }else{ // Partially fill our order on this level 
                    cout << "can partial fill here\n";
                    // Update response vars
                    units_transacted += cur_quote_qty;
                    total_value += cur_quote_qty * cur_quote_price;
                    
                    // Update how many units left to fill
                    order_quantity -= cur_quote_qty;

                    // delete order from book, since we lifted it
                    it = quotes.erase(it);
                }
            }
        }
        rit++;
    }
    
    // Remove empty vectors from the maps
    clean_leg(bids);
    clean_leg(asks);

    return std::make_pair(units_transacted, total_value);
};


// Return avg fill price and qty filled
std::pair<int,double> Orderbook::handle_order(OrderType type, int order_quantity, Side side, double price){
	// Analytics to return
	int units_transacted = 0;
	double total_value = 0;

	// market order
	if (type == OrderType::market) {
        std::pair<int,double> fill;
        if (side==Side::sell){
            fill = Orderbook::fill_order(bids, OrderType::market, Side::sell, order_quantity, price, units_transacted, total_value);
        }else if(side==Side::buy){
            fill = Orderbook::fill_order(asks, OrderType::market, Side::buy, order_quantity, price, units_transacted, total_value);
        }
        return fill;
	} else if(type == OrderType::limit) {
		// Analytics 
		int units_transacted = 0;
		double total_value = 0;

		if (side==Side::buy){
			if (best_quote(BookSide::ask) <= price){
				// Can at least partially fill
				tuple<int, double> fill = Orderbook::fill_order(asks, OrderType::limit, Side::buy, order_quantity, price, units_transacted, total_value);
				// Add remaining order to book
				add_order(order_quantity, price, BookSide::bid);
				return fill;
			}else{
				// No fill possible, put on book
				add_order(order_quantity, price, BookSide::bid);
				return std::make_pair(units_transacted, total_value);
			}

		}else{
			if (best_quote(BookSide::bid) >= price){
				// Can at least partially fill
                tuple<int, double> fill= Orderbook::fill_order(bids, OrderType::limit, Side::sell, order_quantity, price, units_transacted, total_value);
				// Add remaining order to book
				add_order(order_quantity, price, BookSide::ask);
				return fill;
			}else{
				// No fill possible, put on book
				add_order(order_quantity, price, BookSide::ask);
				return std::make_pair(units_transacted, total_value);
			}
		}
	}else{
		throw std::runtime_error("Invalid order type encountered");
	}
}

double Orderbook::best_quote(BookSide side){
	if (side == BookSide::bid){
		return std::prev(bids.end())->first;
	}else if (side == BookSide::ask){
		return std::prev(asks.end())->first;
	} else {
		return 0.0;
	}
}

template<typename T>
void Orderbook::print_leg(map<double, vector<unique_ptr<Order>>, T>& hashmap, BookSide side){
	if (side == BookSide::ask){
		for(auto& pair : hashmap){ // Price level
			// Count size at a price level
			int size_sum = 0;
			for (auto& order : pair.second){ // Order level
				size_sum += order->quantity;
			}

			string color = "31";
			cout << "\t\033[1;" << color << "m" << "$" << setw(6) << fixed << setprecision(2) 
                << pair.first << setw(5) << size_sum << "\033[0m ";

			// Make bars to visualize size
			for (int i = 0; i < size_sum/10; i++){
				cout << "█";
			}
			cout << "\n";
		}
	}else if (side == BookSide::bid){
		for(auto pair = hashmap.rbegin(); pair != hashmap.rend(); ++pair) { // Price level
			int size_sum = 0;
			for (auto& order : pair->second){ // Order level
				size_sum += order->quantity;
			}

			string color = "32";
			cout << "\t\033[1;" << color << "m" << "$" << setw(6) << fixed << setprecision(2)
                << pair->first << setw(5) << size_sum << "\033[0m ";

			// Make bars to visualize size
			for (int i = 0; i < size_sum/10; i++){
				cout << "█";
			}
			cout << "\n";
		}
	}
	
}

void Orderbook::print(){
	cout << "========== Orderbook =========" << "\n";
	print_leg(asks, BookSide::ask);

	// print bid-ask spread
	double best_ask = best_quote(BookSide::ask);
	double best_bid = best_quote(BookSide::bid);
	cout  << "\n\033[1;33m" << "======  " << 10000 * (best_ask-best_bid)/best_bid << "bps  ======\033[0m\n\n";

	print_leg(bids, BookSide::bid);
	cout << "==============================\n\n\n";
}
