#ifndef NODEWRAPPER_H
#define NODEWRAPPER_H

#define NODE_DIMEN 80
#define NODE_RADIUS (NODE_DIMEN / 2)

#include <QObject>
#include <QDebug>
#include <QuickQanava>

class NodeWrapper : public QObject
{
	Q_OBJECT
  public:
	enum FileType {
		txt,
		exe,
		pdf,
		img,
		script
	};

	struct NodeFile {
		QString filename;
		QString path;
		FileType filetype;

		NodeFile(QString filename, QString path, FileType ft);
		bool operator==(const NodeFile* other) const;
	};
	explicit NodeWrapper(qan::Node* n, QString id);
	explicit NodeWrapper(qan::Node* n, QString id, bool malicious, bool active);

	Q_INVOKABLE QString getNodeInfo();

	void setNode(qan::Node* n);
	void setId(QString id);
	void addNeighbour(QPointer<NodeWrapper> n);
	void removeNeighbour(QPointer<NodeWrapper> n);
	void clearNeighbours();
	void addFile(NodeFile file);
	void setMalicious(bool malicious);
	void setActive(bool active);

	QPointer<qan::Node> getNode();
	QString getId();
	QList<QPointer<NodeWrapper>> getNeighbours();
	QList<NodeFile> getFiles();

	bool isNeighbour(NodeWrapper* n);
	bool isNeighbour(qan::Node* n);
	bool containsFile(NodeFile* f);
	bool isMalicious();
	bool isActive();

	bool operator==(const NodeWrapper& other) const;

  signals:

  private:
	QPointer<qan::Node> m_node;
	QList<QPointer<NodeWrapper>> m_neighbours;
	QList<NodeFile> m_files;
	QString m_id;
	bool m_malicious = false;
	bool m_active = true;

	void setNodeStyle();
	void setNodeShape();
};

#endif // NODEWRAPPER_H
