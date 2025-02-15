# C Data Structures Library

Welcome to my **C Data Structures Library**! This repository offers a collection of efficient and thread-safe data structures implemented in C, adhering to the [Barr Group's Embedded C Coding Standard](https://barrgroup.com/embedded-systems/books/embedded-c-coding-standard) for maintainable and portable code.

## Features

- **Thread-Safe Implementations**: Most data structures are designed with internal synchronization mechanisms to ensure safe operations across multiple threads.
- **Opaque Typing for Portability**: Structures use **opaque pointers (void*)**, allowing **generic data storage** while keeping the internal representation hidden. This ensures **modularity** and **type flexibility** across applications.
- **Barr-C Coding Style**: Adheres to **industry-standard C coding practices**, improving **code maintainability, readability, and safety**.
- **Efficient Memory Management**: Optimized dynamic memory allocation and deallocation with minimal overhead.

## Available Data Structures

- **Linked Lists**: Singly and circularly linked lists with dynamic memory management.
- **Stacks and Queues**: LIFO and FIFO structures for various use cases.
- **Hash Tables**: Efficient key-value storage with collision handling.
- **Binary Trees**: Balanced trees for quick search, insertion, and deletion.

## Usage Example: Thread-Safe Hash Table

The following example demonstrates how to create a hash table, store key-value pairs, retrieve values, and clean up memory. The opaque-typed design allows the hash table to store any data type.

```c
#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[50];
} UserData;

int main() {
    hashtable_t *ht = hashtable_create(16); // Create a hash table with 16 buckets

    // Create user data entries
    UserData *user1 = malloc(sizeof(UserData));
    user1->id = 1;
    strcpy(user1->name, "Alice");

    UserData *user2 = malloc(sizeof(UserData));
    user2->id = 2;
    strcpy(user2->name, "Bob");

    // Insert data into the hash table
    hashtable_insert(ht, "user1", user1);
    hashtable_insert(ht, "user2", user2);

    // Retrieve data from the hash table
    UserData *retrieved = (UserData *)hashtable_get(ht, "user1");
    if (retrieved) {
        printf("Retrieved: ID=%d, Name=%s\n", retrieved->id, retrieved->name);
    }

    // Remove an entry
    hashtable_remove(ht, "user2");

    // Free memory and destroy the hash table
    free(user1);
    free(user2);
    hashtable_destroy(ht);

    return 0;
}
```

### Notes:

The hash table automatically resizes to balance performance and memory efficiency: 
- Expands dynamically when the load factor exceeds a predefined threshold, improving lookup efficiency.
- Shrinks when underutilized, releasing unused memory and optimizing space usage.
- Minimizes collisions by reallocating and rehashing entries, maintaining fast search performance.
