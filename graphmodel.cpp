#include "graphmodel.h"
#include <cmath>
#include <QQuickItem>

#include <QTcpSocket>
#include <QTcpServer>


GraphModel::GraphModel(QObject *parent)
	: QObject{parent}
{
}

void GraphModel::setGraphElement(QPointer<CustomNetworkGraph> graph) {
	m_graphElement = graph;
	QObject::connect(m_graphElement, &qan::Graph::edgeInserted, this, &GraphModel::onDrawNewEdge);

	tcpServer = new QTcpServer(this);
	QObject::connect(tcpServer, &QTcpServer::newConnection, this, &GraphModel::handleNewConnection);

	if(!tcpServer->listen(QHostAddress::LocalHost, 1234)) {
		qDebug() << "Failed to start server";
		return;
	}

	qDebug() << "Server started, waiting for connections...";

 //TODO: change to this; This won't trigger onDrawNew edge while loading from JSON, only while drawing via UI
 //QObject::connect(m_graphElement, &qan::Graph::connectorEdgeInserted, this, &GraphModel::onDrawNewEdge);
}

void GraphModel::setGraphView(QPointer<qan::GraphView> gw){
	m_graphView = gw;
	m_graphView->getGrid()->setVisible(false);
	QObject::connect(m_graphView, &qan::GraphView::clicked, this, &GraphModel::onDrawNewNode);
}

bool GraphModel::isNewActive() const {
	return newActive;
}

bool GraphModel::isNewMalicious() const {
	return newMalicious;
}

bool GraphModel::isAddingNode() const {
	return m_addingNode;
}

void GraphModel::setNewActive(bool active) {
	if(newActive == active)
		return;

	newActive = active;
	emit newActiveChanged();
}

void GraphModel::setNewMalicious(bool malicious) {
	if(newMalicious == malicious)
		return;
	newMalicious = malicious;
	emit newMaliciousChanged();
}

void GraphModel::setAddingNode(bool adding) {
	if(m_addingNode == adding)
		return;
	m_addingNode = adding;
	emit addingNodeChanged();
}

void GraphModel::removeSelected() {
	auto& selectedNodes = m_graphElement->getSelectedNodes();
	auto& selectedEdges = m_graphElement->getSelectedEdges();

	if(!(selectedNodes.size() + selectedEdges.size())) {
		qDebug() << "GraphModel::removeSelected() -> No elements selected, ignoring...";
		return;
	}

	for(auto node : selectedNodes) {
		QString key = getNodeId(node);
		m_nodeMap.remove(key);
		m_graphElement->removeNode(node);
	}

	for(auto edge : selectedEdges) {
		QString key = getEdgeId(edge);
		m_edgeMap.remove(key);
		m_graphElement->removeEdge(edge);
	}

	// Deleting "dangling edges" left after node deletion
	for(auto key : m_edgeMap.keys()) {
		if (!m_edgeMap.value(key)) {
			qDebug() << "Removing dangling edge:" << key;
			m_edgeMap.remove(key);
		}
	}
}


void GraphModel::clearGraph() {
	m_graphElement->clearGraph();
	m_nodeMap.clear();
	m_edgeMap.clear();
}

