#include "graphmodel.h"

GraphModel::GraphModel(QObject *parent)
	: QObject{parent}
{

}

void GraphModel::setGraphElement(QPointer<qan::Graph> graph)
{
	m_graphElement = graph;
}

void GraphModel::addNode() {
	qan::Node* n = m_graphElement->insertNode();
	n->setLabel("New node");
	n->getItem()->setRect({0, 0, 100, 100});

	m_nodeMap[n->getLabel()] = n;
}
