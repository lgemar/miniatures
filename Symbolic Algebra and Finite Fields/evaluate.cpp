#include <iostream>
#include <stdio.h>
#include "Field.hpp"
#include "polys.hpp"
#include <sstream>
#include <ctype.h>

typedef ExpressionTree e_tree;
typedef std::string string;

#define DEBUG false

typedef struct coeff_modulo {
	int mod;
	coeff_modulo() : mod(1) {}
	coeff_modulo(int mod) : mod(mod) {}
	coeff_array operator()(coeff_array c) {
		coeff_array result;
		for(int i = 0; i < c.size(); ++i) {
			result.push_back(c[i] % mod);
		}
		return result;
	}
} coeff_modulo;

typedef struct coeff_power_swap {
	int pow; 
	coeff_array sub;
	coeff_power_swap() : pow(0), sub(coeff_array()) {}
	coeff_power_swap(int power, coeff_array replacement) : 
				pow(power), sub(replacement) { 
	}
	coeff_array operator()(coeff_array c) {
		if( c.size() > pow ) {
			coeff_array result = c;
			if( c[pow] ) {
				result = coeff_scalar_multiply(sub, c[pow]);
				c[pow] = 0;
				result = coeff_add(result, c);
			}
			return result;
		}
		else {
			return c;
		}
	}
} coeff_power_swap;

typedef struct poly_adder {
	coeff_modulo simplifier;
	poly_adder(int max_coeff) {
		simplifier = coeff_modulo(max_coeff);
	}

	poly_adder(coeff_modulo c) : simplifier(c) {}

	string operator()(string s1, string s2) {
		//db("s1 + ", s1);
		//db("s2 + ", s2);
		// Add the polynomials
		Polynomial p1 = Polynomial(s1);
		Polynomial p2 = Polynomial(s2);
		Polynomial result = p1 + p2;
		// Simplify the result with the simplifier rules
		result.simplify( simplifier );
		//db(" = ", result.expression() );
		return result.expression();
	}
} poly_adder;

typedef struct poly_multiplier {
	coeff_modulo simplifier;
	std::vector< coeff_power_swap > reducers;

	poly_multiplier(coeff_modulo s, coeff_power_swap r) : 
			simplifier(s) {
			reducers.push_back(r);
	}

	void add_reducer(coeff_power_swap r) {
		reducers.push_back(r);
	}

	string operator()(string s1, string s2) {
		// db("s1 * ", s1);
		// db("s2 * ", s2);
		// Multiply the polynomials
		Polynomial p1 = Polynomial(s1);
		Polynomial p2 = Polynomial(s2);
		Polynomial result = p1 * p2;

		// Reduce and simplify the result
		result.simplify( simplifier );
		for(auto it = reducers.begin(); it != reducers.end(); ++it)  {
			result.simplify( *it );
			result.simplify( simplifier );
		}
		result.simplify( simplifier );
		// db(" = ", result.expression() );
		return result.expression();
	}
} poly_multiplier;

typedef struct mod_adder {
	int N;
	mod_adder(int N) : N(N) {}
	int operator()(int a, int b) {
		return (a + b) % N;
	}

	string operator()(string s1, string s2) {
		int a = std::stoi(s1);
		int b = std::stoi(s2);
		return std::to_string( (*this)(a, b) );
	}
} mod_adder;

typedef struct mod_multiplier {
	int N;
	mod_multiplier(int N) : N(N) {}
	int operator()(int a, int b) {
		return (a * b) % N;
	}
	
	string operator()(string s1, string s2) {
		int a = std::stoi(s1);
		int b = std::stoi(s2);
		return std::to_string( (*this)(a, b) );
	}
} mod_multiplier;


/** Determines if the given character is a valid symbol for evaluation
 * Returns true if and only if the symbol is a digit, a parenthesis, or
 * 	a valid binary operator (+, -, *, /)
 */
bool is_valid_symbol(char c) {
	return isdigit(c) || c == 'x'|| c == ')' || c == '(' || c == '+' || 
		c == '-' || c == '*' || c == '/' || c == ' '|| c=='^';
}

