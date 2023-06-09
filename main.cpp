#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QuickQanava>
#include <QQuickStyle>

#include <QJsonDocument>
#include <QJsonObject>

#include <jsoncpp/json/json.h>

#include <QRandomGenerator>

#include "graphmodel.h"
#include <QuickQanava/samples/style/custom.h>

//Function for accessing Qan.Graph element in QML
QPointer<qan::Graph> findGraph(QQmlApplicationEngine* engine, QString itemId) {
	QPointer<qan::Graph> graph;
	for (const auto rootObject : engine->rootObjects()) {
		graph = qobject_cast<qan::Graph*>(rootObject->findChild<QQuickItem *>(itemId));
		if(graph) {
			qDebug() << "Graph element found" << graph;
			qDebug() << "Graph view" << graph->parentItem();
			break;
		}
	}
	return graph;
}

QPointer<qan::GraphView> findGraphView(QQmlApplicationEngine* engine, QString itemId) {
	QPointer<qan::GraphView> graphView;
	for (const auto rootObject : engine->rootObjects()) {
		graphView = qobject_cast<qan::GraphView*>(rootObject->findChild<QQuickItem *>(itemId));
		if(graphView) {
			qDebug() << "Graph View element found" << graphView;
			break;
		}
	}
	return graphView;
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

	//Make this a graphModel method
	QPointer<qan::Graph> graph = findGraph(&engine, "graph");
	QPointer<qan::GraphView> graphView = findGraphView(&engine, "graphView");

	if(!graph) {
		qDebug() << "Graph element not found!";
		emit engine.quit();
	} else if (!graphView) {
		qDebug() << "Graph view element not found!";
		emit engine.quit();
	}

	graphModel->setGraphElement(graph);
	graphModel->setGraphView(graphView);


	return app.exec();
}
