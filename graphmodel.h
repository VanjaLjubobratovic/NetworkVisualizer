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

#include "customnetworkedge.h"
#include "customnetworkgraph.h"
#include "customnetworknode.h"

class GraphModel : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool newActive READ isNewActive WRITE setNewActive NOTIFY newActiveChanged)
	Q_PROPERTY(bool newMalicious READ isNewMalicious WRITE setNewMalicious NOTIFY newMaliciousChanged)
	Q_PROPERTY(bool addingNode READ isAddingNode WRITE setAddingNode NOTIFY addingNodeChanged)
  public:
	explicit GraphModel(QObject *parent = nullptr);
	QPointer<qan::Graph> getGraphElement();
	QHash<QString, QPointer<CustomNetworkNode>>* getNodes();
	QHash<QString, QPointer<qan::Edge>>* getEdges();

	//void setGraphElement(QPointer<qan::Graph>);
	void setGraphElement(QPointer<CustomNetworkGraph>);
	void setGraphView(QPointer<qan::GraphView>);
	void setNodeMap(QHash<QString, QPointer<CustomNetworkNode>>*);
	void setEdgeMap(QHash<QString, QPointer<qan::Edge>>*);

	bool isNewActive() const;
	bool isNewMalicious() const;
	bool isAddingNode() const;
	void setNewActive(bool active);
	void setNewMalicious(bool malicious);
	void setAddingNode(bool adding);


	Q_INVOKABLE void clearGraph();
	Q_INVOKABLE bool readFromFile(QUrl fileUrl);
	Q_INVOKABLE bool saveToFile(QUrl fileUrl);
	Q_INVOKABLE void removeSelected();
	Q_INVOKABLE void forceDirectedLayout();
	Q_INVOKABLE void toggleDrawing();
	Q_INVOKABLE QString getNodeInfo(qan::Node * n);
	Q_INVOKABLE QString getNetworkInfo();



  signals:
	void nodesChanged();
	void edgesChanged();
	void newActiveChanged();
	void newMaliciousChanged();
	void addingNodeChanged();

  public slots:
	void onDrawNewEdge(QPointer<qan::Edge> e);
	void onDrawNewNode(QVariant pos);

	//TCP Handlers
	void handleNewConnection();
	void handleSocketData();
	void handleInsertNodeCommand(const QJsonObject nodeObj, const QTcpSocket* socket);
	void handleRemoveNodeCommand(const QJsonObject nodeObj, const QTcpSocket* socket);
	void handleInsertEdgeCommand(const QJsonObject edgeObj, const QTcpSocket* socket);
	void handleRemoveEdgeCommand(const QJsonObject edgeObj, const QTcpSocket* socket);
	void handleInsertFileCommand(const QJsonObject fileObj, const QTcpSocket* socket);
	void handleRemoveFileCommand(const QJsonObject fileObj, const QTcpSocket* socket);
	void handleSetActiveCommand(const QJsonObject payload, const QTcpSocket* socket);
	void handleSetMaliciousCommand(const QJsonObject payload, const QTcpSocket* socket);

  private:
	QPointer<CustomNetworkGraph> m_graphElement;
	QPointer<qan::GraphView> m_graphView;
	QHash<QString, QPointer<CustomNetworkNode>> m_nodeMap;
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

	QPointer<QTcpServer> tcpServer;
};

#endif // CUSTOMGRAPH_H
