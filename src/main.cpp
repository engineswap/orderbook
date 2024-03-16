/**
 * @file main.cpp
 * @brief This file contains the main function that runs the order book program.
 * 
 * The program reads the contents of a file named "ascii.txt" and prints it to the console.
 * It then prompts the user to choose an action: print the order book or submit an order.
 * If the user chooses to submit an order, they are prompted to enter the order type, side, quantity, and price (for limit orders).
 * The program then executes the order and prints the result.
 * The process continues until the user chooses to exit the program.
 */

// Run command: g++ -std=c++14 main.cpp order.cpp -o main && ./main
#include <iostream>
#include <fstream>

#include "../include/order.hpp"
#include "../include/orderbook.hpp"
#include "../include/helpers.hpp"

using namespace std;

int main(){
	// Read ascii.txt and print it
	std::ifstream f("./assets/ascii.txt");

    if (f.is_open())
        std::cout << f.rdbuf();

	cout << endl;

	Orderbook ob;

	while(true){
		int action;
		cout << "Options\n————————————————————\n|1. Print Orderbook|\n|2. Submit order   |\n ————————————————————\nChoice: ";
		cin >> action;
		cout << endl;

		if (action == 1){
			ob.print();
		}else if(action == 2){
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
				cout << "\nSubmitting market " << ((side == buy) ? "buy":"sell") << " order for " << quantity << " units.." << endl;
				
				u_int64_t start_time = unix_time();
				tuple<int,double> fill = ob.execute_order(order_type, quantity, side);
				u_int64_t end_time = unix_time();
				
				cout << "\033[33mFilled " << get<0>(fill) << "/" << quantity << " units @ $" << get<1>(fill)/get<0>(fill) << " average price. Time taken: " << (end_time-start_time) << " nano seconds\033[0m" << endl;
			}else if(order_type == limit){
				cout << "\nEnter limit price: ";
				cin >> price;

				cout << "\nSubmitting limit " << ((side == buy) ? "buy":"sell") << " order for " << quantity << " units @ $" << price << ".." << endl;

				u_int64_t start_time = unix_time();
				tuple<int,double> fill = ob.execute_order(order_type, quantity, side, price);
				u_int64_t end_time = unix_time();

				//come up with a new, clean order confimation message which incdes how many units filled as a fraction, execution time, avg fill price, side (bought or sold)
				cout << "\033[33mFilled " << get<0>(fill) << "/" << quantity << " units @ $" << get<1>(fill)/get<0>(fill) << " average price. Time taken: " << (end_time-start_time) << " nano seconds\033[0m" << endl;
			}
			cout << endl;
		}
	}
}