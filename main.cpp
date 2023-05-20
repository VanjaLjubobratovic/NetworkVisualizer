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
	if(!graph) {
		qDebug() << "Graph element not found!";
		emit engine.quit();
	}

	graphModel->setGraphElement(graph);

	return app.exec();
}
