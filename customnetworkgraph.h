#ifndef CUSTOMNETWORKGRAPH_H
#define CUSTOMNETWORKGRAPH_H

#include <QObject>
#include <QQmlEngine>
#include <QuickQanava>
#include "customnetworknode.h"

class CustomNetworkGraph : public qan::Graph
{
	Q_OBJECT
  public:
	explicit CustomNetworkGraph(QQuickItem* parent = nullptr) : qan::Graph(parent) { /* Nil */ }
	virtual ~CustomNetworkGraph() override = default;
	CustomNetworkGraph(const CustomNetworkGraph&) = delete;

  public:
	Q_INVOKABLE qan::Node*  insertCustomNode();
	Q_INVOKABLE qan::Edge*  insertCustomEdge(qan::Node* source, qan::Node* destination);
};

QML_DECLARE_TYPE(CustomNetworkGraph)

#endif // CUSTOMNETWORKGRAPH_H
