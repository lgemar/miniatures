/** 
 * lukasgemar@gmail.com
 * 
 * Takes an integer and finds all the prime numbers less than or equal
 * to that integer
 *
 */

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int gcd(int a, int b) {
	// Swap the two so that a > b
	if( b > a ) {
		int temp = a;
		a = b;
		b = temp;
	}

	// Recursively compute the result
	if( b == 0 ) {
		return a;
	}
	else {
		return gcd(b, a % b);
	}
}

int main(int argc, char** argv) {

	// Check input validity
	if( argc != 2 ) {
		std::cerr << "USAGE: primes <q>" << std::endl;
		return 1;
	}

	// Convert the input integer to a decimal
	int q = atoi(argv[1]);

	std::vector<int> coprimes;
	for(int i = 1; i < q; ++i) {
		if( gcd(q, i) == 1 )
			coprimes.push_back(i);
	}

	// The number of elements in the group
	int group_size = coprimes.size();

	// Declare the group table
	int group_table[group_size][group_size];

	// Build the group table with multiplication facts
	for(int i = 0; i < group_size; ++i) {
		for(int j = 0; j < group_size; ++j) {
			group_table[i][j] = (coprimes[i] * coprimes[j]) % q;
		}
	}

	// Print out the group table
	printf("Group table: \n");
	for(int i = 0; i < group_size; ++i) {
		for(int j = 0; j < group_size; ++j) {
			printf("%3d ", group_table[i][j]);
		}
		printf("\n");
	}

	// Determine the order of each element in the group
	// Row i in order contains all the group elements of order i
	std::vector< std::vector<int> > element_orders (group_size + 1);
	for(int i = 0; i < group_size; ++i) {
		int e = coprimes[i];
		int tot = e;
		int order = 1;
		while( tot != 1 ) {
			tot = (e * tot) % q;
			++order;
			assert( e != 0 );
		}
		element_orders[order].push_back(coprimes[i]);
	}

	// Print out all of the elements of various orders
	for(int i = 1; i <= group_size; ++i) {
		int num_elements = element_orders[i].size();
		if( num_elements > 0 )
			printf("Elements of order %d:", i);
		for(int j = 0; j < num_elements; ++j) {
			printf(" %d", element_orders[i][j]);
		}
		if( num_elements > 0 )
			printf("\n");
	}
	return 0;
}


