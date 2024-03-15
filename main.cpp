#include <iostream>
#include <vector>
#include <chrono>
#include <stdlib.h> 
#include <map>
#include <thread>
#include <iomanip>
#include <memory> 
#include "orderbook.hpp"


// Run command: g++ -std=c++14 main.cpp -o main && ./main

using namespace std;

// Return unix time (ns)
uint64_t unix_time() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

enum Side {buy = 1, sell = 2};
enum OrderType {market = 1, limit = 2};

class Order{
	int quantity;
	double price;
	Side side;
	uint64_t timestamp = unix_time(); // Set timestamp of order

	public:
	// Constructor
	Order(int quantity_, double price_, Side side_){
		quantity = quantity_;
		price = price_;
		side = side_;
	}

	// Setters
	void set_quantity(int new_qty){
		quantity = new_qty;
	}

	// Getters
	int get_quantity() { return quantity; }
	double get_price() { return price; }
	time_t get_timestamp() { return timestamp; }
};



class Orderbook{
	map<double, vector<unique_ptr<Order>>, less<double>> bids; // timestamp, Price, Qty
	map<double, vector<unique_ptr<Order>>, greater<double>> asks; // timestamp, Price, Qty

	public:
	Orderbook(){
		// seed RNG
		srand (time(NULL)); 

		// Add some dummy orders
		for (int i=0; i<5; i++){ 
			double random_price = 90.0 + (rand() % 1001) / 100.0;
			int random_qty = rand() % 50 + 1; 
			int random_qty2 = rand() % 50 + 1;
			
			bids[random_price].push_back(make_unique<Order>(random_qty, random_price, Side::buy));
			this_thread::sleep_for(chrono::milliseconds(1)); // Sleep for a millisecond so the orders have different timestamps
			bids[random_price].push_back(make_unique<Order>(random_qty2, random_price, Side::buy));
		}
		for (int i=0; i<5; i++){
			double random_price = 100.0 + (rand() % 1001) / 100.0;
			int random_qty = rand() % 50 + 1; 
			int random_qty2 = rand() % 50 + 1; 			
			
			asks[random_price].push_back(make_unique<Order>(random_qty, random_price, Side::sell));
			this_thread::sleep_for(chrono::milliseconds(1)); // Sleep for a millisecond so the orders have different timestamps
			asks[random_price].push_back(make_unique<Order>(random_qty2, random_price, Side::sell));
		}
	}

	template<typename T>
	void clean_leg(map<double, vector<unique_ptr<Order>>, T>& map){
		for (auto it = map.begin(); it != map.end(); ) {
			if (it->second.empty()) { // Check if the vector associated with the current key is empty
				it = map.erase(it);  // Erase the key and update the iterator to the next element
			} else {
				++it; // Move to the next element
			}
		}
	}

	void remove_empty_keys(){
		// Remove all empty keys from map
		clean_leg(bids);
		clean_leg(asks);
	}

	// Return avg fill price and qty
	tuple<int,double> execute_order(OrderType type, int order_quantity, Side side, double price = 0){
		// market order
		if (type == market) {
			// Analytics 
			int units_transacted = 0;
			double total_value = 0;

			auto process = [&] (auto& offers) {
				for(auto rit = offers.rbegin(); rit != offers.rend();) {
					auto& pair = *rit; // Dereference the reverse iterator to get the key-value pair

					// cout << "Price level: " << pair.first << endl;
					
					auto& quotes = pair.second; // Vect of objects
					// sort(quotes.begin(), quotes.end()); // TODO: Sort quotes in ascending order by timestamp

					// lift/hit as many levels as needed until qty filled
					auto it = quotes.begin();
					while(it != quotes.end() && order_quantity > 0){
						int quote_qty = (*it)->get_quantity();
						if(quote_qty > order_quantity){
							// cout << "Filling part of order"<<endl;

							units_transacted += order_quantity;
							total_value += order_quantity * pair.first;

							// Fill part of this order and break
							(*it)->set_quantity(quote_qty-order_quantity);
							quote_qty -= order_quantity;
							order_quantity = 0;
							break;
						}else{
							// cout << "Filling entire order"<<endl;
							
							units_transacted += quote_qty;
							total_value += quote_qty * pair.first;
							
							// delete order from book and on
							order_quantity -= quote_qty;
							it = quotes.erase(it);
						}
					}
					rit++;
				}
				remove_empty_keys();
				return make_tuple(units_transacted, total_value);
			};

			return (side == sell) ? process(bids) : process(asks);

		} else if(type == limit) {
			cout << "hehe i didnt do that yet silly hehehoho" << endl;
			cout << "ur mom gae" << endl;
		}
	}

