#include <vector>
#include <stack>
#include <set>
#include "Point.hpp"
#include "error.hpp"
#include "debug.hpp"

template<typename NodeData, typename EdgeData>
class Graph {

	// Type aliases for common indices and identifiers
	typedef int nid_type;
	typedef int eid_type;
	typedef int idx_type;

	/** Stores information about a node. 
	 * RI: If directed, then outgoing_edges and incoming_edges refer to 
	 * 	the directed edges pointing away from and towards the node.
	 * RI: If graph is undirected, then only outgoing_edges is populated, 
	 * 	and it is assumed that all outgoing_edges are also incoming_edges.
	 * The data field stores user data.
	 */
	struct NodeInfo {
		idx_type idx;

		std::set<eid_type> outgoing_edges;
		std::set<eid_type> incoming_edges;

		mutable Point position;
		mutable NodeData data;

		// Sets all elements to their default values
		void clear() {
			idx = idx_type();
			outgoing_edges.clear();
			incoming_edges.clear();
			position = Point();
			data = NodeData();
		}

		NodeInfo(idx_type idx, Point pos, NodeData data) :
				idx(idx), position(pos), data(data) {}
	};

	/** Stores information about an edge. 
	 * RI: If directed, then edge points from nid1 -> nid2
	 * The data field stores user specified data.
	 */
	struct EdgeInfo {
		idx_type idx;
		nid_type nid1;
		nid_type nid2;
		mutable EdgeData data;

		// Sets all elements to their default value
		void clear() {
			idx = idx_type();
			nid1 = nid_type();
			nid2 = nid_type();
			data = EdgeData();
		}

		EdgeInfo(idx_type i, nid_type n1, nid_type n2, EdgeData v) :
					idx(i), nid1(n1), nid2(n2), data(v) {}
	};

	public:

	//////////////////////////////////////////////////////////////////
	///// DEFINE TYPES AND CONSTRUCTOR ///////////////////////////////
	//////////////////////////////////////////////////////////////////

	// Declare classes
	class Node; 
	class Edge;
	template<typename IT, typename T>
	class TransformIter;

	// Define value types
	typedef Node node_type;
	typedef Edge edge_type;
	typedef NodeData node_value_type;
	typedef EdgeData edge_value_type;

	// Declare iterator types
	typedef std::vector<nid_type> nid_list;
	typedef std::vector<nid_type> eid_list;
	typedef std::set<eid_type> eid_set;
	typedef TransformIter<nid_list::iterator, node_type> node_iterator;
	typedef TransformIter<eid_list::iterator, edge_type> edge_iterator;
	typedef TransformIter<eid_set::iterator, edge_type> incident_iterator;
	
	// Constructor. Allows for creation of directed or undirected graph.
	Graph(bool directed = false) : directed_(directed) {
	}

	// Clears the graph of all nodes and edges.
	void clear() {
		clear_data_();
	}


	//////////////////////////////////////////////////////////////////
	///// DEFINE SUBCLASSES AND FUNCTIONS ////////////////////////////
	//////////////////////////////////////////////////////////////////

	class Node : private totally_ordered<Node> {
		public:

			// Construct an invalid Node
			Node() : g_(NULL), nid_(-1) {
			}

			const Point& position() const {
				return g_->nodes_[nid_].position;
			}

			Point& position() {
				return g_->nodes_[nid_].position;
			}

			const node_value_type& value() const {
				return g_->nodes_[nid_].data;
			}

			node_value_type& value() {
				return g_->nodes_[nid_].data;
			}

			idx_type index() {
				return g_->nodes_[nid_].idx;
			}

			// By default, iterates over the outgoing edges in a directed
			// graph. In an undirected graph, iterates over all adjacent
			// edges to the node.
			incident_iterator edge_begin() {
				return incident_iterator(
							g_, g_->nodes_[nid_].outgoing_edges.begin());
			}

			incident_iterator edge_end() {
				return incident_iterator(
							g_, g_->nodes_[nid_].outgoing_edges.end());
			}

			// Returns the degree of the node, the number of edges
			// adjacent to the node in an undirected graph, and the 
			// number of directed edges in a directed graph
			size_t degree() const {
				return g_->nodes_[nid_].outgoing_edges.size();
			}

			// In a directed graph, iterates over outgoing edges from a
			// node.
			incident_iterator outgoing_edge_begin() {
				return edge_begin();
			}

			incident_iterator outgoing_edge_end() {
				return edge_end();
			}

			// In a directed graph, iterates over incoming edges to a node.
			incident_iterator incoming_edge_begin() {
				return incident_iterator(
							g_, g_->nodes_[nid_].incoming_edges.begin());
			}

			incident_iterator incoming_edge_end() {
				return incident_iterator(
							g_, g_->nodes_[nid_].incoming_edges.end());
			}