bool GraphModel::readFromFile(QUrl fileUrl) {
	QFile file(fileUrl.path());
	if(!file.open(QIODevice::ReadOnly)) {
		qDebug() << "Failed to open graph JSON" << file.errorString();
		return false;
	}

	QByteArray jsonData = file.readAll();
	QJsonParseError parseError;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

	if(parseError.error != QJsonParseError::NoError) {
		qDebug() << "Failed to parse JSON" << parseError.error;
		return false;
	}

	if(!jsonDoc.isObject()) {
		qDebug() << "Top level JSON element is not an object";
		return false;
	}

	/* insertEdge() function triggers the same signal
	 * like drawing in UI does so the slot function gets
	 * called and adds a duplicate edge.
	 * m_loading mitigates that.
	 */
	m_loading = true;

	QJsonObject jsonObj = jsonDoc.object();

	GraphModel::clearGraph();

	//Reading nodes
	if(jsonObj.contains("nodes")) {
		QJsonObject nodesObj = jsonObj["nodes"].toObject();

		for(const auto &nodeKey : nodesObj.keys()) {
			QJsonObject nodeObj = nodesObj[nodeKey].toObject();

			auto n = CustomNetworkNode::nodeFromJSON(m_graphElement, nodeObj);

			m_nodeMap[nodeKey] = n;
		}
	} else {
		qDebug() << "JSON document contains no nodes";
	}

	//Reading edges
	if(jsonObj.contains("edges")) {
		QJsonObject edgesObj = jsonObj["edges"].toObject();

		for(const auto &edgeKey : edgesObj.keys()) {
			QJsonObject edgeObj = edgesObj[edgeKey].toObject();
			QString to = edgeObj["to"].toString();
			QString from = edgeObj["from"].toString();

			auto e = m_graphElement->insertEdge(m_nodeMap[from], m_nodeMap[to]);
			e->getItem()->setDstShape(qan::EdgeStyle::ArrowShape::None);

			//App treats all edges as undirected while QuickQanava doesn't
			if(!edgeExists(e)) {
				m_edgeMap[edgeKey] = e;
			} else if (e) {
				m_graphElement->removeEdge(e);
			}
		}
	} else {
		qDebug() << "JSON document contains no edges";
	}

	m_loading = false;

	//Calculating layout
	forceDirectedLayout(m_graphElement->get_nodes(), m_graphElement->get_edges());

	return true;

}

bool GraphModel::saveToFile(QUrl fileUrl) {
	QFile file(fileUrl.path());

	if(file.exists()) {
		if(!file.remove()) {
			qDebug() << "Failed to remove existing file:" << file.errorString();
			return false;
		}
	}

	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "Failed to open file for writing:" << file.errorString();
		return false;
	}

	QJsonObject mainObject;

	//Writing nodes
	QJsonObject nodesObj;
	for(const auto &nodeKey: m_nodeMap.keys()) {
		QJsonObject nodeObj = CustomNetworkNode::nodeToJSON(m_nodeMap[nodeKey]);
		nodesObj.insert(nodeKey, nodeObj);
	}

	mainObject.insert("nodes", nodesObj);

	//Writing edges
	QJsonObject edgesObj;
	for(const auto edgeKey : m_edgeMap.keys()) {
		QJsonObject edgeObj;

		//fetch node keys
		edgeObj.insert("from", getNodeId(m_edgeMap[edgeKey]->getSource()));
		edgeObj.insert("to", getNodeId(m_edgeMap[edgeKey]->getDestination()));
		edgeObj.insert("data", *(new QJsonObject()));

		edgesObj.insert(edgeKey, edgeObj);
	}

	mainObject.insert("edges", edgesObj);

	QJsonDocument jsonDoc;
	jsonDoc.setObject(mainObject);

	QTextStream stream(&file);
	stream << jsonDoc.toJson(QJsonDocument::Indented);

	return true;
}

void GraphModel::onDrawNewNode(const QVariant pos) {
	if(!m_addingNode)
		return;

	qDebug() << pos;
	QPointF point(pos.toPoint());
	point -= QPointF(NODE_DIMEN/2, NODE_DIMEN/2); //Node center is put where user clicks

	//Transforming from window coordinate system to graph coordinate system
	QPointF transformedPoint = m_graphView->mapToItem(m_graphView->getContainerItem(), point);

	QString id = generateUID(m_nodeMap);

	//QPointer<qan::Node> n = m_graphElement->insertNode();
	auto n = dynamic_cast<CustomNetworkNode*>(m_graphElement->insertCustomNode());
	n->setLabel("New node");
	n->getItem()->setRect({transformedPoint.x(), transformedPoint.y(), NODE_DIMEN, NODE_DIMEN});
	n->setActive(newActive);
	n->setMalicious(newMalicious);
	n->setId(id);

	m_nodeMap.insert(id, n);

	m_addingNode = false;

	qDebug() << "New node inserted:" << id;
}

