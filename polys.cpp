#include "polys.hpp"

int main(int argc, char** argv) {
	// Get the two polynomials from the command line
	if( argc != 3 ) {
		error("USAGE: ./polys <poly1> <poly2>");
	}
	std::stringstream ss1;
	std::stringstream ss2;
	ss1 << argv[1];
	ss2 << argv[2];
	std::string s1 = ss1.str();
	std::string s2 = ss2.str();

	// Add some test values
	Polynomial p1 (s1);
	db("a: ");
	p1.print();
	Polynomial p2 (s2);
	db("b: ");
	p2.print();
	
	// Computations
	Polynomial p3 = p1 + p2;
	Polynomial p4 = p1 * p2;

	// Print the result1
	db("Sum: ");
	p3.print();
	db("Product: ");
	p4.print();
}