			bool operator<(const Node& other) const {
				return g_->nodes_[nid_].idx < g_->nodes_[other.nid_].idx;
			}

			bool operator==(const Node& other) const {
				return nid_ == other.nid_ && g_ == other.g_;	
			}

		private:
			friend class Graph;
			Node(const Graph* g, nid_type nid) : g_(g), nid_(nid) {}
			const Graph* g_;
			int nid_;
	};

	/** Computes the number of nodes in the graph. 
	 * @returns the number of nodes in the graph
	 */
	size_t num_nodes() {
		return idx2nid_.size();
	}

	size_t size() {
		return num_nodes();
	}

	/** Adds a node to the graph.  
	 * @param[in] @a p is the position of the node to be added
	 * @param[in] @a v is the value of node_value_type that will 
	 * 	be associated with the node at position @a p
	 * @returns the new node with position @a p
	 * @post new size() == old size() + 1
	 */
	node_type add_node(Point p, 
					   node_value_type v = node_value_type()) {
		// Declare variables
		int nid;
		int idx;

		nid = nodes_.size();
		idx = num_nodes();
		idx2nid_.push_back(nid);
		nodes_.push_back(NodeInfo(idx, p, v));

		return Node(this, nid);
	}

	// Returns the node indexed by index i
	node_type node(idx_type idx) {
		nid_type nid = idx2nid_[idx];
		return Node(this, nid);
	}

	// Removes a node from the graph. Invalidates all node iterators.
	void remove_node(Node n) {
		nid_type nid = n.nid_;
		// Remove all outgoing edges associated with this node.
		for(auto it = nodes_[nid].outgoing_edges.begin(); 
			it != nodes_[nid].outgoing_edges.end(); ++it) {
			remove_edge(Edge(this, *it));
		}
		// Remove all incoming edges associated with this node.
		for(auto it = nodes_[nid].incoming_edges.begin(); 
			it != nodes_[nid].incoming_edges.end(); ++it) {
			remove_edge(Edge(this, *it));
		}
		// Remove the edge from the index list.
		idx_type idx = nodes_[nid].idx;
		for(auto it = idx2nid_.begin()+idx; it != idx2nid_.end(); ++it) {
			nid_type id = *it;
			--nodes_[id].idx;
		}
		idx2nid_.erase(idx2nid_.begin() + idx);
		nodes_[nid].clear();
	}

	class Edge : private totally_ordered<Edge> {
		public: 
			// Default constructor
			Edge() : g_(NULL), eid_(-1) {
			}

			node_type node1() {
				return node_type(g_, g_->edges_[eid_].nid1);
			}

			node_type node2() {
				return node_type(g_, g_->edges_[eid_].nid2);
			}

			const edge_value_type& value() const {
				return g_->edges_[eid_].data;
			}

			edge_value_type& value() {
				return g_->edges_[eid_].data;
			}

			idx_type index() {
				return g_->edges_[eid_].idx;
			}

			bool operator<(const Edge& other) const {
				return g_->edges_[eid_].idx < g_->edge[other.eid_].idx;
			}

			bool operator==(const Edge& other) const {
				return g_->edges_[eid_].idx == g_->edge[other.eid_].idx;
			}
		private:
			friend class Graph;
			Edge(const Graph* g, eid_type eid) : g_(g), eid_(eid) {}
			const Graph* g_;
			int eid_;
	};

	/** Returns the number of edges in the graph
	 */
	size_t num_edges() {
		return idx2eid_.size();
	}

	// Adds the edge to the graph. If the edge already exists, returns the
	// 	already existing edge.
	edge_type add_edge(const node_type& n1, 
					   const node_type& n2,
					   edge_value_type v = edge_value_type()) {
		eid_type eid;
		eid_type eid_check;
		idx_type idx;

		if( (eid_check = has_edge(n1, n2)) >= 0 ) 
			return Edge(this, eid_check);

		idx = num_edges();
		// TODO: branch here to handle the reuse of edge id's
		eid = edges_.size();
		idx2eid_.push_back(eid);

		if( directed_ ) {
			edges_.push_back(EdgeInfo(idx, n1.nid_, n2.nid_, v));
			nodes_[n1.nid_].outgoing_edges.emplace(eid);
			nodes_[n2.nid_].incoming_edges.emplace(eid);
		}
		else {
			edges_.push_back(EdgeInfo(idx, n1.nid_, n2.nid_, v));
			nodes_[n1.nid_].outgoing_edges.emplace(eid);
			nodes_[n2.nid_].outgoing_edges.emplace(eid);
		}
		return Edge(this, eid);
	}

