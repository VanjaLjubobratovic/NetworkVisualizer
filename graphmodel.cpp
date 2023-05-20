#include "graphmodel.h"

GraphModel::GraphModel(QObject *parent)
	: QObject{parent}
{

}

void GraphModel::setGraphElement(QPointer<qan::Graph> graph)
{
	m_graphElement = graph;
}

void GraphModel::clearGraph() {
	m_graphElement->clearGraph();
	m_nodeMap.clear();
}

bool GraphModel::readSavedGraph(QUrl fileUrl) {
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

	QJsonObject jsonObj = jsonDoc.object();

	GraphModel::clearGraph();

	//Reading nodes
	QMap<QString, qan::Node*> nodeMap;

	if(jsonObj.contains("nodes")) {
		QJsonObject nodesObj = jsonObj["nodes"].toObject();

		for(const auto &nodeKey : nodesObj.keys()) {
			QJsonObject nodeObj = nodesObj[nodeKey].toObject();
			double x = QRandomGenerator::global()->bounded(1280);
			double y = QRandomGenerator::global()->bounded(720);

			auto n = m_graphElement->insertNode();
			n->setLabel(nodeObj["label"].toString());
			n->getItem()->setRect({x, y, NODE_DIMEN, NODE_DIMEN});

			nodeMap[nodeKey] = n;
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

			auto e = m_graphElement->insertEdge(nodeMap[from], nodeMap[to]);
		}
	} else {
		qDebug() << "JSON document contains no edges";
	}

	return true;

}

//Turn this into a generalized function for adding any node
void GraphModel::addNode() {
	qan::Node* n = m_graphElement->insertNode();
	n->setLabel("New node");
	n->getItem()->setRect({0, 0, 100, 100});

	m_nodeMap[n->getLabel()] = n;
}
