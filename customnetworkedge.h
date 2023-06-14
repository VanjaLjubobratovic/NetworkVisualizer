#ifndef CUSTOMNETWORKEDGE_H
#define CUSTOMNETWORKEDGE_H

#include <QObject>
#include <QQmlEngine>
#include <QuickQanava>

#include "customnetworknode.h"

class CustomNetworkEdge : public qan::Edge
{
	Q_OBJECT
	QML_ELEMENT
  public:
	explicit CustomNetworkEdge(QObject* parent = nullptr) : qan::Edge{parent} { }
	virtual ~CustomNetworkEdge() override = default;
	CustomNetworkEdge(const CustomNetworkEdge&) = delete;

  public:
	static  QQmlComponent*  delegate(QQmlEngine& engine, QObject* parent = nullptr) noexcept;
	static  qan::EdgeStyle* style(QObject* parent = nullptr) noexcept;
};

QML_DECLARE_TYPE(CustomNetworkEdge)

#endif // CUSTOMNETWORKEDGE_H