	// Returns the eid if the graph has the edge specified by n1 and n2, 
	// 	else returns -1.
	// If the graph is a directed graph, then the directed edge is 
	// interpreted as n1 -> n2 and the function only returns true if there
	// is a directed edge from node n1 to node n2
	eid_type has_edge(const node_type& n1, const node_type& n2) {
		// Check to make sure that the nids are valid.
		if( !(0 <= n1.nid_ && n1.nid_ < (int) nodes_.size() && 
			  0 <= n2.nid_ && n2.nid_ < (int) nodes_.size()) )
			return -1;
		return has_edge_(n1.nid_, n2.nid_);

	}

	// Removes an edge from the graph. Invalidates all edge iterators.
	// @returns true if the removal was successful
	bool remove_edge(Edge e) {
		eid_type eid = e.eid_;
		// Make sure that the eid of the edge is valid.
		if(  !(0 <= eid && eid < (int) edges_.size())  )
			return false;
		// Make sure that the edges exists.
		nid_type nid1 = edges_[eid].nid1;
		nid_type nid2 = edges_[eid].nid2;
		if( has_edge_(nid1, nid2) < 0 )
			return false;
		// Remove the edge from the adjacency lists.
		nodes_[nid1].outgoing_edges.erase(eid);
		nodes_[nid2].outgoing_edges.erase(eid);
		nodes_[nid1].incoming_edges.erase(eid);
		nodes_[nid2].incoming_edges.erase(eid);
		// Remove the edge from the index list.
		idx_type idx = edges_[eid].idx;
		for(auto it=idx2eid_.begin()+idx; it != idx2eid_.end(); ++it) {
			eid_type id = *it;
			--edges_[id].idx;
		}
		idx2eid_.erase(idx2eid_.begin() + idx);
		edges_[eid].clear();
		return true;
	}

	/** Returns an iterator to the beginning of the edge list for the graph.
	 * RI: If the graph is a directed graph, then the edge iterator will
	 * 	iterate over all directed edges in the graph such that if x, y are
	 * 	in the set of vertices, (x, y) is considered to be a different edge
	 * 	than (y, x)
	 * RI: If the graph is an undirected graph, then the edge iterator will
	 * 	only iterate over each undirected edge once. 
	 */
	edge_iterator edge_begin() {
		return edge_iterator(this, idx2eid_.begin());
	}

	edge_iterator edge_end() {
		return edge_iterator(this, idx2eid_.end());
	}

	template<typename IT, typename T>
	class TransformIter : private equality_comparable<TransformIter<IT, T>>{
		public: 
			// Types that help us use STL's iterator traits
			typedef T value_type;
			typedef T* pointer;
			typedef T& reference;
			typedef std::input_iterator_tag iterator_category;
			typedef std::ptrdiff_t difference_type;

			TransformIter() : g_(NULL), it_(IT()) {
			}

			value_type operator*() {
				return value_type(g_, *it_);
			}

			TransformIter& operator++() {
				++it_;
				return *this;
			}

			bool operator==(const TransformIter& other) const {
				return it_ == other.it_;
			}

		private:
			friend class Graph;
			TransformIter(const Graph* g, IT it) : it_(it), g_(g) {}
			IT it_;
			const Graph* g_;
	};

	/** Returns an iterator to the first node in the graph. 
	 */
	node_iterator node_begin() {
		return node_iterator(this, idx2nid_.begin());
	}

	node_iterator node_end() {
		return node_iterator(this, idx2nid_.end());
	}

	private:

	/////////////////////////////////////////////////////////////////////
	///// PRIVATE MEMBERS ///////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

	// True if the graph is a directed graph
	const bool directed_;

	// Index and ID mappings for nodes
	std::vector<NodeInfo> nodes_;
	std::vector<nid_type> idx2nid_;

	// Index and ID mappings for edges
	std::vector<EdgeInfo> edges_;
	std::vector<eid_type> idx2eid_;

	// Stacks that contain info about deleted nid's and eid's
	std::stack<nid_type> free_nids_;
	std::stack<eid_type> free_eids_;

	//////////////////////////////////////////////////////////////////////
	///// HELPER FUNCTIONS ///////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	// Clears all the member data of the graph.
	void clear_data_() {
		nodes_.clear();
		idx2nid_.clear();
		edges_.clear();
		idx2eid_.clear();
	}

	// Returns the eid of the edge if there is an edge from the node 
	// represented by nid1 to the node, nid2. Returns -1 if there is 
	// no such edge. 
	// @pre nid1 and nid2 must be 0 <= nid1, nid2 < nodes_.size()
	eid_type has_edge_(const nid_type nid1, const nid_type nid2) {
		assert( 0 <= nid1 && nid1 < (int) nodes_.size() );
		assert( 0 <= nid2 && nid2 < (int) nodes_.size() );
		auto it = nodes_[nid1].outgoing_edges.begin();
		while(1) {
			if( it == nodes_[nid1].outgoing_edges.end() )
				return -1;
			else if( edges_[*it].nid2 == nid2 ) {
				eid_type eid = *it;
				return eid;
			}
			else
				++it;
		}
	}
};
