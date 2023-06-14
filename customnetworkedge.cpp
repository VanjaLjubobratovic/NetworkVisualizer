#include "customnetworkedge.h"

QQmlComponent*  CustomNetworkEdge::delegate(QQmlEngine& engine, QObject* parent) noexcept
{
	static std::unique_ptr<QQmlComponent>   customNetworkEdge_delegate;
	if (!customNetworkEdge_delegate)
		customNetworkEdge_delegate = std::make_unique<QQmlComponent>(&engine, "qrc:/CustomEdge.qml",
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
