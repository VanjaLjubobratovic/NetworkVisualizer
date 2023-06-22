#include "customnetworkedge.h"

#include <QQuickView>

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
	QQmlEngine* engine = qmlEngine(this->getItem());
	QQmlComponent component(engine, QUrl("qrc:/NetworkVisualizer/FileImage.qml"));
	QQuickItem* item = qobject_cast<QQuickItem*>(component.create());
	item->setParentItem(this->getItem());
}
