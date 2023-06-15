#ifndef CUSTOMNETWORKNODE_H
#define CUSTOMNETWORKNODE_H

#define NODE_DIMEN 80
#define NODE_RADIUS (NODE_DIMEN / 2)

#include <QObject>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QuickQanava>

enum FileType {
	txt,
	exe,
	pdf,
	img,
	script,
	generic
};

struct NodeFile {
	QString filename;
	QString path;
	FileType filetype;

	NodeFile(QString filename, QString path, FileType filetype);
	bool operator==(const NodeFile* other) const;
};

class CustomNetworkNode : public qan::Node
{
	Q_OBJECT
	QML_ELEMENT
  public:
	explicit CustomNetworkNode(QObject *parent = nullptr) : qan::Node{parent}{};
	explicit CustomNetworkNode(QString id, bool malicious, bool active);
	virtual ~CustomNetworkNode() override = default;
	CustomNetworkNode( const CustomNetworkNode& ) = delete;

	Q_INVOKABLE QString getNodeInfo();

	void setId(QString id);
	void addNeighbour(QPointer<CustomNetworkNode> n);
	void removeNeighbour(QPointer<CustomNetworkNode> n);
	void clearNeighbours();
	void addFile(NodeFile* file);
	void setMalicious(bool malicious);
	void setActive(bool active);

	QString getID();
	QList<QPointer<CustomNetworkNode>> getNeighbours();
	QList<QPointer<NodeFile>> getFiles();

	bool isNeighbour(CustomNetworkNode* n);
	bool containsFile(NodeFile* f);
	bool isMalicious();
	bool isActive();

	bool operator==(const CustomNetworkNode& other) const;

  public:
	static  QQmlComponent*  delegate(QQmlEngine& engine) noexcept;
	static qan::NodeStyle* style(QObject* parent = nullptr) noexcept;

  signals:

  private:
	QPointer<qan::NodeStyle> m_style;
	QList<QPointer<CustomNetworkNode>> m_neighbours;
	QList<QPointer<NodeFile>> m_files;
	QString m_id;
	bool m_malicious = false;
	bool m_active = true;

	void setNodeStyle();
	void setNodeShape();
};

QML_DECLARE_TYPE(CustomNetworkNode)

#endif // CUSTOMNETWORKNODE_H
