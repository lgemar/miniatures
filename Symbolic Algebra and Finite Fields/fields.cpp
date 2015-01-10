#include "Field.hpp"


struct add_modN {
	int N;
	add_modN(int N) : N(N) {}
	int operator()(int a, int b) {
		return (a + b) % N;
	}
};

struct mult_modN {
	int N;
	mult_modN(int N) : N(N) {}
	int operator()(int a, int b) {
		return (a * b) % N;
	}
};

typedef FiniteField<int,add_modN,mult_modN> int_field;
typedef int_field::element element;

int main() {
	int N = 5;

	// Build the field of integers modulo N
	std::vector<int> field_elements;
	for(int i = 0; i < N; ++i) {
		field_elements.push_back(i);
	}

	// Build the field of integers modulo 5
	int_field F = int_field(field_elements.begin(), 
			  field_elements.end(), 
			  add_modN(5), 
			  mult_modN(5), 
			  true);

	// Test out the integer fields
	element one = F.build_element(1);
	element three = F.build_element(3);

	element a = one + three;
	element b = one * three;
	std::cout << "one plus three: " << a.val << std::endl;
	std::cout << "one times three: " << b.val << std::endl;
	return 0;
}
