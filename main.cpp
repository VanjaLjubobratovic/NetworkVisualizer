#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QuickQanava>
#include <QQuickStyle>

#include <QJsonDocument>
#include <QJsonObject>

#include <jsoncpp/json/json.h>

#include <QRandomGenerator>

#include "graphmodel.h"

#define NODE_DIMEN 100

//Function for accessing Qan.Graph element in QML
QPointer<qan::Graph> findGraph(QQmlApplicationEngine* engine, QString itemId) {
	QPointer<qan::Graph> graph;
	for (const auto rootObject : engine->rootObjects()) {
		graph = qobject_cast<qan::Graph*>(rootObject->findChild<QQuickItem *>(itemId));
		if(graph) {
			qDebug() << "Graph element found" << graph;
			break;
		}
	}
	return graph;
}

//Reading saved graph in JSON format
bool readSavedGraph(QPointer<qan::Graph> graph, QString filename) {
	//TODO: Implement relative path
	QFile file("/home/vanja/QTProjects/NetworkVisualizer/" + filename);
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

	//Reading nodes
	QMap<QString, qan::Node*> nodeMap;

	if(jsonObj.contains("nodes")) {
		QJsonObject nodesObj = jsonObj["nodes"].toObject();

		for(const auto &nodeKey : nodesObj.keys()) {
			QJsonObject nodeObj = nodesObj[nodeKey].toObject();
			double x = QRandomGenerator::global()->bounded(1280);
			double y = QRandomGenerator::global()->bounded(720);

			auto n = graph->insertNode();
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

			auto e = graph->insertEdge(nodeMap[from], nodeMap[to]);
		}
	} else {
		qDebug() << "JSON document contains no edges";
	}

	return true;
}


int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

	/*This in tandem with qtquickcontrols2.conf
	 fixes invisible node text due to darkmode linux*/
	QQuickStyle::setStyle("Material");

	//qmlRegisterType<GraphModel>("GraphModel", 1, 0, "GraphModel");
	QPointer<GraphModel> graphModel = new GraphModel();

	QQmlApplicationEngine engine;
	const QUrl url(u"qrc:/NetworkVisualizer/Main.qml"_qs);
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
		&app, []() { QCoreApplication::exit(-1); },
		Qt::QueuedConnection);

	QuickQanava::initialize(&engine);

	engine.rootContext()->setContextProperty("graphModel", graphModel);

	engine.load(url);

	QPointer<qan::Graph> graph = findGraph(&engine, "graph");
	if(!graph) {
		qDebug() << "Graph element not found!";
		emit engine.quit();
	}

	if (!readSavedGraph(graph, "graphData.json")) {
		qDebug() << "Reading data and adding to graph failed";
		graph.clear();
	} else {
		qDebug() << "Json reading success!";
	}

	//--- Class Transfer test ---
	graphModel->setGraphElement(graph);

	return app.exec();
}
