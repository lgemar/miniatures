#include <iostream>
#include <stdio.h>
#include "ExpressionTree.hpp"

typedef ExpressionTree e_tree;

int main(int argc, char** argv) {
	if(argc != 2) {
		std::cout << "USAGE: parse <expression>" << std::endl;
		return 1;
	}
	// Build a parse tree from the input expression; parse tree
	// follows tradition algebraic orders of operation
	e_tree e1 = e_tree(argv[1]);
	e1.print_tree();
	return 0;
}
