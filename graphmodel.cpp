#include "graphmodel.h"
#include <cmath>

GraphModel::GraphModel(QObject *parent)
	: QObject{parent}
{

}

void GraphModel::setGraphElement(QPointer<qan::Graph> graph) {
	m_graphElement = graph;
	QObject::connect(m_graphElement, &qan::Graph::edgeInserted, this, &GraphModel::onDrawNewEdge);
}

void GraphModel::setGraphView(QPointer<qan::GraphView> gw){
	m_graphView = gw;
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

	//qDebug() << "EDGES" << m_edgeMap;
	//qDebug() << "NODES" << m_nodeMap;
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
			double x = QRandomGenerator::global()->bounded(1280);
			double y = QRandomGenerator::global()->bounded(720);

			auto n = m_graphElement->insertNode();
			n->setLabel(nodeObj["label"].toString());
			//TODO: Change node positioning
			n->getItem()->setRect({x, y, NODE_DIMEN, NODE_DIMEN});
			setNodeStyle(n);

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
			m_edgeMap[edgeKey] = e;
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
		QJsonObject nodeObj;
		nodeObj.insert("label", m_nodeMap[nodeKey]->getLabel());

		//insert other attributes

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

void GraphModel::drawNewNode(const QString label) {
	QPointer<qan::Node> n = m_graphElement->insertNode();
	n->setLabel(label);
	n->getItem()->setRect({100, 100, NODE_DIMEN, NODE_DIMEN});
	setNodeStyle(n);

	QString id = generateUID(m_nodeMap);
	m_nodeMap.insert(id, n);

	qDebug() << "New node inserted:" << id;
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

	QString id = generateUID(m_edgeMap);
	m_edgeMap.insert(id, e);

	qDebug() << "New edge inserted:" << id;
}

//Function for finding a key for a specified node object
//Used when writing edges to JSON
QString GraphModel::getNodeId(QPointer<qan::Node> targetNode) {
	for (auto it = m_nodeMap.begin(); it != m_nodeMap.end(); ++it) {
		if(it.value() == targetNode) {
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

		//Possibly make edge comparator separate function
		if((e->getSource() == targetEdge->getSource()) && (e->getDestination() == targetEdge->getDestination())) {
			return true;
		}
	}
	return false;
}


//TODO: There has to be a cleaner way of doing this
void GraphModel::setNodeStyle(QPointer<qan::Node> n) {
	n->getItem()->setResizable(false);

	/* Generates round bounding polygon so
	 * there is never a gap between edges
	 * and dest / src nodes
	 */
	QPainterPath path;
	qreal shapeRadius = 100.;   // In percentage = 100% !
	path.addRoundedRect(QRectF{ 0., 0., NODE_DIMEN, NODE_DIMEN}, shapeRadius, shapeRadius);
	QPolygonF boundingShape =  path.toFillPolygon(QTransform{});
	n->getItem()->setBoundingShape(boundingShape);


	n->style()->setBackRadius(NODE_RADIUS);
	n->style()->setBorderColor("darkblue");
	n->style()->setBorderWidth(2);

	n->style()->setFillType(qan::NodeStyle::FillType::FillGradient);
	n->style()->setBaseColor("#368bfc");
	n->style()->setBackColor("#c5dbfa");
	n->style()->setBackOpacity(70);

	n->style()->setEffectType(qan::NodeStyle::EffectType::EffectGlow);
	n->style()->setEffectColor("black");
	n->style()->setEffectRadius(7);
}

//QML invokable function without arguments
//I didn't want to change original function signature for this
//I know it's dumb
void GraphModel::forceDirectedLayout() {
	forceDirectedLayout(m_graphElement->get_nodes(), m_graphElement->get_edges());
}

void GraphModel::forceDirectedLayout(QList<qan::Node*> nodeList, QList<qan::Edge*> edgeList) {
	int nodeCount = m_graphElement->getNodeCount();
	int k = 70; //spread constant (bigger means nodes more spread out)
	double c = sqrt((1280*720) / std::max(1, nodeCount));
	std::vector<QPointF> displacement(nodeCount, QPointF(0,0));

	for (int i = 0; i < 200; i++) {
		//Calculating repulsive forces
		for(int u = 0; u < nodeCount; u++) {
			for(int v = 0; v < nodeCount; v++) {
				if(u == v)
					continue;

				int ux = nodeList.at(u)->getItem()->x();
				int uy = nodeList.at(u)->getItem()->y();

				double dx = ux - nodeList.at(v)->getItem()->x();
				double dy = uy - nodeList.at(v)->getItem()->y();

				double distance = std::max(0.1, sqrt(dx*dx + dy*dy));
				double force = k * k / distance;

				QPointF newPoint(displacement[u].x() + (dx / distance) * force,
								  displacement[u].y() + (dy / distance) * force);

				//qDebug() << ux << uy << dx << dy << distance << force;

				displacement[u] = newPoint;
			}
		}

		//Calculating attractive forces
		for(const auto& edge : edgeList) {
			qan::Node* src = edge->getSource();
			qan::Node* dst = edge->getDestination();

			double dx = src->getItem()->x() - dst->getItem()->x();
			double dy = src->getItem()->y() - dst->getItem()->y();
			double distance = std::max(0.1, sqrt(dx*dx + dy*dy));
			double force = (distance + k) * (distance + k) / (k * k);

			int srcInd = nodeList.indexOf(src);
			int dstInd = nodeList.indexOf(dst);


			QPointF disSrc(displacement[srcInd].x() - (dx / distance) * force,
							  displacement[srcInd].y() - (dy / distance) * force);
			QPointF disDst(displacement[dstInd].x() + (dx / distance) * force,
							displacement[dstInd].y() + (dy / distance) * force);


			//qDebug() << "ATTRACTIVE:" << "Src-" << disSrc << "|| Dst-" << disDst;
			/*qDebug() << QString("Dis. SRC: %1x %2y, Dis. DST: %3x %4y")
							 .arg(disSrc.x()).arg(disSrc.y())
							 .arg(disDst.x()).arg(disDst.y());*/

			displacement[srcInd] = disSrc;
			displacement[dstInd] = disDst;
		}

		//TODO: implement some form of annealing
		for(int j = 0; j < nodeCount; j++) {
			/*qDebug() << QString("%1 - Dis. X: %2 || DIS. Y: %3")
							 .arg(j).arg(displacement[j].x()).arg(displacement[j].y());*/

			double dispNorm = sqrt(displacement[j].x() * displacement[j].x() +
									displacement[j].y() * displacement[j].y());

			dispNorm = std::max(1.0, dispNorm); //division by 0 mitigation
			double ratio = dispNorm / std::max(1.0, dispNorm);


			double nx = (displacement[j].x() / dispNorm) * std::min(dispNorm, c);
			double ny = (displacement[j].y() / dispNorm) * std::min(dispNorm, c);

			/*qDebug() << QString("dispNorm: %1, ratio: %2, nx: %3, ny: %4")
							.arg(dispNorm).arg(ratio).arg(nx).arg(ny);*/

			nodeList[j]->getItem()->setPosition(QPointF(nx, ny));
		}
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
