/** 
 * lukasgemar@gmail.com
 *
 * Defines a set of functions that check the axioms for a field
 * 
**/

#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <assert.h>

/** Implements a finite field 
 * @pre The template type T must implement the following concept:
 * 	struct T {
 *  	virtual bool operator==(T a) = 0;
 *  };
 * @pre The template type ADD must implement the following concept:
 *  struct ADD {
 *		// Must return the sum of a and b
 * 		virtual bool operator()(T a, T, b) = 0;
 *  }
 * @pre The template type MULT must implement the following concept:
 *  struct MULT {
 *		// Must return the product of a and b
 * 		virtual bool operator()(T a, T, b) = 0;
 *  }
 */
template<typename T, typename ADD, typename MULT>
class FiniteField {

	////////////////////////////////////////////////////////////////////////
	///// PRIVATE TYPES ////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	typedef std::unordered_map< T, std::unordered_map<T, T> > table;
	typedef std::unordered_map< T, T > map;

	public: 

	////////////////////////////////////////////////////////////////////////
	///// PUBLIC TYPES /////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	typedef T value_type;
	typedef ADD adder;
	typedef MULT multiplier;
	typedef typename std::vector<value_type> element_list;
	typedef typename element_list::iterator iterator;

	class FieldElement;
	typedef FieldElement element;

	// Types
	/** Constructor: builds a finite field  
	 * @pre type(*IT) == T
	 * @pre [begin, end) defines a valid iterator range over all of the 
	 * 	elements of the finite field
	 * @param[in] adder takes two elements of type *IT == value_type and 
	 * 	returns the sum of the two elements, also of type *IT == value_type
	 * @param[in] multiplier takes two elements of type *IT == value_type 
	 * 	and returns the sum of the two elements, also of type 
	 * 	*IT == value_type
	 */
	template<typename IT>
	FiniteField(IT begin, IT end, ADD add, MULT multiply, bool debug=false):
						debug_(debug),
						add_(add), 
					    mult_(multiply) {

		// Build element list with all the elements in the finite field
		for(auto it = begin; it != end; ++it)
			elements_.push_back(*it);

		// Check the field axioms before building the addition and 
		// multiplication tables
		if( debug) {
			db("Checking field axioms...");
			db("");
			check_closure();	
			check_associativity();
			check_commutativity();
			check_zero_element();
			check_one_element();
			check_if_trivial();
			db("");

			// Build the addition and multiplication tables
			db("Building the addition and multiplication tables...");
		}
		for(auto it = begin; it != end; ++it) {
			for(auto jt = begin; jt != end; ++jt) {

				// Compute the sums and products
				value_type a = *it;
				value_type b = *jt;
				value_type sum = add(a, b);
				value_type product = multiply(a, b);

				// Fill in the computation tables
				addition_table_[a][b] = sum;
				multiplication_table_[a][b] = product;

				// Check if b is the additive identity
				if( a == sum )
					zero_ = b;

				// Check if b is the multiplicative identity
				if( a == product )
					one_ = b;
			}
		}
		if( debug ) {
			dbg("Build complete.");
			db("");

			// These checks require the zero_ and one_ element initialized in 
			// the above loop to build the addiiton and multiplication tables
			check_additive_inverses();
			check_multiplicative_inverses();
			check_distributivity();
			db("");
			db("Field axiom checks are complete.");
			db("");

			db("Building additive and multiplicative inverse tables...");
		}
		// Build the additive inverse table
		for(auto it = elements_.begin(); it != elements_.end(); ++it) {
			value_type a = *it;
			value_type a_inv;
			// Look for the inverse of a
			auto jt = elements_.begin();
			while( jt != elements_.end() ) {
				a_inv = *jt;
				if( add_(a, a_inv) == zero_ )
					break;
				++jt;
			}
			addinv_map_[a] = a_inv;
		}

		// Build the multiplicative inverse table
		for(auto it = elements_.begin(); it != elements_.end(); ++it) {
			value_type a = *it;
			if( a == zero_ ) 
				continue;
			value_type a_inv;
			// Look for the inverse of a
			auto jt = elements_.begin();
			while( jt != elements_.end() ) {
				a_inv = *jt;
				if( mult_(a, a_inv) == one_ )
					break;
				++jt;
			}
			multinv_map_[a] = a_inv;
		}
		if( debug ) {
			dbg("Build complete.");
		}
	}

	template<typename VAL>
	element build_element(VAL a) {
		return FieldElement(this, a);
	}


	class Operator {
		public:
		value_type operator()(value_type a, value_type b) {
			if( type == '+' )  {
				return f_->addition_table_[a][b];
			}
			else if( type == '-' ) {
				value_type b_inv = f_->addinv_map_[b];
				return f_->addition_table_[a][b_inv];
			}
			else if( type == '*' ) {
				return f_->multiplication_table_[a][b];
			}
			else if( type == '/' ) {
				value_type b_inv = f_->multinv_map_[b];
				return f_->multiplication_table_[a][b_inv];
			}
			assert(false);
		}
		private: 
		friend class FiniteField;
		Operator(FiniteField* f, char type) : type(type), f_(f) {
		}

