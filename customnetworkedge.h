#ifndef CUSTOMNETWORKEDGE_H
#define CUSTOMNETWORKEDGE_H

#include <QObject>
#include <QQmlEngine>
#include <QuickQanava>

#include "customnetworkgraph.h"
#include "customnetworknode.h"

class CustomNetworkNode;
class NodeFile;

class CustomNetworkEdge : public qan::Edge
{
	Q_OBJECT
	QML_ELEMENT
  public:
	explicit CustomNetworkEdge(QObject* parent = nullptr) : qan::Edge{parent} { }
	virtual ~CustomNetworkEdge() override = default;
	CustomNetworkEdge(const CustomNetworkEdge&) = delete;

	void setId(const QString id);
	void setBandwidth(const double bandwidth);
	void setSendingData(const bool sending);
	QString getId();
	double getBandwidth();
	bool isSendingData();

	Q_INVOKABLE void animateTransfer(CustomNetworkNode* sender, CustomNetworkNode* receiver, NodeFile* file);

  public:
	static  QQmlComponent*  delegate(QQmlEngine& engine, QObject* parent = nullptr) noexcept;
	static  qan::EdgeStyle* style(QObject* parent = nullptr) noexcept;
	static QPointer<CustomNetworkEdge> edgeFromJSON(CustomNetworkGraph* g, QJsonObject edgeObj);
	static QJsonObject edgeToJSON(CustomNetworkEdge* e);

  private:
	QPointer<qan::EdgeStyle> m_style;
	QString m_id;
	double m_bandwidth;
	bool sendingData = false; //As in currently sending data

	void setEdgeStyle();

  public slots:
	void handleTransferEnded(QString objName, QString senderId, QString hash);
};

QML_DECLARE_TYPE(CustomNetworkEdge)

#endif // CUSTOMNETWORKEDGE_H
