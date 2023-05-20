#ifndef GRAPHMODEL_H
#define GRAPHMODEL_H
#define NODE_DIMEN 100

#include <QObject>
#include <QDebug>
#include <QuickQanava>

class GraphModel : public QObject
{
	Q_OBJECT
  public:
	explicit GraphModel(QObject *parent = nullptr);
	QPointer<qan::Graph> getGraphElement();
	QMap<QString, QPointer<qan::Node>>* getNodes();
	QMap<QString, QPointer<qan::Edge>>* getEdges();

	void setGraphElement(QPointer<qan::Graph>);
	void setNodeMap(QMap<QString, QPointer<qan::Node>>*);
	void setEdgeMap(QMap<QString, QPointer<qan::Edge>>*);

	void clearGraph();

	Q_INVOKABLE bool readFromFile(QUrl fileUrl);
	Q_INVOKABLE bool saveToFile(QUrl fileUrl);
	Q_INVOKABLE void addNode();


  signals:
	void nodesChanged();
	void edgesChanged();

  public slots:

  private:
	QPointer<qan::Graph> m_graphElement;
	QMap<QString, QPointer<qan::Node>> m_nodeMap;
	QMap<QString, QPointer<qan::Edge>> m_edgeMap;

	QString getNodeId(QPointer<qan::Node> targetNode);
};

#endif // CUSTOMGRAPH_H