void GraphModel::handleNewConnection() {
	QTcpSocket* clientSocket = tcpServer->nextPendingConnection();
	QObject::connect(clientSocket, &QTcpSocket::readyRead, this, &GraphModel::handleSocketData);

	qDebug() << "New client connected.";

	QString welcomeMsg = "Welcome to NetworkVisualizer server!";
	clientSocket->write(welcomeMsg.toUtf8());
}

void GraphModel::handleSocketData() {
	QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
	QByteArray data = clientSocket->readAll();

	QJsonDocument document = QJsonDocument::fromJson(data);
	QString command = document.object().value("command").toString();
	QJsonObject payload = document.object().value("payload").toObject();

	qDebug() << command << payload;

	if(command == "insertNode") {
		handleInsertNodeCommand(payload, clientSocket);
	} else if(command == "removeNode") {
		handleRemoveNodeCommand(payload, clientSocket);
	} else if (command == "insertEdge") {
		handleInsertEdgeCommand(payload, clientSocket);
	} else if (command == "removeEdge") {
		handleRemoveEdgeCommand(payload, clientSocket);
	} else if (command == "insertFile") {
		handleInsertFileCommand(payload, clientSocket);
	} else if (command == "removeFile") {
		handleRemoveFileCommand(payload, clientSocket);
	} else if (command == "setActive") {
		handleSetActiveCommand(payload, clientSocket);
	} else if (command == "setMalicious") {
		handleSetMaliciousCommand(payload, clientSocket);
	}

	QString response = "Command " + command + " received and processed";
	clientSocket->write(response.toUtf8());
}

void GraphModel::handleInsertNodeCommand(const QJsonObject nodeObj, const QTcpSocket* socket) {
	auto n = CustomNetworkNode::nodeFromJSON(m_graphElement, nodeObj);
	n->setId(GraphModel::generateUID(m_nodeMap));
	m_nodeMap[n->getID()] = n;

	/*QJsonObject response;
	response.insert("message", "Success");
	response.insert("command", "insertNode");
	response.insert("nodeId", n->getID());*/

	//TODO: write a response
}

void GraphModel::handleRemoveNodeCommand(const QJsonObject nodeObj, const QTcpSocket *socket) {
	QString id = nodeObj.value("nodeId").toString();
	auto n = m_nodeMap[id];

	if(!n) {
		//TODO: write a response
		return;
	}

	m_graphElement->removeNode(n);
	m_nodeMap.remove(id);

	//TODO: send response
}

void GraphModel::handleInsertEdgeCommand(const QJsonObject edgeObj, const QTcpSocket *socket) {
	//TODO:
}

void GraphModel::handleRemoveEdgeCommand(const QJsonObject edgeObj, const QTcpSocket *socket) {
	//TODO:
}

void GraphModel::handleInsertFileCommand(const QJsonObject fileObj, const QTcpSocket *socket) {
	//TODO
}

void GraphModel::handleRemoveFileCommand(const QJsonObject fileObj, const QTcpSocket *socket) {
	//TODO
}

void GraphModel::handleSetActiveCommand(const QJsonObject payload, const QTcpSocket *socket) {
	//TODO
}

void GraphModel::handleSetMaliciousCommand(const QJsonObject payload, const QTcpSocket *socket) {
	//TODO
}



