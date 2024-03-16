# Orderbook in C++

This C++ project simulates a basic orderbook, a key component in financial trading systems, managing buy and sell orders with a FIFO order matching algorithm. It supports both market and limit orders, handles partial and full fills, and visually represents the state of the orderbook. 


![Screenshot 1](./screenshots/ss1.png)
***
## Features

* FIFO queue matching algorithm
* Visualization as seen above
* Accepts Market & Limit orders
* Whole and partial fills
* Pre-populates the orderbook with random orders

***
## Demo Video
You can watch a demo of the project in action [here](https://www.youtube.com/watch?v=1JoNpImW0TQ).
[![Demo video](https://img.youtube.com/vi/1JoNpImW0TQ/0.jpg)](https://www.youtube.com/watch?v=1JoNpImW0TQ)
***

## Design

The project is designed using Object-Oriented Programming (OOP) principles. It is divided into three main parts:

- `main.cpp`: This is where user interaction is handled. Users can place market or limit orders and the program will process them accordingly.
- `order.cpp`: This file contains the `Order` class, which represents an order. Each order has properties like price, quantity, and type (market or limit).
- `orderbook.cpp`: This file contains the `Orderbook` class, which manages order objects. It uses a FIFO queue to ensure that orders are processed in the order they are received. It also has logic to execute incoming orders against the book. And finally it has logic to visualize the book.

***

## How to Run

To compile and run the program, follow these steps:

1. Clone the repo: ``https://github.com/engineswap/cpp-orderbook.git``
2. ``cd cpp-orderbook``
3. Compile the program using the command `g++ -std=c++14 ./src/main.cpp ./src/order.cpp ./src/orderbook.cpp -o main`.
4. Run the program `./main`.

***

## Screenshots

Here are some screenshots of the project in action:

![Screenshot 1](./screenshots/ss3.png)
This limit order was partially filled, then rest (68 units) went on the book.

![Screenshot 2](./screenshots/ss2.png)
This market order was fully filled.

![Screenshot 3](./screenshots/ss4.png)
This is the order book visualized. 
***