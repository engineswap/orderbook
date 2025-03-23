#!/usr/bin/env python3

import matplotlib.pyplot as plt

def read_times(filename):
    """Read one integer per line from a file and return a list of times in nanoseconds."""
    times = []
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if line:
                times.append(int(line))
    return times

def main():
    # Read the data from the text files (times in nanoseconds)
    market_times_ns = read_times("market_times.txt")
    modify_times_ns = read_times("modify_times.txt")
    delete_times_ns = read_times("delete_times.txt")
    
    # Convert nanoseconds to microseconds
    market_times = [t / 1000.0 for t in market_times_ns]
    modify_times = [t / 1000.0 for t in modify_times_ns]
    delete_times = [t / 1000.0 for t in delete_times_ns]
    
    # Create a figure with 3 subplots in a row
    fig, axs = plt.subplots(1, 3, figsize=(18, 5))
    
    bins = 100 # number of histogram bins
    
    # Plot market orders histogram
    axs[0].hist(market_times, bins=bins, color='blue', alpha=0.7)
    axs[0].set_title("Market Orders (μs)")
    axs[0].set_xlabel("Time (μs)")
    axs[0].set_ylabel("Frequency")
    
    # Plot modify orders histogram
    axs[1].hist(modify_times, bins=bins, color='green', alpha=0.7)
    axs[1].set_title("Modify Orders (μs)")
    axs[1].set_xlabel("Time (μs)")
    axs[1].set_ylabel("Frequency")
    
    # Plot delete orders histogram
    axs[2].hist(delete_times, bins=bins, color='red', alpha=0.7)
    axs[2].set_title("Delete Orders (μs)")
    axs[2].set_xlabel("Time (μs)")
    axs[2].set_ylabel("Frequency")
    
    # Adjust layout to prevent overlapping elements
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()
