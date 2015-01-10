#include <iostream>
#include <string>
#include <regex>
#include "error.hpp"
#include "db.hpp"

int main(int argc, char** argv) {
	if( argc != 2 ) {
		error("USAGE: d3mult <expression>");
	}

	std::string input (argv[1]);

	// Check to make sure input only contains r and f
	for(auto it = input.begin(); it != input.end(); ++it) {
		char c = *it;
		if( c != 'r' && c != 'f' )
			error("Input string can only contains sequence of r and f");
	}

	// Print the input
	db("Input: ", input);

	std::string result;
	std::regex e1 ("([rf]*)rrr([rf]*)");
	std::regex e2 ("([rf]*)ff([rf]*)");
	std::regex e3 ("([rf]*)fr([rf]*)");

	result = input;
	// using string/c-string (3) version:
	while( std::regex_match(result, e1) || 
		   std::regex_match(result, e2) ||
		   std::regex_match(result, e3) ) {

		result = std::regex_replace (result,e1,"$1$2");
		db("Simplifying rrr -> I ... ", result);
		result = std::regex_replace (result,e2,"$1$2");
		db("Simplifying ff -> I ... ", result);
		result = std::regex_replace (result,e3,"$1rrf$2");
		db("Simplifying fr -> rrf ... ", result);
	}

	if( !result.compare("") )
		result = "I";

	// Print the result
	db("Output: ", result);

	return 0;
}
