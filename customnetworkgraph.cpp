#include "customnetworkgraph.h"
#include "customnetworknode.h"
#include "customnetworkedge.h"

qan::Node*  CustomNetworkGraph::insertCustomNode()
{
	return qan::Graph::insertNode<CustomNetworkNode>();
}

qan::Edge*  CustomNetworkGraph::insertCustomEdge(qan::Node* source, qan::Node* destination)
{
	const auto engine = qmlEngine(this);
	if ( source != nullptr && destination != nullptr && engine != nullptr )
		return qan::Graph::insertEdge<CustomNetworkEdge>(*source, destination, CustomNetworkEdge::delegate(*engine) );
	return nullptr;
}
