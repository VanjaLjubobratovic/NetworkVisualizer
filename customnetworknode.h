#ifndef CUSTOMNETWORKNODE_H
#define CUSTOMNETWORKNODE_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QuickQanava>

class CustomNetworkNode : public qan::Node
{
	Q_OBJECT
	QML_ELEMENT
  public:
	explicit CustomNetworkNode(QObject *parent = nullptr) : qan::Node{parent}{};
	virtual ~CustomNetworkNode() override = default;
	CustomNetworkNode( const CustomNetworkNode& ) = delete;

  public:
	static  QQmlComponent*  delegate(QQmlEngine& engine) noexcept;
	static qan::NodeStyle* style(QObject* parent = nullptr) noexcept;

  private:
	QPointer<qan::NodeStyle> m_style;
};

QML_DECLARE_TYPE(CustomNetworkNode)

#endif // CUSTOMNETWORKNODE_H
