#include <map>
#include <list>
#include <string>
#include <cmath>
using namespace std;

typedef map<char, char> MAP;
typedef pair<char, char> PAIR;
typedef unsigned size_type;

class Cycle {

	public:
		Cycle( string a ) {
			// Set inverse to false
			inverted_ = false;
			// Create forward and reverse mappings
			int i = 0;
			for( ; (i < a[i + 1]) != '\0'; i++ ) {
				map_.insert( PAIR( a[i], a[i+1] ));
				inverted_map_.insert( PAIR( a[i+1], a[i] )); 
			}
			map_.insert( PAIR( a[i], a[0] ));
			inverted_map_.insert( PAIR( a[0], a[i] )); 
		}

		char operator()( char b ) {
			return find( b );
		}
		
	private: 

		void invert() {
			inverted_ = (!inverted_);
		}

		char find( char b ) {
			MAP::iterator it;
			if (inverted_ 
					&& ((it = inverted_map_.find( b )) != inverted_map_.end())) {
				return it->second;
			}
			else if (!inverted_ && 
							((it = map_.find( b )) != map_.end())) {
				return it->second;
			}
			else {
				return b;
			}

		}

		friend class Permutation; // allows access to private members
		MAP map_;
		MAP inverted_map_;
		bool inverted_;
};

class Permutation {

	public:
		// Default constructor
		Permutation() {
		}

		// Construct permutation from string
		Permutation( string a ) {
			size_type substr_pos;
			size_type substr_size;
			// Build cycles
			for( size_type i = 0; i < a.size(); i++) {
				while( a[i] != '(' ) {
					i++;
				}
				substr_pos = i + 1;
				while( a[i] != ')' ) {
					i++;
				}
				substr_size = i - substr_pos;
				string acycle = a.substr( substr_pos, substr_size );
				cycles_.push_front( Cycle( acycle ) );
			}
		}

		char operator()( char b ) {
			list<Cycle>::iterator it;
			char result;
			if ( (it = cycles_.begin()) != cycles_.end() ) {
				result = (*it)( b );
				for ( it++ ; it != cycles_.end(); it++) {
					result = (*it)( result );
				}
				return result;
			}
			else {
				return b;
			}
		}

		Permutation& operator* (const Permutation& other) {
			Permutation* this_copy_p = clone();
			this_copy_p->cycles_.insert(this_copy_p->cycles_.begin(), 
										other.cycles_.begin(), 
										other.cycles_.end());
			return *this_copy_p;
		}

		Permutation& operator^ (int pow) {
			Permutation* this_copy_p = clone();
			if (pow < 0) {
				this_copy_p->invert();
			}
			for (int i = 0; i < (int)abs((float)pow) - 1 ; i++) {
				this_copy_p = &((*this_copy_p) * (*this_copy_p));
			}
			return *this_copy_p;
		}

	private: 
		// Construct a permutation copy of an original
		Permutation* clone() {
			Permutation* clone = new Permutation();
			clone->cycles_ = list<Cycle> (cycles_);
			return clone;
		}

		void invert() {
			cycles_.reverse();
			list<Cycle>::iterator it;
			for ( it = cycles_.begin(); it != cycles_.end(); it++) {
				it->invert();
			}
		}

		list<Cycle> cycles_;
	};
