/**
 * @file tsort.cpp
 * Prints
 * A B
 * where A = number of nodes
 *       B = number of edges
 * and launches an SDLViewer to visualize the system
 */

#include <fstream>
#include <set>
#include <stdlib.h>
#include <iterator>

#include "CS207/SDLViewer.hpp"
#include "CS207/Util.hpp"

#include "Graph.hpp"

typedef struct node_data {
	int id;
	bool temp;
	bool perm;
} node_data;

typedef Graph<node_data, int> GraphType;
typedef GraphType::Node Node;
typedef std::vector<int> topo_list;
std::set<int> node_set;
topo_list t_list;

// Algorithm taken from wikipedia page on Topological Sorting
void visit(Node& n) {
	topo_list t;
	if(n.value().temp) {
		  std::cout <<"tsort: "<<"input contains a loop:" << std::endl;
	}
	else {
		n.value().temp = true;
		for(auto it = n.incoming_edge_begin(); 
			it != n.incoming_edge_end(); ++it) {
			Node other = (*it).node1();
			visit(other);
		}
		// If this node doesn't have a permanent mark on it yet then
		// it hasn't been place in the topological list ordering yet
		// and we need to do that! 
		if( !n.value().perm )
			t_list.push_back(n.index());
		n.value().perm = true;
		n.value().temp = false;
		node_set.erase(n.value().id);
	}
}

void print_directed_edges(GraphType& g) {
	for(auto it = g.node_begin(); it != g.node_end(); ++it) {
		Node n = *it;
		std::cout << "Node " << n.index() << " ... " << std::endl;
		for(auto jt = n.incoming_edge_begin(); 
			jt != n.incoming_edge_end(); ++jt) {
			Node other = (*jt).node1();
			std::cout << "depends on node " << other.index() << std::endl;
		}
	}
}

int main(int argc, char** argv)
{
  // Check arguments
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << "NODES_FILE\n";
    exit(1);
  }

  // Construct a Graph
  GraphType graph (true);
  // Construct a vector of nodes
  std::vector<typename GraphType::node_type> nodes;

  // Create a file stream from the input file
  std::filebuf fb;
  fb.open(argv[1], std::ios::in);
  std::istream edge_file(&fb);

  // Try reading from the file stream into a vector
  auto f_start = std::istream_iterator<int> (edge_file);
  auto f_end = std::istream_iterator<int>();

  // Create a vector with pairs of edges and a set with all the nodes
  std::vector<int> edge_vect(f_start, f_end);
  node_set.insert(edge_vect.begin(), edge_vect.end());

  // Attempt to put all the nodes into a set
  for(auto it = node_set.begin(); it != node_set.end(); ++it) {
  	Node n = graph.add_node(Point());
	n.value().id = *it;
    nodes.push_back(n);
  }

  // Add the edges to the graph
  assert(edge_vect.size() % 2 == 0);
  for(int i = 0; i < (int) edge_vect.size(); i += 2) {
        graph.add_edge(nodes[edge_vect[i]], nodes[edge_vect[i+1]]);
  }

  print_directed_edges(graph);

  // Topological sorting algorithm
  while( !( node_set.empty() ) ) {
  	  auto it = node_set.begin();
	  Node n = graph.node( *it );
	  visit(n);
  }

  // Print the answer
  for(auto it = t_list.begin(); it != t_list.end(); ++it) {
  	std::cout << *it << std::endl;
  }

  return 0;
}
