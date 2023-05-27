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

class GraphModel : public QObject
{
	Q_OBJECT
  public:
	explicit GraphModel(QObject *parent = nullptr);
	QPointer<qan::Graph> getGraphElement();
	QHash<QString, QPointer<qan::Node>>* getNodes();
	QHash<QString, QPointer<qan::Edge>>* getEdges();

	void setGraphElement(QPointer<qan::Graph>);
	void setGraphView(QPointer<qan::GraphView>);
	void setNodeMap(QHash<QString, QPointer<qan::Node>>*);
	void setEdgeMap(QHash<QString, QPointer<qan::Edge>>*);


	Q_INVOKABLE void clearGraph();
	Q_INVOKABLE bool readFromFile(QUrl fileUrl);
	Q_INVOKABLE bool saveToFile(QUrl fileUrl);
	Q_INVOKABLE void drawNewNode(const QString label = "New Node");
	Q_INVOKABLE void removeSelected();


  signals:
	void nodesChanged();
	void edgesChanged();

  public slots:
	void onDrawNewEdge(QPointer<qan::Edge> e);

  private:
	QPointer<qan::Graph> m_graphElement;
	QPointer<qan::GraphView> m_graphView;
	QHash<QString, QPointer<qan::Node>> m_nodeMap;
	QHash<QString, QPointer<qan::Edge>> m_edgeMap;
	bool m_loading = false;

	QString getNodeId(QPointer<qan::Node> targetNode);
	QString getEdgeId(QPointer<qan::Edge> targetEdge);
	bool edgeExists(QPointer<qan::Edge> targetEdge);

	void setNodeStyle(QPointer<qan::Node> n);

	void forceDirectedLayout(QList<qan::Node*> nodeList, QList<qan::Edge*> edgeList);

	template<typename T>
	QString generateUID(const QHash<QString, QPointer<T>> &container);
};

#endif // CUSTOMGRAPH_H
