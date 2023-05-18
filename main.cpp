#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QuickQanava>
#include <QQuickStyle>


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

	return app.exec();
}