		char type;
		FiniteField* f_;

	};

	Operator get_operator(char c) {
		return Operator(this, c);
	}

	/** A field element that uses the algebra tables of a field to overload
	 * the algebraic operators 
	 */
	class FieldElement {
		public: 

		element operator+(element b) {
			return element( f_, f_->addition_table_[val][b.val] );
		}
		
		element operator*(element b) {
			return element( f_, f_->multiplication_table_[val][b.val] );
		}

		element operator-(element b) {
		  	value_type b_inv = f_->addinv_map_[b.val];
			return element( f_, f_->addition_table_[val][b_inv] );
		}

		element operator/(element b) {
			value_type b_inv = f_->multinv_map_[b,val];
			return element(f_, f_->multiplication_table_[val][b_inv]);
		}

		bool operator==(element b) {
			return val == b.val;
		}

		bool operator!=(element b) {
			return val != b.val;
		}

		value_type val;

		private:

		friend class FiniteField;
		FieldElement(FiniteField* f, value_type val) : val(val), f_(f) {
		}

		FiniteField* f_;
	};

	private: 


	////////////////////////////////////////////////////////////////////////
	///// PRIVATE DATA MEMBERS ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	bool debug_;
	element_list elements_;
	adder add_;
	multiplier mult_;
	value_type zero_;
	value_type one_;
	table addition_table_;
	table multiplication_table_;
	map addinv_map_;
	map multinv_map_;

	////////////////////////////////////////////////////////////////////////
	///// HELPER FUNCTIONS /////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	// Thanks to Erik Schluntz for the following debug functions

	template<typename S> 
	void db(S s) {
		if(debug_)
			std::cout << s << std::endl;
	}

	template<typename S, typename U>
	void db(S t, U u) {
		if(debug_)
			std::cout << t << u << std::endl;
	}

	template <typename S1> // debug_ red
	void dbr(const S1 s1) {
		if(debug_)
			std::cout << "\x1b[31m" << s1 << "\x1b[0m" << std::endl;
	}

	template <typename S1> // debug_ green
	void dbg(const S1 s1) {
		if(debug_)
			std::cout << "\x1b[32m" << s1 << "\x1b[0m" << std::endl;
	}

	template<typename IT, typename S>
	bool in_range(IT first, IT last, S e) {
		return std::find(first, last, e) != last;
	}
	////////////////////////////////////////////////////////////////////////
	///// TEST SUITE FOR ANY TYPE IMPLEMENTING THE FIELD CONCEPT ///////////
	////////////////////////////////////////////////////////////////////////

	// Source of axioms: http://en.wikipedia.org/wiki/Field_(mathematics)

	// Check: For all a, b in F, both a + b and a · b are in F
	void check_closure() {
		db("Checking closure...");
		for(auto it = elements_.begin(); it != elements_.end(); ++it) {
			for(auto jt = elements_.begin(); jt != elements_.end(); ++jt) {

				// Get a and b from iterators
				value_type a = *it;
				value_type b = *jt;

				//db("a: ", a);
				//db("b: ", b);
				//db("a + b: ", add_(a, b));
				//db("a * b: ", mult_(a, b));
				// Check if the multiplication is closed
				assert( in_range(elements_.begin(), 
								 elements_.end(), 
								 add_(a, b)) );

				// Check if the addition is closed
				assert( in_range(elements_.begin(), 
							     elements_.end(), 
								 mult_(a,b)) );
			}
		}
		dbg("All asserts pass");
	}

	// Check: For all a, b, and c in F, the following equalities hold: 
	// a + (b + c) = (a + b) + c and a · (b · c) = (a · b) · c
	void check_associativity() {
		db("Checking associativity...");
		for(auto it = elements_.begin(); it != elements_.end(); ++it) {
			for(auto jt = elements_.begin(); jt != elements_.end(); ++jt) {
				for(auto kt =elements_.begin(); kt!=elements_.end(); ++kt) {
					// Dereference elements
					value_type a = *it;
					value_type b = *jt;
					value_type c = *kt;

					//db("a: ", a);
					//db("b: ", b);
					//db("c: ", c);
					//db("a + (b + c) = ", add_(a, add_(b, c)));
					//db("(a + b) + c = ", add_(add_(a, b), c));
					// Check associativity of addition
					assert(add_(a, add_(b, c)) == add_(add_(a, b), c));

					//db("a: ", a);
					//db("b: ", b);
					//db("c: ", c);
					//db("a * (b * c) = ", mult_(a, mult_(b, c)));
					//db("(a * b) * c = ", mult_(mult_(a, b), c));
					// Check associativity of multiplication
					assert(mult_(a, mult_(b, c)) == mult_(mult_(a, b), c));
				}
			}
		}
		dbg("All asserts pass");
	}

