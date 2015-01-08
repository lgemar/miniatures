#include <string>
#include <algorithm>
#include "db.hpp"
#include "error.hpp"
#include <cstring>
#include <assert.h>
#include <stdio.h>

#define VALUE 0
#define ADD 1
#define SUB 2
#define MULT 3
#define DIV 4

class ExpressionTree {
	public:

	typedef std::string value_type;
	struct term;
	typedef std::vector<term> term_list;

	struct term {
		int tag;
		value_type value;

		term_list leaves;
		term() : tag(-1), value(value_type()) {
		}
	};

	// Create a parse tree out of a c++ string
	ExpressionTree(const std::string s) {
		exp_ = std::string(s);
		// Remove whitespace and store in the exp_ private member
		exp_.erase(std::remove_if(exp_.begin(), exp_.end(),isspace()), 
				exp_.end());
		head_term_ = build_tree_(exp_);
	}

	// Create a parse tree out of a char*
	ExpressionTree(const char* s) {
		// Remove whitespace and store in the exp_ private member
		int len = str_len(s);
		char s_copy[len];
		strcpy(s_copy, s);
		char* pch = strtok(s_copy, " \t\n\r");
		while( pch != NULL ) {
			exp_.append(pch);
			pch = strtok(NULL, " \t\n\r");
		}
		head_term_ = build_tree_(exp_);
	}

	~ExpressionTree() {
	}

	template<typename OPERATOR>
	void implement_operator(char c, OPERATOR op) {
		int op_code = 0;
		switch(c) {
			case '+' : op_code = ADD; break;
			case '-' : op_code = SUB; break;
			case '*' : op_code = MULT; break;
			case '/' : op_code = DIV; break;
			default: error("Operation not recognized.");
		}
		head_term_ = simplify_(head_term_, op, op_code);
	}

	void print_tree() {
		print_terms_(head_term_);
		std::cout << std::endl;
	}

	std::string expression() {
		return exp_;
	}

	term_list::iterator begin() {
		return head_term_.leaves.begin();
	}

	term_list::iterator end() {
		return head_term_.leaves.end();
	}

	bool is_simplified() {
		return is_leaf(head_term_);
	}

	private:

	//////////////////////////////////////////////////////////////////
	///// PRIVATE MEMBERS ////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	std::string exp_;
	term head_term_;

	//////////////////////////////////////////////////////////////////
	///// HELPER FUNCTIONS ///////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	// Returns the simplified term applying the operation op
	// For now, only handles binary operations
	template<typename OPERATION>
	term simplify_(term t, OPERATION op, int op_code) {
		// Grab the list of all leaves on this branch
		term_list leaf_list = t.leaves;

		size_t num_leaves = leaf_list.size();

		if( num_leaves == 0 ) { 
			return t;
		}
		if( num_leaves == 1 ) {
			return simplify_(t.leaves[0], op, op_code);
		}
		if( num_leaves == 2) {
			error("Invalid expression tree");
		}

		// Grab the three interesting terms for this pass
		term left = leaf_list[0];
		term operation = leaf_list[1];
		term right = leaf_list[2];

		int tag = leaf_list[1].tag;

		// Simplify any sub-expressions in the left side
		if( !is_leaf(left) ) {
			left = simplify_(left, op, op_code);
		}
		
		// Simplify any sub-expressions in the right side
		if( !is_leaf(right) ) {
			right = simplify_(right, op, op_code);
		}

		// Allow for higher precedence terms to evaluate first
		int next_op = next_operation(leaf_list.begin() + 2, 
									 leaf_list.end());
		if( next_op > tag && num_leaves > 3 ) {
			// Copy all the remaining terms into a term
			term temp;

			// Add the computed branch to the remaining terms and 
			// then simplify
			temp.leaves.push_back(right);
			for(size_t i = 3; i < num_leaves; ++i) {
				temp.leaves.push_back(leaf_list[i]);
			}
			right = simplify_(temp, op, op_code);
		}

		// Condense the expression using the given operator
		term temp_branch;
		if(tag == op_code && is_leaf(left) && is_leaf(right)) {
			temp_branch.tag = 0;
			temp_branch.value = op(left.value, right.value);
		}
		else {
			temp_branch.leaves.push_back(left);
			temp_branch.leaves.push_back(operation);
			temp_branch.leaves.push_back(right);
		}

		// If this was a higher precedence expression, move to more
		// terms
		if( next_op <= tag && num_leaves > 3 ) {
			term temp_trunk;

			// Add the computed branch to the remaining branches
			temp_trunk.leaves.push_back(temp_branch);
			for(size_t i = 3; i < num_leaves; ++i) {
				temp_trunk.leaves.push_back(leaf_list[i]);
			}

			// Simplify the remaining terms
			return simplify_(temp_trunk, op, op_code);
		}
		else {
			return temp_branch;
		}
	}

