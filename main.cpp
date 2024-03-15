#include <iostream>
#include <vector>
#include <chrono>
#include <stdlib.h> 
#include <map>
#include <thread>
#include <iomanip>
#include "orderbook.hpp"


// Run command: g++ -std=c++11 main.cpp -o main && ./main

using namespace std;

// Return unix time (ns)
uint64_t unix_time() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

/*
class Order{
	int quantity;
	double price;
	uint64_t timestamp = unix_time(); // Set timestamp of order

	public:
	// Constructor
	Order(int quantity_, double price_){
		quantity = quantity_;
		price = price_;
	}

	// Getters
	int get_quantity() { return quantity; }
	double get_price() { return price; }
	time_t get_timestamp() { return timestamp; }
};
*/


class Orderbook{
	typedef tuple<uint64_t, double, int> Order;
	map<double, vector<Order>, std::greater<double>> bids; // timestamp, Price, Qty
	map<double, vector<Order>, std::greater<double>> asks; // timestamp, Price, Qty

	public:
	Orderbook(){
		// seed RNG
		srand (time(NULL)); 

		// Add some dummy orders
		for (int i=0; i<5; i++){ 
			double random_price = 90.0 + (rand() % 1001) / 100.0;
			int random_qty = rand() % 50 + 1; 
			int random_qty2 = rand() % 50 + 1; 

			Order tmp = make_tuple(unix_time(), random_price, random_qty);
			this_thread::sleep_for(chrono::milliseconds(1)); // Sleep for a millisecond so the orders have different timestamps
			Order tmp2 = make_tuple(unix_time(), random_price, random_qty2);
			
			bids[random_price].push_back(tmp);
			bids[random_price].push_back(tmp2);
		}
		
		for (int i=0; i<5; i++){
			double random_price = 100.0 + (rand() % 1001) / 100.0;
			int random_qty = rand() % 50 + 1; 
			int random_qty2 = rand() % 50 + 1; 

			auto tmp = make_tuple(unix_time(), random_price, random_qty);
			this_thread::sleep_for(chrono::milliseconds(1)); // Sleep for a millisecond so the orders have different timestamps
			Order tmp2 = make_tuple(unix_time(), random_price, random_qty2);
			
			asks[random_price].push_back(tmp);
			asks[random_price].push_back(tmp2);
		}
	}

	void remove_empty_keys(map<double, vector<tuple<uint64_t, double, int>>, std::greater<double>> &map){
		// Remove all empty keys from 
		for (auto it = map.begin(); it != map.end(); ) {
			if (it->second.empty()) { // Check if the vector associated with the current key is empty
				it = map.erase(it);  // Erase the key and update the iterator to the next element
			} else {
				++it; // Move to the next element
			}
		}
	}

	// Return avg fill price and qty
	void execute_order(int type, int order_quantity, int side, double price = 0){
		// market order
		if (type == 1){
			auto& offers = (side==1) ? asks : bids;
			for(auto rit = offers.rbegin(); rit != offers.rend();) {
				auto& pair = *rit; // Dereference the reverse iterator to get the key-value pair
				
				auto& quotes = pair.second;
				// sort(quotes.begin(), quotes.end()); // TODO: Sort quotes in ascending order by timestamp

				// lift/hit as many levels as needed until qty filled
				auto it = quotes.begin();
				while(it != quotes.end() && order_quantity > 0){
					int& quote_qty = get<2>(*it);
					if(quote_qty > order_quantity){
						// Fill part of this order and break
						quote_qty -= order_quantity;
						order_quantity = 0;
						break;
					}else{
						// delete order from book and on
						order_quantity -= quote_qty;
						it = quotes.erase(it);
					}
				}
				rit++;
			}

			// Clean up quotes
			remove_empty_keys(offers);
			
		} else {
			cout << "hehe i didnt do that yet silly hehehoho" << endl;
			cout << "ur mom gae" << endl;
		}
	}

	void print_leg(const map<double, vector<Order>, std::greater<double>>& hashmap, int type){
		for(const auto& pair : hashmap){
			int size_sum = 0;
			for (auto& order : pair.second){
				size_sum += get<2>(order);
			}
			string color = (type==0)?"32":"31";
			cout << "\t\033[1;" << color << "m" << "$" << fixed << setprecision(2) << pair.first << " - " << size_sum << "\033[0m ";

			// Make bars to visualize size
			for (int i = 0; i < size_sum/10; i++){
				cout << "█";
			}
			cout << endl;
		}
	}

	void print(){
		cout << "======== L2 Orderbook ========" << endl;
		print_leg(asks, 1);

		// print bid-ask spread
		double best_ask = std::prev(asks.end())->first;
		double best_bid = bids.begin()->first;
		cout  << "\n\033[1;33m" << "======  " << 10000*(best_ask-best_bid)/best_bid << "bps  ======\033[0m\n\n";

		print_leg(bids, 0);
		cout << "==============================" << endl;
	}
};

enum Side {buy = 1, sell = 2};
enum OrderType {market = 1, limit = 2};

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

	cout << "Enter side:\n1. Buy\n2. Sell\nSelection: ";
	cin >> side_input;
	Side side = static_cast<Side>(side_input);

	cout << "Enter order quantity: ";
	cin >> quantity;

	if(order_type == market){
		cout << "Submitting market " << ((side == buy) ? "buy":"sell") << " order for " << quantity << " units" << endl;
		ob.execute_order(order_type, quantity, side);
	}else if(order_type == limit){
		cout << "Enter limit price: ";
		cin >> price;

		cout << "Submitting limit " << ((side == buy) ? "buy":"sell") << " order for " << quantity << " units @ $" << price << endl;
		ob.execute_order(order_type, quantity, side, price);
	}
	
	ob.print();
}