#ifndef GRAPHMODEL_H
#define GRAPHMODEL_H
#define NODE_DIMEN 100
#define ID_LENGTH 3

#include <QObject>
#include <QDebug>
#include <QuickQanava>

class GraphModel : public QObject
{
	Q_OBJECT
  public:
	explicit GraphModel(QObject *parent = nullptr);
	QPointer<qan::Graph> getGraphElement();
	QHash<QString, QPointer<qan::Node>>* getNodes();
	QHash<QString, QPointer<qan::Edge>>* getEdges();

	void setGraphElement(QPointer<qan::Graph>);
	void setNodeMap(QHash<QString, QPointer<qan::Node>>*);
	void setEdgeMap(QHash<QString, QPointer<qan::Edge>>*);

	Q_INVOKABLE void clearGraph();
	Q_INVOKABLE bool readFromFile(QUrl fileUrl);
	Q_INVOKABLE bool saveToFile(QUrl fileUrl);
	Q_INVOKABLE void drawNewNode(const QString label = "New Node");


  signals:
	void nodesChanged();
	void edgesChanged();

  public slots:
	void onDrawNewEdge(QPointer<qan::Edge> e);

  private:
	QPointer<qan::Graph> m_graphElement;
	QHash<QString, QPointer<qan::Node>> m_nodeMap;
	QHash<QString, QPointer<qan::Edge>> m_edgeMap;

	QString getNodeId(QPointer<qan::Node> targetNode);
	QString getEdgeId(QPointer<qan::Edge> targetEdge);
	bool edgeExists(QPointer<qan::Edge> targetEdge);

	template<typename T>
	QString generateUID(const QHash<QString, QPointer<T>> &container);
};

#endif // CUSTOMGRAPH_H
