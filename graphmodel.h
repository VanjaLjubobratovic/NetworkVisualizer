#ifndef GRAPHMODEL_H
#define GRAPHMODEL_H

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

	void setGraphElement(QPointer<qan::Graph>);
	void setNodeMap(QMap<QString, QPointer<qan::Node>>*);

	Q_INVOKABLE void addNode();


  signals:
	void nodesChanged();
	void edgesChanged();

  public slots:

  private:
	QPointer<qan::Graph> m_graphElement;
	QMap<QString, QPointer<qan::Node>> m_nodeMap;
};

#endif // CUSTOMGRAPH_H
