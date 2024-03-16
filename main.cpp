#include "order.hpp"
#include "orderbook.hpp"

// Run command: g++ -std=c++14 main.cpp order.cpp -o main && ./main

using namespace std;

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
		tuple<int,double> fill = ob.execute_order(order_type, quantity, side, price);
		cout << "Immediately filled " << get<0>(fill) << "/" << quantity << " units. The rest went on the book." << endl;
	}
	
	ob.print();
}