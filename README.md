## Synopsis

<!---
At the top of the file there should be a short introduction and/ or overview that explains **what** the project is. This description should match descriptions added for package managers (Gemspec, package.json, etc.)
-->

This repository contains three code projects. One involves graphs, another permutations, and the last symbolic algebra over finite fields. These projects demonstrate separation of data structures and algorithms, generic programming through the use of templates, object-oriented design, the use of functors and closures, recursive programming, and robust testing. 

## Graphs and Graph Algorithms

<!---
A short description of the motivation behind the creation and maintenance of the project. This should explain **why** the project exists.
-->

####Object-oriented design
The mass spring physical simulation in mass\_spring.cpp involves stimuli that superimpose to produce a net force on nodes in a graph. The Stimulus class defines an interface that all stimuli inherit and follow.

####Separation of data structures and algorithms 
The Graph.hpp file contains a Graph class that represents a directed or undirected graph. This structure makes use of a proxy design pattern: when a user of the data structure requests a Node or an Edge from the Graph, the user receives a proxy that refers back to a central representation of the graph. This keeps all clients of the graph in sync throughout different pieces of the program. The graph makes extensive use of iterators to allow algorithms to efficiently pass over all nodes, all edges, or all edges adjacent to any particular node.

####Template and generic programming
Template programming is used to create iterators that pass over the nodes and edges of the Graph. The Graph::TransformIter takes two template parameters: an iterator type and a data type. The transform iterator converts the type of a dereferenced iterator into the specified data type. Creating node and edge iterators involves templating the transform iterator on the underlying container and the types of a Node or Edge.

## Permutations

####Functors
Each permutation is composed of cycles that act as functors; each cycle encloses a one-to-one mapping between elements of a set and returns the permuted element when functionally applied to any element of symbol set. The definition of a Cycle is found in Permutation.hpp. 

####Overloaded operators
The Permutation class in Permutation.hpp makes use of overloaded multiplication and exponentiation operators to enable algebraic operations over permutations. 

## Symbolic Algebra and Finite Fields

####Recursive Programming
The ExpressionTree header constructs an algebraic expression tree from a mathematical expression by following algebraic order of operations. Each expression contains subexpressions that may also contain subexpressions themselves. Once algebraic operators are defined, the ExpressionTree performs recursive simplification. 

####Robust testing
The use of tests is demonstrated in Field.hpp. The Field.hpp file provides the FiniteField data structure that mathematically proves whether a symbol set and the operators of addition and multiplication over that symbol set compose a finite field by checking all the axioms for a finite field. The ExpressionTree and FiniteField data structures work together in evaluate.cpp to perform symbolic algebra over finite fields. 


<!---
## Code Example
Show what the library does as concisely as possible, developers should be able to figure out **how** your project solves their problem by looking at the code example. Make sure the API you are showing off is obvious, and that your code is short and concise.
-->



<!---
## Installation
Provide code examples and explanations of how to get the project.
-->


<!---
## API Reference
Depending on the size of the project, if it is small and simple enough the reference docs can be added to the README. For medium size to larger projects it is important to at least provide a link to where the API reference docs live.
-->


<!---
## Tests
Describe and show how to run the tests with code examples.
-->

## Contributors

<!---
Let people know how they can dive into the project, include important links to things like issue trackers, irc, twitter accounts if applicable.
-->

I developed Graph.hpp, shortest\_path.cpp, and mass\_spring.cpp in Harvard's CS207 with the support of the teaching staff. The remaining programs were developed as independent programming projects for Math152: Discrete Mathematics. 


<!---
## License
A short snippet describing the license (MIT, Apache, etc.)
-->
