#ifndef CUSTOMNETWORKEDGE_H
#define CUSTOMNETWORKEDGE_H

#include <QObject>
#include <QQmlEngine>
#include <QuickQanava>

#include "customnetworknode.h"
#include "appglobals.h"

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

	Q_INVOKABLE void animateTransfer(qan::Node* sender, qan::Node* receiver);
	//Q_PROPERTY(type name READ name WRITE setName NOTIFY nameChanged)

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
};

QML_DECLARE_TYPE(CustomNetworkEdge)

#endif // CUSTOMNETWORKEDGE_H
