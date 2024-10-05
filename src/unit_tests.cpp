#include <iostream>
#include <cassert>
#include "../include/order.hpp"
#include "../include/orderbook.hpp"

// g++ -std=c++17 ./src/orderbook.cpp ./src/unit_tests.cpp -o orderbook_test && ./orderbook_test

// Function to test adding orders to the orderbook
void test_add_order() {
    Orderbook orderbook(false);

    // Add a bid order and an ask order
    orderbook.add_order(100, 100.50, BookSide::bid);
    orderbook.add_order(200, 101.00, BookSide::ask);

    // Use getter functions to access bids and asks
    const auto& bids = orderbook.get_bids();
    const auto& asks = orderbook.get_asks();

    // Check if the bid order was added correctly
    cout << bids.size();
    assert(bids.size() == 1); // Check the number of price levels in bids
    assert(bids.at(100.50).size() == 1); // Check the number of orders at this price level
    assert(bids.at(100.50)[0]->quantity == 100); // Check the order quantity
    assert(bids.at(100.50)[0]->price == 100.50); // Check the order price

    // Check if the ask order was added correctly
    assert(asks.size() == 1); // Check the number of price levels in asks
    assert(asks.at(101.00).size() == 1); // Check the number of orders at this price level
    assert(asks.at(101.00)[0]->quantity == 200); // Check the order quantity
    assert(asks.at(101.00)[0]->price == 101.00); // Check the order price

    std::cout << "test_add_order passed!" << std::endl;
}

// Function to test executing a market order
void test_execute_market_order() {
    Orderbook orderbook(false);

    // Add multiple bid and ask orders to the orderbook
    orderbook.add_order(100, 100.50, BookSide::bid);
    orderbook.add_order(150, 100.50, BookSide::bid);
    orderbook.add_order(200, 101.00, BookSide::ask);
    orderbook.add_order(250, 101.00, BookSide::ask);
    orderbook.print();
    // Execute a market order to sell 200 units
    auto [units_transacted, total_value] = orderbook.handle_order(OrderType::market, 200, Side::sell, 0);

    orderbook.print();
    // Use getter functions to access bids
    const auto& bids = orderbook.get_bids();

    // Check if 200 units were transacted correctly
    assert(units_transacted == 200); // Check that 200 units were filled
    assert(total_value == 100.50 * 200); // Check that the total value is correct

    // Check if the bid order at 100.50 is now reduced to 50 units
    assert(bids.at(100.50).size() == 1); // Ensure there is still one order at 100.50
    assert(bids.at(100.50)[0]->quantity == 50); // Check the remaining quantity of the order

    std::cout << "test_execute_market_order passed!" << std::endl;
}

// Function to test executing a limit order
void test_execute_limit_order() {
    Orderbook orderbook(false);

    // Add multiple bid and ask orders to the orderbook
    orderbook.add_order(100, 100.50, BookSide::bid);
    orderbook.add_order(150, 100.50, BookSide::bid);
    orderbook.add_order(200, 101.00, BookSide::ask);
    orderbook.add_order(250, 101.00, BookSide::ask);

    // Execute a limit order to buy 300 units at 101.00
    auto [units_transacted, total_value] = orderbook.handle_order(OrderType::limit, 300, Side::buy, 101.00);

    // Use getter functions to access asks
    const auto& asks = orderbook.get_asks();

    // Check if 300 units were transacted correctly
    assert(units_transacted == 300); // Check that 300 units were filled
    assert(total_value == 101.00 * 300); // Check that the total value is correct

    // Check if the ask order at 101.00 is now reduced to 150 units
    assert(asks.at(101.00).size() == 1); // Ensure there is still one order at 101.00
    assert(asks.at(101.00)[0]->quantity == 150); // Check the remaining quantity of the order

    std::cout << "test_execute_limit_order passed!" << std::endl;
}

// Function to test the best bid and best ask prices
void test_best_quote() {
    Orderbook orderbook(false);

    // Add a bid and an ask order to the orderbook
    orderbook.add_order(100, 100.50, BookSide::bid);
    orderbook.add_order(200, 101.00, BookSide::ask);

    // Check the best bid and best ask
    double best_bid = orderbook.best_quote(BookSide::bid);
    double best_ask = orderbook.best_quote(BookSide::ask);

    assert(best_bid == 100.50); // Ensure the best bid is correct
    assert(best_ask == 101.00); // Ensure the best ask is correct

    std::cout << "test_best_quote passed!" << std::endl;
}

// Function to test a small market order against multiple ask orders
void test_small_market_order_best_ask() {
    Orderbook orderbook(false);

    // Add three large ask orders to the orderbook
    orderbook.add_order(1000, 101.00, BookSide::ask); // 1000 units at 101.00
    orderbook.add_order(1500, 102.00, BookSide::ask); // 1500 units at 102.00
    orderbook.add_order(2000, 103.00, BookSide::ask); // 2000 units at 103.00

    // Execute a small market order to buy 100 units
    auto [units_transacted, total_value] = orderbook.handle_order(OrderType::market, 100, Side::buy, 0);

    // Use getter function to access asks
    const auto& asks = orderbook.get_asks();

    // Check if 100 units were transacted correctly at the best ask price (101.00)
    assert(units_transacted == 100); // Check that 100 units were filled
    assert(total_value == 101.00 * 100); // Check that the total value is correct

    // Check that only the best ask order (101.00) is affected
    assert(asks.at(101.00)[0]->quantity == 900); // 1000 - 100 = 900 remaining units at 101.00

    // Ensure the other ask orders are not affected
    assert(asks.at(102.00)[0]->quantity == 1500); // No change in quantity
    assert(asks.at(103.00)[0]->quantity == 2000); // No change in quantity

    std::cout << "test_small_market_order_best_ask passed!" << std::endl;
}

// Main function to run all tests
int main() {
    test_add_order();
    test_execute_market_order();
    test_execute_limit_order();
    test_best_quote();
    test_small_market_order_best_ask();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
