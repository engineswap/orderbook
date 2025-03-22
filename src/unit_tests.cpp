#include <iostream>
#include <cassert>
#include "../include/order.hpp"
#include "../include/helpers.hpp"
#include "../include/orderbook.hpp"

using namespace std;

// Function to test adding orders to the orderbook
void test_add_order() {
    Orderbook orderbook(false);

    // Add one bid order and one ask order
    orderbook.add_order(100, 100.50, BookSide::bid);
    orderbook.add_order(200, 101.00, BookSide::ask);

    // Use getter functions to access bids and asks
    const auto& bids = orderbook.get_bids();
    const auto& asks = orderbook.get_asks();

    // Check if the bid order was added correctly
    assert(bids.size() == 1);                   // Only one price level in bids
    assert(bids.at(100.50).size() == 1);          // One order at price 100.50
    assert(bids.at(100.50)[0]->quantity == 100);  // Order quantity is 100
    assert(bids.at(100.50)[0]->price == 100.50);   // Order price is 100.50

    // Check if the ask order was added correctly
    assert(asks.size() == 1);                   // Only one price level in asks
    assert(asks.at(101.00).size() == 1);          // One order at price 101.00
    assert(asks.at(101.00)[0]->quantity == 200);  // Order quantity is 200
    assert(asks.at(101.00)[0]->price == 101.00);   // Order price is 101.00

    cout << "test_add_order passed!" << endl;
}

// Function to test executing a market order
void test_execute_market_order() {
    Orderbook orderbook(false);

    // Add multiple bid orders
    orderbook.add_order(100, 100.50, BookSide::bid);
    orderbook.add_order(150, 100.50, BookSide::bid);
    // Add multiple ask orders (though market sell order works against bids)
    orderbook.add_order(200, 101.00, BookSide::ask);
    orderbook.add_order(250, 101.00, BookSide::ask);

    // Execute a market order to sell 200 units (should fill against bids)
    auto [units_transacted, total_value] = orderbook.handle_order(OrderType::market, 200, Side::sell, 0);

    const auto& bids = orderbook.get_bids();
    // Expect 200 units filled at 100.50 price
    assert(units_transacted == 200);
    assert(total_value == 100.50 * 200);

    // After filling, the bid orders at 100.50 should be reduced:
    // Initially, there were two orders: one with 100 and one with 150 (total 250).
    // Filling 200 units should remove the first 100 completely and reduce the second from 150 to 50.
    assert(bids.at(100.50).size() == 1);
    assert(bids.at(100.50)[0]->quantity == 50);

    cout << "test_execute_market_order passed!" << endl;
}

// Function to test executing a limit order
void test_execute_limit_order() {
    Orderbook orderbook(false);

    // Add multiple bid and ask orders
    orderbook.add_order(100, 100.50, BookSide::bid);
    orderbook.add_order(150, 100.50, BookSide::bid);
    orderbook.add_order(200, 101.00, BookSide::ask);
    orderbook.add_order(250, 101.00, BookSide::ask);

    // Execute a limit order to buy 300 units at 101.00
    auto [units_transacted, total_value] = orderbook.handle_order(OrderType::limit, 300, Side::buy, 101.00);

    const auto& asks = orderbook.get_asks();
    // Expect 300 units filled at 101.00 price level
    assert(units_transacted == 300);
    assert(total_value == 101.00 * 300);

    // Initially there were two ask orders at 101.00 (200 and 250 = 450).
    // Filling 300 should remove the 200-unit order entirely and reduce the 250-unit order to 150.
    assert(asks.at(101.00).size() == 1);
    assert(asks.at(101.00)[0]->quantity == 150);

    cout << "test_execute_limit_order passed!" << endl;
}

// Function to test the best bid and best ask prices
void test_best_quote() {
    Orderbook orderbook(false);

    orderbook.add_order(100, 100.50, BookSide::bid);
    orderbook.add_order(200, 101.00, BookSide::ask);

    double best_bid = orderbook.best_quote(BookSide::bid);
    double best_ask = orderbook.best_quote(BookSide::ask);

    assert(best_bid == 100.50);
    assert(best_ask == 101.00);

    cout << "test_best_quote passed!" << endl;
}

// Function to test a small market order against multiple ask orders
void test_small_market_order_best_ask() {
    Orderbook orderbook(false);

    // Add three ask orders at different prices
    orderbook.add_order(1000, 101.00, BookSide::ask); // Best ask
    orderbook.add_order(1500, 102.00, BookSide::ask);
    orderbook.add_order(2000, 103.00, BookSide::ask);

    // Execute a market order to buy 100 units (should fill at best ask: 101.00)
    auto [units_transacted, total_value] = orderbook.handle_order(OrderType::market, 100, Side::buy, 0);

    const auto& asks = orderbook.get_asks();
    assert(units_transacted == 100);
    assert(total_value == 101.00 * 100);

    // The best ask at 101.00 should be reduced from 1000 to 900
    assert(asks.at(101.00)[0]->quantity == 900);
    // The orders at higher price levels should remain unchanged.
    assert(asks.at(102.00)[0]->quantity == 1500);
    assert(asks.at(103.00)[0]->quantity == 2000);

    cout << "test_small_market_order_best_ask passed!" << endl;
}

void test_modify_and_delete_order() {
    Orderbook orderbook(false);

    // Add an order. We'll capture its ID so we can modify/delete it.
    orderbook.add_order(100, 100.50, BookSide::bid);

    // Retrieve the bids map and extract the first (and only) order at 100.50
    const auto& bids = orderbook.get_bids();
    assert(!bids.empty());
    assert(bids.at(100.50).size() == 1);

    // Capture the ID of this order
    uint64_t orderId = bids.at(100.50)[0]->id;

    // ==========================
    // Time the modify_order call
    // ==========================
    uint64_t start_modify = unix_time();
    bool modified = orderbook.modify_order(orderId, 999);
    uint64_t end_modify = unix_time();

    // Confirm modify worked
    assert(modified && "modify_order should return true for a valid ID");
    assert(bids.at(100.50)[0]->quantity == 999);

    // Print how long modify_order took
    cout << "modify_order took: " << (end_modify - start_modify) 
         << " ns" << endl;

    // ==========================
    // Time the delete_order call
    // ==========================
    uint64_t start_delete = unix_time();
    bool deleted = orderbook.delete_order(orderId);
    uint64_t end_delete = unix_time();

    // Confirm delete worked
    assert(deleted && "delete_order should return true for a valid ID");

    // Verify that the order is gone
    if (bids.find(100.50) != bids.end()) {
        assert(bids.at(100.50).empty());
    }

    // Print how long delete_order took
    cout << "delete_order took: " << (end_delete - start_delete)
         << " ns" << endl;

    cout << "test_modify_and_delete_order passed!" << endl;
}

// Main function to run all tests
int main() {
    test_add_order();
    test_execute_market_order();
    test_execute_limit_order();
    test_best_quote();
    test_small_market_order_best_ask();
    test_modify_and_delete_order();

    cout << "All tests passed!" << endl;
    return 0;
}
