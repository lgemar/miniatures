#include <iostream>
#include <set>
#include "Permutation.hpp"

typedef set<char> SET;

string build( Permutation& p, const SET e ) {
	// Make a copy of charcter set 
	SET elements = SET (e);
	// Iterate through set and build cycle structure
	char root;
	char next;
	SET::iterator it;
	string product;
	while ( !elements.empty() ) {
		it = elements.begin();
		root = *it;
		product.push_back('(');
		product.push_back(root);
		elements.erase(root);
		next = p( root );
		while ( next != root ) {
			elements.erase(next);
			product.push_back(next);
			next = p( next );
		}
		product.push_back(')');
	}
	return product;
}

/** Make a set of characters out of a and b and store result in e */
void make_set( SET& e, string a , string b ) {
	for (size_type i = 0; i < a.size(); i++) {
		if (a[i] != '(' && a[i] != ')')
			e.insert(a[i]);
	}
	for (size_type i = 0; i < b.size(); i++) {
		if (b[i] != '(' && b[i] != ')')
			e.insert(b[i]);
	}
}

int main (int argc, char* argv[]) {
	(void) argc;
	// Grab command line input
	string a (argv[1]);
	string b (argv[2]);

	// Create a set of characters in a and b
	SET elements; 
	make_set( elements, a , b );

	// Create a permutation representing the transformation
	Permutation pa = Permutation( a );
	Permutation pb = Permutation( b );
	Permutation ainv = pa^(-1);
	Permutation binv = pb^(-1);
	// Basic multiplications of ab and ba
	string ab = build( pa * pb , elements);
	string ba = build( pb * pa , elements);
	cout << ab << endl;
	cout << ba << endl;
	// Conjugate multiplications
	string abainv = build( pa * pb * ainv , elements);
	string babinv = build( pb * pa * binv , elements);
	cout << abainv << endl;
	cout << babinv << endl;
	return 0;
}
