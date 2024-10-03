#include "../include/helpers.hpp"
#include <fstream>
#include <iostream>
#include <utility>

using std::cout;
using std::cerr;

void print_file_contents(std::string_view file_path){
    // std::string_view.data() returns a const char* (c-style string), which is what ifsteram expects 
    std::ifstream file(file_path.data());

    if(!file){
        cerr << "Unable to open ascii.txt";
    }

    if (file.is_open())
        cout << file.rdbuf();

	cout << "\n";

    file.close();
}

void print_fill(std::pair<int, double> fill, int quantity, u_int64_t start_time, u_int64_t end_time){
    cout << "\033[33mFilled " << fill.first << "/" << quantity << " units @ $" 
        << fill.second/fill.first << " average price. Time taken: " 
        << (end_time-start_time) << " nano seconds\033[0m" << "\n";
}
