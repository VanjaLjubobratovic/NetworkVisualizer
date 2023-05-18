#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QuickQanava>
#include <QQuickStyle>

//Function for accessing Qan.Graph element in QML
QPointer<qan::Graph> findGraph(QQmlApplicationEngine* engine) {
	QPointer<qan::Graph> graph;
	for (const auto rootObject : engine->rootObjects()) {
		graph = qobject_cast<qan::Graph*>(rootObject->findChild<QQuickItem *>("graph"));
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

	QQmlApplicationEngine engine;
	const QUrl url(u"qrc:/NetworkVisualizer/Main.qml"_qs);
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
		&app, []() { QCoreApplication::exit(-1); },
		Qt::QueuedConnection);

	QuickQanava::initialize(&engine);

	engine.load(url);

	QPointer<qan::Graph> graph = findGraph(&engine);
	if(!graph) {
		qDebug() << "Graph element not found!";
		emit engine.quit();
	}

	return app.exec();
}
