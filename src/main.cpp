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

// Run command: g++ -std=c++14 ./src/main.cpp ./src/order.cpp ./src/orderbook.cpp -o main
#include <iostream>
#include "../include/orderbook.hpp"
#include "../include/helpers.hpp"

using std::cout;
using std::cin;

int main(){
    print_file_contents("./assets/ascii.txt");

	Orderbook ob(true);

	while(true){
		int action;
		cout << "Options\n————————————————————\n|1. Print Orderbook|\n|2. Submit order   |\n ————————————————————\nChoice: ";
		cin >> action;
		cout << "\n";

		if (action == 1){
			ob.print();
		}else if(action == 2){
			// User inputs
			int order_type_input; 
			int quantity;
			int side_input; 
			double price; 

			cout << "Enter order type:\n0. Market order\n1. Limit order\nSelection: ";
			cin >> order_type_input;
			OrderType order_type = static_cast<OrderType>(order_type_input);

			cout << "\nEnter side:\n0. Buy\n1. Sell\nSelection: ";
			cin >> side_input;
			Side side = static_cast<Side>(side_input);

			cout << "\nEnter order quantity: ";
			cin >> quantity;

			if(order_type == OrderType::market){
				cout << "\nSubmitting market " << ((side == Side::buy) ? "buy":"sell") 
                    << " order for " << quantity << " units.." << "\n";
				
				u_int64_t start_time = unix_time();
                std::pair<int,double> fill = ob.handle_order(order_type, quantity, side);
				u_int64_t end_time = unix_time();
			    
                print_fill(fill, quantity, start_time, end_time);
			}else if(order_type == OrderType::limit){
				cout << "\nEnter limit price: ";
				cin >> price;

				cout << "\nSubmitting limit " << ((side == Side::buy) ? "buy":"sell") 
                    << " order for " << quantity << " units @ $" << price << ".." << "\n";

				u_int64_t start_time = unix_time();
                std::pair<int,double> fill = ob.handle_order(order_type, quantity, side, price);
				u_int64_t end_time = unix_time();

                print_fill(fill, quantity, start_time, end_time);
			}
			cout << "\n";
		}
	}
}
