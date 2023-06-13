#ifndef GRAPHMODEL_H
#define GRAPHMODEL_H

#define NODE_DIMEN 80
#define NODE_RADIUS (NODE_DIMEN / 2)
#define ID_LENGTH 3

#include <QObject>
#include <QDebug>
#include <QuickQanava>

#include <QPainter>
#include <QPainterPath>

#include "nodewrapper.h"

class GraphModel : public QObject
{
	Q_OBJECT
  public:
	explicit GraphModel(QObject *parent = nullptr);
	QPointer<qan::Graph> getGraphElement();
	QHash<QString, QPointer<NodeWrapper>>* getNodes();
	QHash<QString, QPointer<qan::Edge>>* getEdges();

	void setGraphElement(QPointer<qan::Graph>);
	void setGraphView(QPointer<qan::GraphView>);
	void setNodeMap(QHash<QString, QPointer<NodeWrapper>>*);
	void setEdgeMap(QHash<QString, QPointer<qan::Edge>>*);


	Q_INVOKABLE void clearGraph();
	Q_INVOKABLE bool readFromFile(QUrl fileUrl);
	Q_INVOKABLE bool saveToFile(QUrl fileUrl);
	Q_INVOKABLE void readyToInsertNode(bool active, bool malicious);
	Q_INVOKABLE void removeSelected();
	Q_INVOKABLE void forceDirectedLayout();
	Q_INVOKABLE void toggleDrawing();
	Q_INVOKABLE QString getNodeInfo(qan::Node * n);
	Q_INVOKABLE QString getNetworkInfo();



  signals:
	void nodesChanged();
	void edgesChanged();

  public slots:
	void onDrawNewEdge(QPointer<qan::Edge> e);
	void onDrawNewNode(QVariant pos);

  private:
	QPointer<qan::Graph> m_graphElement;
	QPointer<qan::GraphView> m_graphView;
	QHash<QString, QPointer<NodeWrapper>> m_nodeMap;
	QHash<QString, QPointer<qan::Edge>> m_edgeMap;
	bool m_loading = false;
	bool m_addingNode = false;

	QString getNodeId(QPointer<qan::Node> targetNode);
	QString getEdgeId(QPointer<qan::Edge> targetEdge);
	bool edgeExists(QPointer<qan::Edge> targetEdge);

	void forceDirectedLayout(QList<qan::Node*> nodeList, QList<qan::Edge*> edgeList);
	QPointF getNodeCenter(QPointer<qan::Node>);

	template<typename T>
	QString generateUID(const QHash<QString, QPointer<T>> &container);

	bool newActive = true, newMalicious = false;
};

#endif // CUSTOMGRAPH_H