	// Check: For all a and b in F, the following equalities hold: 
	// a + b = b + a and a · b = b · a
	void check_commutativity() {
		db("Checking commutativity...");
		for(auto it = elements_.begin(); it != elements_.end(); ++it) {
			for(auto jt = elements_.begin(); jt != elements_.end(); ++jt) {

				// Get a and b from iterators
				value_type a = *it;
				value_type b = *jt;

				// Check if addition is commutative
				assert( add_(a, b) == add_(b, a) );
				 
				// Check if multiplication is commutative
				assert( mult_(a, b) == mult_(b, a) );
			}
		}
		dbg("All asserts pass");
	}

	// Check: There exists an element of F, called the additive identity 
	// element and denoted by 0, such that for all a in F, a + 0 = a
	void check_zero_element() {
		db("Checking for a zero element...");
		bool found_zero = false;
		value_type zero;
		for(auto it = elements_.begin(); it != elements_.end(); ++it) {
			zero = *it;
			value_type a;
			auto jt = elements_.begin();
			while( jt != elements_.end() ) {
				a = *jt;
				if( add_(a, zero) == a ) {
					++jt;
				}
				else {
					break;
				}
			}
			if( jt == elements_.end() ) {
				found_zero = true;
				break;
			}
		}
		assert(found_zero);
		zero_ = zero;
		dbg("All asserts pass");
	}

	// Check: There exists an element of F, called the multiplicative 
	// identity element and denoted by 1, such that for all a in F, 
	// a · 1 = a
	void check_one_element() {
		db("Checking for a one element...");
		bool found_one = false;
		value_type one;
		for(auto it = elements_.begin(); it != elements_.end(); ++it) {
			one = *it;
			value_type a;
			auto jt = elements_.begin();
			while( jt != elements_.end() ) {
				a = *jt;
				if( mult_(a, one) == a ) {
					++jt;
				}
				else {
					break;
				}
			}
			if( jt == elements_.end() ) {
				found_one = true;
				break;
			}
		}
		assert(found_one);
		one_ = one;
		dbg("All asserts pass");
	}

	// Check: Multiplicative identity and additive identity are distinct
	void check_if_trivial() {
		db("Ensuring that 0 does not equal 1...");
		db("Zero element: ", zero_);
		db("One element: ", one_);
		assert( zero_ != one_ );
		dbg("All asserts pass");
	}

	// Check: For every a in F, there exists an element −a in F, 
	// such that a + (−a) = 0
	void check_additive_inverses() {
		db("Looking for additive inverses...");
		for(auto it = elements_.begin(); it != elements_.end(); ++it) {
			value_type a = *it;
			value_type a_inv;
			// Look for the inverse of a
			auto jt = elements_.begin();
			while( jt != elements_.end() ) {
				a_inv = *jt;
				if( add_(a, a_inv) == zero_ )
					break;
				++jt;
			}
			assert( jt != elements_.end() );
			// assert( a == zero_ || a != a_inv );
		}
		dbg("All asserts pass");
	}

	// Check: For any a in F other than 0, there exists an element a−1 
	// in F, such that a · a−1 = 1
	void check_multiplicative_inverses() {
		db("Looking for multiplicative inverses...");
		//db("One element: ", one_);
		for(auto it = elements_.begin(); it != elements_.end(); ++it) {
			value_type a = *it;
			if( a == zero_ ) 
				continue;
			value_type a_inv;
			// Look for the inverse of a
			auto jt = elements_.begin();
			while( jt != elements_.end() ) {
				a_inv = *jt;
				//db("Looking for an inverse for: ", a);
				//db("Cantidate: ", a_inv);
				//db("Product: ", mult_(a, a_inv));
				if( mult_(a, a_inv) == one_ )
					break;
				++jt;
			}
			assert( jt != elements_.end() );
		}
		dbg("All asserts pass");
	}

	// Check: a + (-b) == a - b
	void check_subtraction() {
	}

	// Check: a * (b-1) == a / b
	void check_division() {
	}

	// Check: For all a, b and c in F, the following equality holds: 
	// a · (b + c) = (a · b) + (a · c)
	void check_distributivity() {
		db("Checking distributivity...");
		for(auto it = elements_.begin(); it != elements_.end(); ++it) {
			for(auto jt = elements_.begin(); jt != elements_.end(); ++jt) {
				for(auto kt =elements_.begin(); kt!=elements_.end(); ++kt) {
					// Dereference elements
					value_type a = *it;
					value_type b = *jt;
					value_type c = *kt;

					// Check distributive law
					assert(mult_(a, add_(b, c)) == 
						   add_(mult_(a, b), mult_(a, c)));
				}
			}
		}
		dbg("All asserts pass");
	}
};



