## Synopsis

<!---
At the top of the file there should be a short introduction and/ or overview that explains **what** the project is. This description should match descriptions added for package managers (Gemspec, package.json, etc.)
-->

This repository contains three code projects. One involves graphs, another permutations, and the last symbolic algebra over finite fields. Each of these projects demonstrates separation of data structures and algorithms. 

In the **Graphs and Graph Algorithms** directory, the Graph.hpp provides a unique proxy-pattern based approach to representing a graph. Topological sorting and finding shortest paths between nodes are two graph algorithms that use the Graph.hpp data structure. 

In **Permutations**, pmul.cpp performs multiplication of permutations represented in cyclic notation. For example, (123) is a permutation that represents the permutation of three symbols -- 1 to 2, 2 goes to 3, and 3 goes to 1. The multiplication of two permutations is performed by applying the cyclic permutations from right to left on the set of symbols 1 through 9. 

**Symbolic Algebra and Finite Fields** has a number of files that perform symbolic algebra. ExpressionTree.hpp constructs a tree structure from an algebraic expression that preserves order of operations. Functionally defining the operators +, -, \*, and / allows for recursive simplification of any expression tree. Field.hpp data structure contructs a finite field from custom defined multiplication and division operators and is a good example of functional programming design patterns. 

## Code Example

<!---
Show what the library does as concisely as possible, developers should be able to figure out **how** your project solves their problem by looking at the code example. Make sure the API you are showing off is obvious, and that your code is short and concise.
-->

## Motivation

<!---
A short description of the motivation behind the creation and maintenance of the project. This should explain **why** the project exists.
-->

## Installation

<!---
Provide code examples and explanations of how to get the project.
-->

## API Reference

<!---
Depending on the size of the project, if it is small and simple enough the reference docs can be added to the README. For medium size to larger projects it is important to at least provide a link to where the API reference docs live.
-->

## Tests

<!---
Describe and show how to run the tests with code examples.
-->

## Contributors

<!---
Let people know how they can dive into the project, include important links to things like issue trackers, irc, twitter accounts if applicable.
-->

## License

<!---
A short snippet describing the license (MIT, Apache, etc.)
-->