int main(int argc, char** argv) {
	int NUM_ARGS = 4;

	if(argc != NUM_ARGS) {
		error("USAGE: evaluate -s <field size> <expression>");
	}

	size_t field_size;
	for(int i = 0; i < NUM_ARGS; ++i) {
		std::stringstream ss;
		ss << argv[i];
		std::string arg = ss.str();
		if( i == 1 ) {
			if( arg.compare("-s") ) {
				error("-s is the only supported command line option at this time.");
			}
		}
		if( i == 2) {
			if( arg.size() == 1 ) {
				if( isdigit(arg[0]) ) {
					field_size = std::stoi(arg);
				}
			}
			else if( arg.size() == 2 ) {
				if (isdigit(arg[0]) && isdigit(arg[1])) {
					field_size = std::stoi(arg);
				}
			}
			else {
				error("Specified field size is invalid or too large.");
			}
		}
		if( i == 3 ) {
			std::string stripped_expression;
			for(size_t j = 0; j < arg.size(); ++j) {
				if( !is_valid_symbol(arg[j]) ) {
					error("Expression should only contains digits and the symbol x.");
				}
			}
		}
	}


	// Build a parse tree from the input expression; parse tree
	// follows tradition algebraic orders of operation
	e_tree e1 = e_tree(argv[3]);

	if( field_size == 3 ) {
		// Build the field of size 3
		mod_adder int_adder (3);
		mod_multiplier int_mult (3);
		std::vector< string > field_elements = { "0", "1", "2" };
		typedef FiniteField<string,mod_adder,mod_multiplier> int_field;
		typedef typename int_field::Operator int_operator;
		int_field F = int_field(field_elements.begin(), 
								field_elements.end(), 
								int_adder, 
								int_mult, DEBUG);

		// Simplify the expression tree
		while( !e1.is_simplified() ) {
			e1.implement_operator('+', F.get_operator('+'));
			e1.implement_operator('-', F.get_operator('-'));
			e1.implement_operator('*', F.get_operator('*'));
			e1.implement_operator('/', F.get_operator('/'));
		}
		e1.print_tree();
	}
	else if ( field_size == 5 ) {
		// Build the field of size 5
		mod_adder int_adder (5);
		mod_multiplier int_mult (5);
		std::vector< string > field_elements = { "0", "1", "2", "3", "4" };
		typedef FiniteField<string,mod_adder,mod_multiplier> int_field;
		typedef int_field::Operator int_operator;
		int_field F = int_field(field_elements.begin(), 
								field_elements.end(), 
								int_adder, 
								int_mult, DEBUG);

		// Simplify the expression tree
		while( !e1.is_simplified() ) {
			e1.implement_operator('+', F.get_operator('+'));
			e1.implement_operator('-', F.get_operator('-'));
			e1.implement_operator('*', F.get_operator('*'));
			e1.implement_operator('/', F.get_operator('/'));
		}
		e1.print_tree();
	}
	else if( field_size == 7 ) {
		// Build the field of size 7
		mod_adder int_adder (7);
		mod_multiplier int_mult (7);
		std::vector< string > field_elements = { "0", "1", "2", "3", "4", 
												 "5", "6" };
		typedef FiniteField<string,mod_adder,mod_multiplier> int_field;
		typedef int_field::Operator int_operator;
		int_field F = int_field(field_elements.begin(), 
								field_elements.end(), 
								int_adder, 
								int_mult, DEBUG);

		// Simplify the expression tree
		while( !e1.is_simplified() ) {
			e1.implement_operator('+', F.get_operator('+'));
			e1.implement_operator('-', F.get_operator('-'));
			e1.implement_operator('*', F.get_operator('*'));
			e1.implement_operator('/', F.get_operator('/'));
		}
		e1.print_tree();
	}
	else if( field_size == 4 ) {
		// Build array representing x+1
		coeff_array substitute;
		substitute.push_back(1);
		substitute.push_back(1);

		coeff_power_swap reducer = coeff_power_swap(2, substitute);
		coeff_modulo modulus = coeff_modulo(2);

		poly_adder adder = poly_adder(modulus);
		poly_multiplier multiplier = poly_multiplier(modulus, reducer);

		typedef FiniteField<string,poly_adder,poly_multiplier> poly_field;
		typedef poly_field::Operator poly_operator;

		std::vector< string > field_elements = {
			"0", "1", "x", "x+1"
		};
		poly_field F = poly_field(field_elements.begin(), 
								field_elements.end(), 
								adder, 
								multiplier, DEBUG);

		// Simplify the expression tree
		while( !e1.is_simplified() ) {
			e1.implement_operator('+', F.get_operator('+'));
			e1.implement_operator('-', F.get_operator('-'));
			e1.implement_operator('*', F.get_operator('*'));
			e1.implement_operator('/', F.get_operator('/'));
		}
		e1.print_tree();
	}
	else if( field_size == 8 ) {
		// Make the power 2 reducer
		coeff_array substitute1 = {1, 1};

		// Make power 4 reducer
		coeff_array substitute2 = {0, 1, 1};

		coeff_power_swap reducer1 = coeff_power_swap(3, substitute1);
		coeff_power_swap reducer2 = coeff_power_swap(4, substitute2);
		coeff_modulo modulus = coeff_modulo(2);

		poly_adder adder = poly_adder(modulus);
		poly_multiplier multiplier = poly_multiplier(modulus, reducer1);
		multiplier.add_reducer(reducer2);

		typedef FiniteField<string,poly_adder,poly_multiplier> poly_field;
		typedef poly_field::Operator poly_operator;

		std::vector< string > field_elements = {
			"0", "1", "x", "x+1", "x^2", "x^2+x", "x^2+1", "x^2+x+1"
		};

		poly_field F = poly_field(field_elements.begin(), 
								field_elements.end(), 
								adder, 
								multiplier, DEBUG);

		// Simplify the expression tree
		while( !e1.is_simplified() ) {
			e1.implement_operator('+', F.get_operator('+'));
			e1.implement_operator('-', F.get_operator('-'));
			e1.implement_operator('*', F.get_operator('*'));
			e1.implement_operator('/', F.get_operator('/'));
		}
		e1.print_tree();
	}
	else if( field_size == 9 ) {
		// Make the power 2 reducer
		coeff_array substitute1 = {1, 1};

		coeff_power_swap reducer1 = coeff_power_swap(2, substitute1);
		coeff_modulo modulus = coeff_modulo(3);

		poly_adder adder = poly_adder(modulus);
		poly_multiplier multiplier = poly_multiplier(modulus, reducer1);

		typedef FiniteField<string,poly_adder,poly_multiplier> poly_field;
		typedef poly_field::Operator poly_operator;

		std::vector< string > field_elements = {
			"0", "1", "2", "x", "2x", "x+1", "x+2", "2x+1", "2x+2"
		};

		poly_field F = poly_field(field_elements.begin(), 
								field_elements.end(), 
								adder, 
								multiplier, DEBUG);

		// Simplify the expression tree
		while( !e1.is_simplified() ) {
			e1.implement_operator('+', F.get_operator('+'));
			e1.implement_operator('-', F.get_operator('-'));
			e1.implement_operator('*', F.get_operator('*'));
			e1.implement_operator('/', F.get_operator('/'));
		}
		e1.print_tree();
	}
	else if( field_size == 16 ) {
		// Make the power 4 reducer
		coeff_array substitute1 = {1, 1};

		// Make power 5 reducer
		coeff_array substitute2 = {0, 1, 1};

		// Make power 6 reducer
		coeff_array substitute3 = {0, 0, 1, 1};

		coeff_power_swap reducer1 = coeff_power_swap(4, substitute1);
		coeff_power_swap reducer2 = coeff_power_swap(5, substitute2);
		coeff_power_swap reducer3 = coeff_power_swap(6, substitute3);
		coeff_modulo modulus = coeff_modulo(2);

		poly_adder adder = poly_adder(modulus);
		poly_multiplier multiplier = poly_multiplier(modulus, reducer1);
		multiplier.add_reducer(reducer2);
		multiplier.add_reducer(reducer3);

		typedef FiniteField<string,poly_adder,poly_multiplier> poly_field;
		typedef poly_field::Operator poly_operator;

		std::vector< string > field_elements = {
			"0", "1", "x", "x+1", "x^2", "x^2+1", "x^2+x", "x^2+x+1", 
			"x^3", "x^3+1", "x^3+x", "x^3+x+1", "x^3+x^2", "x^3+x^2+1", 
			"x^3+x^2+x", "x^3+x^2+x+1"
		};

		poly_field F = poly_field(field_elements.begin(), 
								field_elements.end(), 
								adder, 
								multiplier, DEBUG);

		// Simplify the expression tree
		while( !e1.is_simplified() ) {
			e1.implement_operator('+', F.get_operator('+'));
			e1.implement_operator('-', F.get_operator('-'));
			e1.implement_operator('*', F.get_operator('*'));
			e1.implement_operator('/', F.get_operator('/'));
		}
		e1.print_tree();
	}
	else if( field_size == 25 ) {
		// Make the power 2 reducer
		coeff_array substitute1 = {2, 2};

		coeff_power_swap reducer1 = coeff_power_swap(2, substitute1);
		coeff_modulo modulus = coeff_modulo(5);

		poly_adder adder = poly_adder(modulus);
		poly_multiplier multiplier = poly_multiplier(modulus, reducer1);

		typedef FiniteField<string,poly_adder,poly_multiplier> poly_field;
		typedef poly_field::Operator poly_operator;

		std::vector< string > field_elements = {
			"0", "1", "2", "3", "4", "x+1", "x", "x+2", "x+3", "x+4", "2x", 
			"2x+1", "2x+2", "2x+3", "2x+4", "3x", "3x+1", "3x+2", "3x+3", 
			"3x+4", "4x", "4x+1", "4x+2", "4x+3", "4x+4"
		};

		poly_field F = poly_field(field_elements.begin(), 
								field_elements.end(), 
								adder, 
								multiplier, DEBUG);

		// Simplify the expression tree
		while( !e1.is_simplified() ) {
			e1.implement_operator('+', F.get_operator('+'));
			e1.implement_operator('-', F.get_operator('-'));
			e1.implement_operator('*', F.get_operator('*'));
			e1.implement_operator('/', F.get_operator('/'));
		}
		e1.print_tree();
	}
	else if( field_size == 27 ) {
		// Make the power 3 reducer
		coeff_array substitute1 = {2, 1};

		// Make power 4 reducer
		coeff_array substitute2 = {0, 2, 1};

		coeff_power_swap reducer1 = coeff_power_swap(3, substitute1);
		coeff_power_swap reducer2 = coeff_power_swap(4, substitute2);
		coeff_modulo modulus = coeff_modulo(3);

		poly_adder adder = poly_adder(modulus);
		poly_multiplier multiplier = poly_multiplier(modulus, reducer1);
		multiplier.add_reducer(reducer2);

		typedef FiniteField<string,poly_adder,poly_multiplier> poly_field;
		typedef poly_field::Operator poly_operator;
		std::vector< string > field_elements = {
			"0", "1", "2", "x", "x+1", "x+2", "2x", "2x+1", "2x+2", "x^2", 
			"x^2+1", "x^2+2", "x^2+x", "x^2+x+1", "x^2+x+2", "x^2+2x",
			"x^2+2x+1", "x^2+2x+2", "2x^2", "2x^2+1", "2x^2+2", "2x^2+x", 
			"2x^2+x+1", "2x^2+x+2", "2x^2+2x", "2x^2+2x+1", "2x^2+2x+2"
		};

		poly_field F = poly_field(field_elements.begin(), 
								field_elements.end(), 
								adder, 
								multiplier, DEBUG);

		// Simplify the expression tree
		while( !e1.is_simplified() ) {
			e1.implement_operator('*', F.get_operator('*'));
			e1.implement_operator('/', F.get_operator('/'));
			e1.implement_operator('+', F.get_operator('+'));
			e1.implement_operator('-', F.get_operator('-'));
		}
		e1.print_tree();
	}
	else if( field_size == 32 ) {
		// Make the power 5 reducer
		coeff_array substitute1 = {1, 0, 1};

		// Make power 6 reducer
		coeff_array substitute2 = {0, 1, 0, 1};

		// Make power 7 reducer
		coeff_array substitute3 = {0, 0, 1, 0, 1};

		// Make power 8 reducer
		coeff_array substitute4 = {1, 0, 1, 1};

		coeff_power_swap reducer1 = coeff_power_swap(5, substitute1);
		coeff_power_swap reducer2 = coeff_power_swap(6, substitute2);
		coeff_power_swap reducer3 = coeff_power_swap(7, substitute3);
		coeff_power_swap reducer4 = coeff_power_swap(8, substitute4);
		coeff_modulo modulus = coeff_modulo(2);

		poly_adder adder = poly_adder(modulus);
		poly_multiplier multiplier = poly_multiplier(modulus, reducer1);
		multiplier.add_reducer(reducer2);
		multiplier.add_reducer(reducer3);
		multiplier.add_reducer(reducer4);

		typedef FiniteField<string,poly_adder,poly_multiplier> poly_field;
		typedef poly_field::Operator poly_operator;
		std::vector< string > field_elements = {
			"0", "1", "x", "x+1", "x^2", "x^2+1", "x^2+x", "x^2+x+1", 
			"x^3", "x^3+1", "x^3+x", "x^3+x+1", "x^3+x^2", "x^3+x^2+1", 
			"x^3+x^2+x", "x^3+x^2+x+1", "x^4", "x^4+1", "x^4+x", 
			"x^4+x+1", "x^4+x^2", "x^4+x^2+1", "x^4+x^2+x", "x^4+x^2+x+1", 
			"x^4+x^3", "x^4+x^3+1", "x^4+x^3+x", "x^4+x^3+x+1", 
			"x^4+x^3+x^2", "x^4+x^3+x^2+1", "x^4+x^3+x^2+x", 
			"x^4+x^3+x^2+x+1"
		};

		poly_field F = poly_field(field_elements.begin(), 
								field_elements.end(), 
								adder, 
								multiplier, DEBUG);

		// Simplify the expression tree
		while( !e1.is_simplified() ) {
			e1.implement_operator('+', F.get_operator('+'));
			e1.implement_operator('-', F.get_operator('-'));
			e1.implement_operator('*', F.get_operator('*'));
			e1.implement_operator('/', F.get_operator('/'));
		}
		e1.print_tree();
	}
	else if( field_size == 49 ) {
		// Make the power 2 reducer
		coeff_array substitute1 = {4, 6};

		coeff_power_swap reducer1 = coeff_power_swap(2, substitute1);
		coeff_modulo modulus = coeff_modulo(7);

		poly_adder adder = poly_adder(modulus);
		poly_multiplier multiplier = poly_multiplier(modulus, reducer1);

		typedef FiniteField<string,poly_adder,poly_multiplier> poly_field;
		typedef poly_field::Operator poly_operator;
		std::vector< string > field_elements = {
			"0", "1", "2", "3", "4", "5", "6", "x+1", "x", "x+2", "x+3", 
			"x+4", "x+5", "x+6", "2x", "2x+1", "2x+2", "2x+3", "2x+4", 
			"2x+5", "2x+6", "3x", "3x+1", "3x+2", "3x+3", "3x+4", "3x+5", 
			"3x+6", "4x", "4x+1", "4x+2", "4x+3", "4x+4", "4x+5", "4x+6", 
			"5x", "5x+1", "5x+2", "5x+3", "5x+4", "5x+5", "5x+6", "6x", 
			"6x+1", "6x+2", "6x+3", "6x+4", "6x+5", "6x+6"
		};

		poly_field F = poly_field(field_elements.begin(), 
								field_elements.end(), 
								adder, 
								multiplier, DEBUG);

		// Simplify the expression tree
		while( !e1.is_simplified() ) {
			e1.implement_operator('+', F.get_operator('+'));
			e1.implement_operator('-', F.get_operator('-'));
			e1.implement_operator('*', F.get_operator('*'));
			e1.implement_operator('/', F.get_operator('/'));
		}
		e1.print_tree();
	}
	else {
		error("Unsupported field size.");
	}

	return 0;
}
