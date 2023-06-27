#include "customnetworkedge.h"

#include <QQuickView>
#include <thread>
#include <chrono>

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

void CustomNetworkEdge::handleTransferEnded(QString objName, QString senderId, QString hash) {

	QObject* obj = this->findChild<QObject*>(objName);
	if(!obj) {
		qDebug() << "Cannot find object in handleTransferEnded";
		return;
	}

	auto src = dynamic_cast<CustomNetworkNode*>(this->getSource());
	auto dst = dynamic_cast<CustomNetworkNode*>(this->getDestination());

	CustomNetworkNode* receiver;
	NodeFile* file;
	bool result;

	// Written like this for readability
	if(src->getID() == senderId) {
		receiver = dst;
		file = src->getFile(hash);
	} else {
		receiver = src;
		file = dst->getFile(hash);
	}

	result = receiver->addFile(file);

	QMetaObject::invokeMethod(obj, "animateResult",
							   Q_ARG(bool, result));

}

void CustomNetworkEdge::animateTransfer(CustomNetworkNode* sender, CustomNetworkNode* receiver, NodeFile* file) {
	//QML item generation
	QQmlEngine* engine = qmlEngine(this->getItem());
	QQmlComponent component(engine, QUrl("qrc:/NetworkVisualizer/FileImage.qml"), this);
	QObject* obj = component.create();

	if(!component.isReady() && component.isError())
		qDebug() << component.errorString();

	obj->setObjectName(QUuid::createUuid().toString());
	obj->setParent(this);

	QQuickItem* item = qobject_cast<QQuickItem*>(obj);
	item->setParentItem(this->getItem());

	QObject::connect(this->getItem(), SIGNAL(dstAngleChanged()), item, SLOT(onParentDimensionsChanged()));
	QObject::connect(item, SIGNAL(transferEnded(QString, QString, QString)), this, SLOT(handleTransferEnded(QString, QString, QString)));

	//Determining connector coordinates in EdgeItem CS
	//for sender and receiver.
	//Has to be done this way since we are pretending
	//edges are undirected.
	QPointF senderC, receiverC;
	if(sender == this->getSource()) {
		senderC = this->getItem()->getP1();
		receiverC = this->getItem()->getP2();
		item->setProperty("sender", "src");
	} else {
		senderC = this->getItem()->getP2();
		receiverC = this->getItem()->getP1();
		item->setProperty("sender", "dst");
	}

	//Animation test from sender to receiver
	item->setPosition(senderC);
	item->setProperty("hash", QString::fromUtf8(file->hashBytes));
	item->setProperty("senderId", sender->getID());
	QMetaObject::invokeMethod(obj, "startFileTransfer",
							   Q_ARG(QVariant, receiverC),
							   Q_ARG(QVariant, 1000));


}
