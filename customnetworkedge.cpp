#include "customnetworkedge.h"

void CustomNetworkEdge::setId(const QString id) {
	m_id = id;
}

QString CustomNetworkEdge::getId() {
	return m_id;
}

QQmlComponent*  CustomNetworkEdge::delegate(QQmlEngine& engine, QObject* parent) noexcept
{
	static std::unique_ptr<QQmlComponent>   customNetworkEdge_delegate;
	if (!customNetworkEdge_delegate)
		/*customNetworkEdge_delegate = std::make_unique<QQmlComponent>(&engine, "qrc:/QuickQanava/Edge.qml",
																  QQmlComponent::PreferSynchronous, parent);*/
		customNetworkEdge_delegate = std::make_unique<QQmlComponent>(&engine, "qrc:/NetworkVisualizer/NetworkEdge.qml",
																	  QQmlComponent::PreferSynchronous, parent);
	return customNetworkEdge_delegate.get();
}

qan::EdgeStyle* CustomNetworkEdge::style(QObject* parent) noexcept
{
	Q_UNUSED(parent)
	static std::unique_ptr<qan::EdgeStyle>  customNetworkEdge_style;
	if ( !customNetworkEdge_style )
		customNetworkEdge_style = std::make_unique<qan::EdgeStyle>();
	return customNetworkEdge_style.get();
}

void CustomNetworkEdge::animateTransfer() {
	qDebug() << g_appEngine;
	QQmlComponent component(g_appEngine,QUrl(QStringLiteral("qrc:/NetworkVisualizer/FileImage.qml")));
	QObject* obj = component.create();
	if(component.status() == QQmlComponent::Error)
		qDebug() << component.errorString();


	/*QMetaObject::invokeMethod(obj, "fileTransfer",
							   Q_ARG(int, 100),
							   Q_ARG(int, 100),
							   Q_ARG(int, 50),
							   Q_ARG(int, 50));*/
}
