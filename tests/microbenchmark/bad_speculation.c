/*
 * Bad Speculation Microbenchmark
 * 
 * This program creates a bad speculation bottleneck by creating
 * hard-to-predict branches based on random data. This will cause
 * frequent branch mispredictions, which leads to pipeline flushes
 * and wasted execution.
 */


#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define NUM_ITERATIONS 100000
#define ARRAY_SIZE 4096

// Volatile to prevent compiler optimizations
volatile int sink = 0;
volatile int array[ARRAY_SIZE];

// Function that performs an operation based on a condition that's hard to predict
// The branch predictor will struggle with the randomness
__attribute__((noinline))
void branch_on_random(int index) {
    if (array[index & (ARRAY_SIZE-1)] > 0) {
        sink += array[index & (ARRAY_SIZE-1)];
    } else {
        sink -= array[index & (ARRAY_SIZE-1)];
    }
}

int main() {
    clock_t start, end;
    double cpu_time_used;
    uint64_t random_state = 0;
    
    // Initialize the array with random values
    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        // Ensure a roughly 50/50 mix of positive and negative values
        array[i] = rand() % 100 - 50;
    }
    
    // Use a linear congruential generator for faster random number generation
    // This makes the branches even harder to predict
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        // Simple LCG for generating pseudo-random numbers quickly
        random_state = random_state * 6364136223846793005ULL + 1442695040888963407ULL;
        int index = (random_state >> 32) % ARRAY_SIZE;
        
        // Call the function with a pseudo-random index
        branch_on_random(index);
        
        // Add some more unpredictable branches to confuse the predictor
        if ((i & 127) == 0) {
            // This branch will be taken rarely and unpredictably
            array[(index + 1) & (ARRAY_SIZE-1)] = -array[(index + 1) & (ARRAY_SIZE-1)];
        }
    }
    
    return 0;
}