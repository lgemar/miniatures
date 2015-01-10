/**
 * @file shortest_path.cpp
 * Test script for using our templated Graph to determine shortest paths.
 *
 * @brief Reads in two files specified on the command line.
 * First file: 3D Points (one per line) defined by three doubles
 * Second file: Tetrahedra (one per line) defined by 4 indices into the point
 * list
 */

#include <vector>
#include <fstream>
#include <math.h>
#include <queue>
#include <set>

#include "CS207/SDLViewer.hpp"
#include "CS207/Util.hpp"
#include "CS207/Color.hpp"

#include "Graph.hpp"

struct MyNodeData {
	double dist;
	bool perm;
	bool temp;
};
typedef Graph<MyNodeData, double> GraphType;
typedef GraphType::node_type Node;
typedef GraphType::edge_type Edge;

/** Define custom operator to return a color object for a node */
struct MyColorFunc {
	double lp_;

	/** Constructor */
	MyColorFunc(double longest_path) : lp_(longest_path) {
	}

	template <typename NODE>
	// Return a color object to color the graph
	CS207::Color operator()(const NODE& node) {
		return CS207::Color::make_heat(node.value().dist / lp_);
	}
};

/** Comparator that compares the distance from a given point p.
 */
struct MyComparator {
   Point p_;
   MyComparator(const Point& p) : p_(p) {
   };

   template <typename NODE>
   // Return true if node1 is closer to p than node2
   bool operator()(const NODE& node1, const NODE& node2) const {
		double dist1 = norm(node1.position() - p_);
		double dist2 = norm(node2.position() - p_);
		return dist1 < dist2;
   }
};

/** Calculate shortest path lengths in @a g from the nearest node to @a point.
 * @param[in,out] g Input graph
 * @param[in] point Point to find the nearest node to.
 * @post Graph has modified node values indicating the minimum path length
 *           to the nearest node to @a point
 * @post Graph nodes that are unreachable to the nearest node to @a point have
 *           the value() -1.
 * @return The maximum path length found.
 *
 * Finds the nearest node to @a point and treats that as the root node for a
 * breadth first search.
 * This sets node's value() to the length of the shortest path to
 * the root node. The root's value() is 0. Nodes unreachable from
 * the root have value() -1.
 */
double shortest_path_lengths(GraphType& g, const Point& point) {
	double longest_path = 0;
	Node current_node;
	Node adjacent_node;
	Node closest_node;
	Edge incident_edge;
	std::set<Node> visited;
	std::queue<Node> to_visit;

	// Find closest current_node to the given point
	auto closest = std::min_element(g.node_begin(), 
									g.node_end(), MyComparator(point));
	closest_node = *closest;
	closest_node.value().dist = 0;
	closest_node.value().perm = true;
	closest_node.value().temp = false;

	to_visit.push(closest_node);
	while (!to_visit.empty()) {
		// Get the next node to visit
		current_node = to_visit.front();
		to_visit.pop();
		for (auto it = current_node.edge_begin(); 
			 it != current_node.edge_end(); ++it) {

			// Find the adjacent node
			incident_edge = *it;
			if( incident_edge.node1() == current_node )
				adjacent_node = incident_edge.node2();
			else 
				adjacent_node = incident_edge.node1();

			// If this node has not been visited before, push it on the 
			// "to visit" list
			if ( !adjacent_node.value().perm ) {
				double dist = norm(current_node.position() - 
											adjacent_node.position());
				adjacent_node.value().dist = dist+current_node.value().dist;

				// Determine if this is the longest path
				if (adjacent_node.value().dist > longest_path) {
					longest_path = adjacent_node.value().dist;
				}

				// Push the adjacent_node on the queue of "to visit" nodes
				if( !adjacent_node.value().temp ) {
					adjacent_node.value().temp = true;
					to_visit.push(adjacent_node);
				}
			}
		}
		// Mark this node as visited
		current_node.value().perm = true;
		current_node.value().temp = false;
	}
	return longest_path;
}



int main(int argc, char** argv)
{
  // Check arguments
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " NODES_FILE TETS_FILE\n";
    exit(1);
  }

  // Construct a Graph
  GraphType graph;
  std::vector<GraphType::node_type> nodes;

  // Create a nodes_file from the first input argument
  std::ifstream nodes_file(argv[1]);
  // Interpret each line of the nodes_file as a 3D Point and add to the Graph
  Point p;
  while (CS207::getline_parsed(nodes_file, p))
    nodes.push_back(graph.add_node(p));

  // Create a tets_file from the second input argument
  std::ifstream tets_file(argv[2]);
  // Interpret each line of the tets_file as four ints which refer to nodes
  std::array<int,4> t;
  while (CS207::getline_parsed(tets_file, t))
    for (unsigned i = 1; i < t.size(); ++i)
      for (unsigned j = 0; j < i; ++j)
        graph.add_edge(nodes[t[i]], nodes[t[j]]);

  // Print out the stats
  std::cout << graph.num_nodes() << " " << graph.num_edges() << std::endl;

  // Launch the SDLViewer
  CS207::SDLViewer viewer;
  viewer.launch();

  // Create empty node map
  auto node_map = viewer.empty_node_map(graph);

  // Use shortest_path_lengths to set the node values to the path lengths
  double longest_path = shortest_path_lengths(graph, Point(-1, 0, 1));

  // Construct a Color functor and view with the SDLViewer
  viewer.add_nodes(graph.node_begin(), 
  				   graph.node_end(), 
				   MyColorFunc(longest_path), 
				   node_map);
  viewer.add_edges(graph.edge_begin(), graph.edge_end(), node_map);
  viewer.center_view();
  return 0;
}
