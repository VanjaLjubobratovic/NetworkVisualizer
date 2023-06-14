#include "customnetworknode.h"

QQmlComponent* CustomNetworkNode::delegate(QQmlEngine& engine) noexcept {
	static std::unique_ptr<QQmlComponent>   customNetworkNode_delegate;
	if ( !customNetworkNode_delegate )
		//I don't know why it can't find file when it is inside .qrc
		customNetworkNode_delegate = std::make_unique<QQmlComponent>(&engine, "qrc:/NetworkVisualizer/RoundNode.qml");
	return customNetworkNode_delegate.get();
}

qan::NodeStyle* CustomNetworkNode::style(QObject* parent) noexcept
{
	Q_UNUSED(parent)
	static std::unique_ptr<qan::NodeStyle>  customNetworkNode_style;
	if ( !customNetworkNode_style ) {
		customNetworkNode_style = std::make_unique<qan::NodeStyle>();
		customNetworkNode_style->setBackColor(QColor("#0770ff"));
	}


	return customNetworkNode_style.get();
}