//Slot for when a new edge is drawn via UI
void GraphModel::onDrawNewEdge(QPointer<qan::Edge> e) {
	if(m_loading) {
		qDebug() << "Loading from JSON, ignoring duplicate edges...";
		return;
	}

	if (edgeExists(e)) {
		//Drawing via UI already added duplicate edge to graph here so it must be removed
		qDebug() << "Edge already exists, ignoring...";
		m_graphElement->removeEdge(e);
		return;
	}

	e->getItem()->setDstShape(qan::EdgeStyle::ArrowShape::None);

	QString id = generateUID(m_edgeMap);
	m_edgeMap.insert(id, e);

	qDebug() << "New edge inserted:" << id;
}

//Function for finding a key for a specified node object
//Used when writing edges to JSON
QString GraphModel::getNodeId(QPointer<qan::Node> targetNode) {
	for (auto it = m_nodeMap.begin(); it != m_nodeMap.end(); ++it) {
		if(QPointer<qan::Node>(it.value()) == targetNode) {
			return it.key();
		}
	}
	return nullptr;
}

QString GraphModel::getEdgeId(QPointer<qan::Edge> targetEdge) {
	for (auto it = m_edgeMap.begin(); it != m_edgeMap.end(); ++it) {
		if(it.value() == targetEdge) {
			return it.key();
		}
	}
	return nullptr;
}

bool GraphModel::edgeExists(QPointer<qan::Edge> targetEdge) {
	for (auto it = m_edgeMap.begin(); it != m_edgeMap.end(); ++it) {
		auto e = it.value().get();

		//This is because we pretend edges are bidirectional
		if(((e->getSource() == targetEdge->getSource()) && (e->getDestination() == targetEdge->getDestination())) ||
			((e->getSource() == targetEdge->getDestination()) && (e->getDestination() == targetEdge->getSource()))) {
			return true;
		}
	}
	return false;
}

//QML invokable function without arguments
//I didn't want to change original function signature for this
//I know it's dumb

//TODO: Ok, I don't remember why the hell this is the way it is. Fix it.
void GraphModel::forceDirectedLayout() {
	forceDirectedLayout(m_graphElement->get_nodes(), m_graphElement->get_edges());
}

void GraphModel::toggleDrawing() {
	m_addingNode = false;
}

QString GraphModel::getNodeInfo(qan::Node *n) {
	for(const auto node : m_nodeMap.values()) {
		if(n == node)
			return node->getNodeInfo();
	}

	return QString("Error fetching node info");
}

QString GraphModel::getNetworkInfo(){
	double malicious = 0, active = 0, maliciousActive = 0;
	double maliciousPer, activePer, maliciousActivePer;
	double data = 0;

	for(const auto n : m_nodeMap) {
		if (n->isMalicious()) {
			malicious++;
			if(n->isActive()) {
				maliciousActive++;
			}
		}
		if(n->isActive())
			active++;
	}

	activePer = active / std::max(1.0, double(m_nodeMap.count())) * 100.0;
	maliciousPer = malicious / std::max(1.0, double(m_nodeMap.count())) * 100.0;
	maliciousActivePer = maliciousActive / std::max(1.0, malicious) * 100.0;

	QString info = "<strong>Nodes:</strong> " + QString::number(m_nodeMap.count()) + "<br>"
				   + "<font color=\'#42b4ff'><strong>Active:</strong></font> " + QString::number(active) + " (" + QString::number(activePer) + "%)<br>"
				   + "<font color=\'#a3a3a3'><strong>Inctive:</strong></font> " + QString::number(m_nodeMap.count() - active) + " (" + QString::number(100.0 - activePer) + "%)<br><br>"

				   + "<font color=\'#FF0000'><strong>Malicious:</strong></font> " + QString::number(malicious) + " (" + QString::number(maliciousPer) + "%)<br>"
				   + "<font color=\'#ff4242'><strong>Malicious active:</strong></font> " + QString::number(maliciousActive) + " (" + QString::number(maliciousActivePer) + "%)<br>"
				   + "<font color=\'#ff8a8a'><strong>Malicious inactive:</strong></font> " + QString::number(malicious - maliciousActive) + " (" + QString::number(100 - maliciousActivePer) + "%)<br><br>"

				   + "<font color=\'#42b4ff'><strong>Active non-malicious:</strong></font> " + QString::number((active - maliciousActive) / std::max(1.0, active) * 100.0) + "%<br><br>"

				   + "<strong>Edges:</strong> " + QString::number(m_edgeMap.count()) + "<br>"
				   + "<strong>Data:</strong> " + QString::number(data) + " GiB";

	//qDebug() << "getNodeInfo:" << info;

	return info;
}

