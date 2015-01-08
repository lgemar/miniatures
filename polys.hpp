#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <ctype.h>
#include <stdlib.h>
#include <vector>
#include "ExpressionTree.hpp"

typedef std::vector<int> coeff_array;
typedef ExpressionTree e_tree;
typedef e_tree::term term;

////////////////////////////////////////////////////////////////
///// HELPER FUNCTIONS DEALING WITH COEFFICIENT MATRICES ///////
////////////////////////////////////////////////////////////////


coeff_array coeff_add(coeff_array a, coeff_array b) {
	coeff_array outer;
	coeff_array inner;
	coeff_array result;
	if( a.size() > b.size() ) {
		outer = a;
		inner = b;
	}
	else {
		outer = b;
		inner = a;
	}
	result.reserve(outer.size());
	for(size_t i = 0; i < outer.size(); ++i) {
		result.push_back(outer[i]);
		if( i < inner.size() ) {
			result[i] += inner[i];
		}
	}
	return result;
}

coeff_array coeff_shift_right(coeff_array a, int shift_amount) {
	coeff_array result;
	result.reserve(a.size() + shift_amount);
	// Add shift_amount zeros at the front
	for(int i = 0; i < shift_amount; ++i) {
		result.push_back(0);
	}
	// Copy in all the values of a
	for(int i = 0; i < a.size(); ++i) {
		result.push_back(a[i]);
	}
	return result;
}


coeff_array coeff_scalar_multiply(coeff_array a, int s) {
	coeff_array result;
	result = coeff_array(a);
	for(int i = 0; i < a.size(); ++i) {
		result[i] *= s;	
	}
	return result;
}

coeff_array coeff_multiply(coeff_array a, coeff_array b) {
	coeff_array result;
	for(size_t i = 0; i < a.size(); ++i) {
		// Shift and coeff_multiply b
		coeff_array addend = coeff_shift_right(b, i);
		addend = coeff_scalar_multiply(addend, a[i]);

		// Accumulate the result
		result = coeff_add(result, addend);
	}
	return result;
}



////////////////////////////////////////////////////////////////
///// POLYNOMIAL CLASS /////////////////////////////////////////
////////////////////////////////////////////////////////////////

// Assumes polynomials of one variable and that variable is 
// named x
class Polynomial {
	public:

	Polynomial() : coeffs_(coeff_array()) {}
	Polynomial(coeff_array c) : coeffs_(c) {}

	// @pre there are no parentheses in this string
	// @pre there are no -, *, / in the string -- all of these must
	// 	be worked out by prior evaluatation
	// @pre all terms are of the form ax^n, bx, or c
	// @pre this constructor is purely for building a coeff array from
	// 	an expression of the form cnx^n ... c2x^2 + c1x + c0
	Polynomial(std::string s) : coeffs_(coeff_array()) {
		e_tree e (s);
		coeff_array result = coeff_array();
		for(auto it = e.begin(); it != e.end(); ++it) {
			term t = *it;
			if( t.tag == ADD )
				continue;
			else if( t.tag )  
				error("Invalid operand found in constructor for polynomial");
			result = coeff_add(result, coeffs_from_term(t.value));
		}
		coeffs_ = result;
	}

	Polynomial(int a, int b) {
		coeffs_.push_back(b);
		coeffs_.push_back(a);
	}

	Polynomial(int a, int b, int c) {
		coeffs_ = coeff_array();
		coeffs_.push_back(c);
		coeffs_.push_back(b);
		coeffs_.push_back(a);
	}

	void print() {
		std::cout << expression() << std::endl;
	}

	std::string expression() {
		std::stringstream ss;
		std::vector<int> non_zero_coeffs;
		for(int i = coeffs_.size() - 1 ; i >= 0; --i) {
			if( coeffs_[i] ) {
				//db("non-zero coeff i: ", i);
				//db("Coeff[i] value: ", coeffs_[i]);
				non_zero_coeffs.push_back(i);
			}
		}
		for(int i = 0; i < non_zero_coeffs.size(); ++i) {

			// Print a plus sign before every term except for the
			// first
			if( i > 0 )
				ss << '+';

			int pow = non_zero_coeffs[i];

			// Print out the coefficient if it's the last in the chain
			// or if it is greater than 1
			if(pow == 0 || coeffs_[pow] > 1) {
				ss << coeffs_[pow];
			}

			if( pow == 1 ) {
				ss << 'x';
			}

			if( pow > 1 ) {
				ss << 'x' << '^' << pow;
			}
		}

		if(all_zeros(coeffs_))
			ss << "0";
		//db("Returned expression: ", ss.str());
		return ss.str();
	}

	Polynomial operator+(Polynomial b) {
		return Polynomial(coeff_add(coeffs_, b.coeffs_));
	}

	Polynomial operator*(Polynomial b) {
		return Polynomial(coeff_multiply(coeffs_, b.coeffs_));
	}

	// @pre f takes a vector of ints and returns a vector of
	// ints
	template<typename FUNCT>
	void simplify(FUNCT f) {
		coeffs_ = f(coeffs_);
	}

	private: 

	bool all_zeros(coeff_array c) {
		int i = 0;
		for(; i < c.size(); ++i) {
			if( c[i] ) {
				break;
			}
		}
		return i == c.size();
	}
	// @pre all terms are of the form ax^n, bx, or c -- no negatives
	coeff_array coeffs_from_term(std::string s) {
		coeff_array result;
		int pos;
		if( (pos = contains(s, '^')) != s.size() ) {
			// Handle errors
			if( pos != 1 && pos != 2 )
				error("Coefficients and exponents cannot be greater than 9.");
			if ( s.size() == 4 ) {
				if( s[1] != 'x' )
					error("Exponential terms must be of the form ax^n");
				if( s[2] != '^' )  
					error("Exponential terms must be of the form ax^n");
				// Compute exponent and coefficient
				int exponent = atoi(&s[3]);
				int coefficient = atoi(&s[0]);
				for(int i = 0; i < exponent; ++i) {
					result.push_back(0);
				}
				result.push_back(coefficient);
			}
			else if( s.size() == 3 ) {
				if( s[0] != 'x' )
					error("Exponential terms must be of the form ax^n");
				if( s[1] != '^' )  
					error("Exponential terms must be of the form ax^n");
				// Compute exponent and coefficient
				int exponent = atoi(&s[2]);
				for(int i = 0; i < exponent; ++i) {
					result.push_back(0);
				}
				result.push_back(1);
			}
			else {
				error("Coefficients and exponents must be of the form ax^n where a and n are between 0 and 9.");
			}
		}
		else if( (pos = contains(s, 'x')) != s.size() ) {
			if( s.size() == 2 ) {
				int coefficient = atoi(&s[0]);
				result.push_back(0);
				result.push_back(coefficient);
			}
			else if(s.size() == 1) {
				result.push_back(0);
				result.push_back(1);
			}
			else {
				error("Coefficients on x cannot be greater than 9.");
			}
		}
		else {
			if( s.size() != 1 ) {
				error("Coefficients cannot be greater than 9");
			}
			int coefficient = atoi(&s[0]);
			result.push_back(coefficient);
		}
		return result;
	}

	// Returns true if the string contains the variable x
	size_t contains(std::string s, char c) {
		size_t i = 0;
		for( ; i < s.size(); ++i) {
			if( s[i] == c )
				break;
		}
		return i;
	}


	coeff_array coeffs_;
};