	double best_quote(Side side){
		if (side == buy){
			return std::prev(bids.end())->first;
		}else if (side == sell){
			return std::prev(asks.end())->first;
		} else {
			return 0.0;
		}
	}

	template<typename T>
	void print_leg(map<double, vector<unique_ptr<Order>>, T>& hashmap, Side side){
		if (side == sell){
			for(auto& pair : hashmap){ // Price level
				// Count size at a price level
				int size_sum = 0;
				for (auto& order : pair.second){ // Order level
					size_sum += order->get_quantity();
				}

				string color = (side==buy)?"32":"31";
				cout << "\t\033[1;" << color << "m" << "$" << fixed << setprecision(2) << pair.first << " - " << size_sum << "\033[0m ";

				// Make bars to visualize size
				for (int i = 0; i < size_sum/10; i++){
					cout << "█";
				}
				cout << endl;
			}
		}else if (side == buy){
			for(auto pair = hashmap.rbegin(); pair != hashmap.rend(); ++pair) { // Price level
				int size_sum = 0;
				for (auto& order : pair->second){ // Order level
					size_sum += order->get_quantity();
				}

				string color = (side==buy)?"32":"31";
				cout << "\t\033[1;" << color << "m" << "$" << fixed << setprecision(2) << pair->first << " - " << size_sum << "\033[0m ";

				// Make bars to visualize size
				for (int i = 0; i < size_sum/10; i++){
					cout << "█";
				}
				cout << endl;
			}
		}
		
	}

	void print(){
		cout << "======== L2 Orderbook ========" << endl;
		print_leg(asks, Side::sell);

		// print bid-ask spread
		double best_ask = best_quote(Side::sell);
		double best_bid = best_quote(Side::buy);
		cout  << "\n\033[1;33m" << "======  " << 10000 * (best_ask-best_bid)/best_bid << "bps  ======\033[0m\n\n";

		print_leg(bids, Side::buy);
		cout << "==============================" << endl;
	}
};

int main(){
	Orderbook ob;
	ob.print();

	// User inputs
	int order_type_input; 
	int quantity;
	int side_input; 
	double price; 

	cout << "Enter order type:\n1. Market order\n2. Limit order\nSelection: ";
	cin >> order_type_input;
	OrderType order_type = static_cast<OrderType>(order_type_input);

	cout << "\nEnter side:\n1. Buy\n2. Sell\nSelection: ";
	cin >> side_input;
	Side side = static_cast<Side>(side_input);

	cout << "\nEnter order quantity: ";
	cin >> quantity;

	if(order_type == market){
		cout << "\nSubmitting market " << ((side == buy) ? "buy":"sell") << " order for " << quantity << " units" << endl;
		tuple<int,double> fill = ob.execute_order(order_type, quantity, side);
		cout << "Filled " << get<0>(fill) << " units @ $" << get<1>(fill)/get<0>(fill) << " average price" << endl;
	}else if(order_type == limit){
		cout << "\nEnter limit price: ";
		cin >> price;

		cout << "\nSubmitting limit " << ((side == buy) ? "buy":"sell") << " order for " << quantity << " units @ $" << price << endl;
		ob.execute_order(order_type, quantity, side, price);
	}
	
	ob.print();
}