#ifndef CUSTOMNETWORKNODE_H
#define CUSTOMNETWORKNODE_H

#define NODE_DIMEN 80
#define NODE_RADIUS (NODE_DIMEN / 2)

#include <QObject>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QuickQanava>

#include "customnetworkgraph.h"
#include "customnetworkedge.h"

enum FileType {
	txt,
	exe,
	pdf,
	img,
	script,
	generic
};

class NodeFile : public QObject{
	Q_OBJECT
  public:
	QString filename;
	QString path;
	QByteArray hashBytes;
	FileType filetype;
	int size; //in B

	NodeFile(QString filename, QString path, FileType filetype, double size, QByteArray hash);
	bool operator==(const NodeFile* other) const;
	static QPointer<NodeFile> fileFromJSON(QJsonObject fileObj);
	static QJsonObject fileToJSON(NodeFile* f);
};

class CustomNetworkEdge;

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
	Q_PROPERTY(QUrl image READ getImage WRITE setImage NOTIFY imageChanged)

	const QUrl& getImage() const noexcept { return m_image; }
	void setImage(QUrl image) noexcept;

	void setId(QString id);
	void addNeighbour(QPointer<CustomNetworkNode> n);
	void removeNeighbour(QPointer<CustomNetworkNode> n);
	void clearNeighbours();
	bool addFile(QPointer<NodeFile> file);
	bool removeFile(QByteArray hash);
	void sendFile(NodeFile* f, CustomNetworkNode* n);
	void setMalicious(bool malicious);
	void setActive(bool active);

	QString getID();
	QPointer<NodeFile> getFile(QString hash);
	QList<QPointer<CustomNetworkNode>> getNeighbours();
	QList<QPointer<NodeFile>> getFiles();

	bool isNeighbour(CustomNetworkNode* n);
	bool containsFile(QByteArray hash);
	bool isMalicious();
	bool isActive();

	int getDataSize();

	bool operator==(const CustomNetworkNode& other) const;

  public:
	static  QQmlComponent*  delegate(QQmlEngine& engine) noexcept;
	static qan::NodeStyle* style(QObject* parent = nullptr) noexcept;
	static QPointer<CustomNetworkNode> nodeFromJSON(CustomNetworkGraph* g, QJsonObject nodeObj);
	static QJsonObject nodeToJSON(CustomNetworkNode* n);

  signals:
	void imageChanged();

  private:
	QPointer<qan::NodeStyle> m_style;
	QList<QPointer<CustomNetworkNode>> m_neighbours;
	QList<QPointer<NodeFile>> m_files;
	QString m_id;
	QUrl m_image;
	bool m_malicious = false;
	bool m_active = true;
	bool m_shapeSet = false;

	void setNodeStyle();
	void setNodeShape();
};

QML_DECLARE_TYPE(CustomNetworkNode)

#endif // CUSTOMNETWORKNODE_H