QPointF GraphModel::getNodeCenter(QPointer<qan::Node> n){
	QPointF dxy(NODE_DIMEN / 2, NODE_DIMEN / 2);
	QPointF corner = n->getItem()->position();
	return corner - dxy;
}

void GraphModel::forceDirectedLayout(QList<qan::Node*> nodeList, QList<qan::Edge*> edgeList) {
	int nodeCount = m_graphElement->getNodeCount();

	//TODO: Define as constants
	int k = 200; //ideal node spacing (center to center)
	double temp = 100 * sqrt(nodeCount);
	double max_force = 100.0;

	for (int i = 0; i < 80; i++) {
		std::vector<QPointF> displacement(nodeCount, QPointF(0,0));

		//Calculating repulsive forces
		for(int u = 0; u < nodeCount; u++) {
			for(int v = 0; v < nodeCount; v++) {
				if(u == v)
					continue;

				QPointF current = getNodeCenter(nodeList.at(u));
				QPointF other = getNodeCenter(nodeList.at(v));
				QPointF delta = current - other;

				double distance = sqrt(delta.x() * delta.x() + delta.y() * delta.y());
				distance = std::max(0.1, distance);

				if(distance > 1000.0)
					continue;

				double force = k * k / distance;

				QPointF displacementPoint(delta.x() / distance * force, delta.y() / distance * force);

				displacement[u] += displacementPoint;
			}
		}

		//Calculating attractive forces
		for(const auto& edge : edgeList) {
			qan::Node* src = edge->getSource();
			qan::Node* dst = edge->getDestination();

			double dx = src->getItem()->x() - dst->getItem()->x();
			double dy = src->getItem()->y() - dst->getItem()->y();

			double distance = std::max(0.1, sqrt(dx*dx + dy*dy));

			double force = (distance * distance) / k;

			int srcInd = nodeList.indexOf(src);
			int dstInd = nodeList.indexOf(dst);

			QPointF disSrc(dx / distance * force, dy / distance * force);
			QPointF disDst(dx / distance * force, dy / distance * force);

			displacement[srcInd] -= disSrc;
			displacement[dstInd] += disDst;
		}

		for (int j = 0; j < nodeCount; j++) {
			double dispNorm = sqrt(displacement[j].x() * displacement[j].x() +
									displacement[j].y() * displacement[j].y());

			if(dispNorm < 1.0)
				continue;

			double cappedDispNorm = std::min(dispNorm, temp);

			QPointF newPos = nodeList.at(j)->getItem()->position() + displacement[j] / dispNorm * cappedDispNorm;
			nodeList.at(j)->getItem()->setPosition(newPos);
		}

		if(temp > 1.5)
			temp *= 0.85;
	}


	//Center view to node coordinate average
	double sumX = 0;
	double sumY = 0;
	for (auto& node : nodeList) {
		sumX += node->getItem()->x();
		sumY += node->getItem()->y();
	}

	QPointF viewCenter(sumX / nodeCount, sumY / nodeCount);
	m_graphView->centerOnPosition(viewCenter);
}


template<typename T>
QString GraphModel::generateUID(const QHash<QString, QPointer<T>> &container) {
	const QString charSet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	QString randomId;
	do {
		randomId.clear();
		for (int i = 0; i < ID_LENGTH; i++) {
			int rndIndex = QRandomGenerator::global()->bounded(charSet.length());
			randomId.append(charSet.at(rndIndex));
		}
	} while(container.contains(randomId));

	return randomId;
}