	template<typename IT> 
	int next_operation(IT begin, IT end) {
		for(auto it = begin; it != end; ++it) {
			if( (*it).tag ) 
				return (*it).tag;
		}
		return 0;
	}

	void print_terms_(term t) {
		if( !t.leaves.size() ) {
			switch( t.tag ) {
				case VALUE: std::cout << t.value; break;
				case ADD: std::cout << "+"; break;
				case SUB: std::cout << "-"; break;
				case MULT: std::cout << "*"; break;
				case DIV: std::cout << "/"; break;
				default: std::cout << ""; 
			}
		}
		else {
			std::cout << "( ";
			for(size_t i = 0; i < t.leaves.size(); ++i) {
				print_terms_(t.leaves[i]);
			}
			std::cout << " )";
		}
	}

	bool is_leaf(term t) {
		return t.leaves.size() == 0;
	}

	bool lonely_branch(term t) {
		return t.leaves.size() == 1;
	}


	// Inspiration for pieces of parser came from 
	// http://www.cplusplus.com/forum/general/49098/
	term build_tree_(std::string s) {
		// db("Build tree on: ", s);
		term t;
		// Helper variables
		int num_open_brackets = 0;
		int separator_tag = 0;
		int bracket;
		size_t i = 0;
		// Declare terms
		term op_term;
		term var_term;
		term sub_term;
		// Substrings and variable names
		std::string sub_string;
		std::string var_name;
		while( i != s.size() ) {
			separator_tag = 0;
			bracket = 0;
			switch( s[i] ) {
				case '(' : ++num_open_brackets; break;
				case ')' : --num_open_brackets; break;
				case '+' : 	separator_tag = ADD; break;
				case '-' : separator_tag = SUB; break;
				case '*' : separator_tag = MULT; break;
				case '/' : separator_tag = DIV; break;
				default: separator_tag = VALUE; break;
			}

			if( s[i] == '(' || s[i] == ')' || num_open_brackets ) {
				sub_string.append(1, s[i]);
			}
			else if( separator_tag ) {
				if( !(var_name.size() ^ sub_string.size()) )
					error("Incorrect operation or bracket separators");

				// Tack on the last variable name to the leaves list
				if( var_name.size() ) {
					if( sub_string.size() )
						error("Dangling variable or operator");
					var_term = term();
					var_term.tag = VALUE;
					var_term.value = var_name;
					t.leaves.push_back(var_term);
					var_name.clear();
				}
				// Take on the last set of terms to the leaves list
				if( sub_string.size() ) {
					if( var_name.size() )
						error("Dangling variable or operator");
					sub_term = build_tree_(trim(sub_string));
					t.leaves.push_back(sub_term);
					sub_string.clear();
				}

				// Build the separator term
				op_term = term();
				op_term.tag = separator_tag;
				t.leaves.push_back(op_term);
			}
			else {
				var_name.append(1, s[i]);
			}

			//db(" s[i]: ", s[i]);
			//db(" var_name: ", var_name);
			//db(" sub_string: ", sub_string);

			++i;

			// Ensure that brackets are used properly in the expr
			if( num_open_brackets < 0 )
				error("Dangling brackets");
		}

		if( separator_tag )
			error("Incorrect operation or bracket separators");

		// Tack on the last variable name to the leaves list
		if( var_name.size() ) {
			var_term = term();
			var_term.tag = VALUE;
			var_term.value = var_name;
			t.leaves.push_back(var_term);
			var_name.clear();
		}
		
		// Take on the last set of terms to the leaves list
		if( sub_string.size() ) {
			sub_term = build_tree_(trim(sub_string));
			t.leaves.push_back(sub_term);
			sub_string.clear();
		}

		// Check to make sure that all brackets were closed
		if( num_open_brackets > 0 )
			error("Unclosed brackets");
		if( num_open_brackets < 0 )
			error("Incorrect bracket separations");
		return t;
	}

	std::string trim(std::string s) {
		return s.substr(1, s.size() - 2);
	}

	int str_len(const char* s) {
		int length = 0;
		while(*s != '\0') {
			++length;
			++s;
		}
		return length;
	}

	struct isspace {
		bool operator()(char c) {
			return c == '\t' || c == '\n' || c == ' ';	
		}
		bool operator()(std::string s) {
			return !s.compare("\t")||!s.compare("\n")||!s.compare(" ");
		}
	};
};
