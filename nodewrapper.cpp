#include "nodewrapper.h"

NodeWrapper::NodeWrapper(qan::Node *n, QString id)
	:m_node(n), m_id(id)
{
}

NodeWrapper::NodeFile::NodeFile(QString filename, QString path, FileType ft)
	:filename(filename), path(path), filetype(ft)
{
}

bool NodeWrapper::NodeFile::operator==(const NodeFile* other) const {
	return filename.compare(other->filename) && path.compare(other->path) && filetype == other->filetype;
}

void NodeWrapper::setNode(qan::Node *n){
	m_node = n;
}

void NodeWrapper::setId(QString id){
	m_id = id;
}

void NodeWrapper::addNeighbour(QPointer<NodeWrapper> n){
	m_neighbours.append(n);
}

void NodeWrapper::removeNeighbour(QPointer<NodeWrapper> n){
	int index = m_neighbours.indexOf(n);
	m_neighbours.removeAt(index);
}

void NodeWrapper::clearNeighbours(){
	m_neighbours.clear();
}

void NodeWrapper::addFile(NodeFile file){
	m_files.append(file);
}

void NodeWrapper::setMalicious(bool malicious){
	m_malicious = malicious;
}

QPointer<qan::Node> NodeWrapper::getNode(){
	return m_node;
}

QString NodeWrapper::getId(){
	return m_id;
}

QList<QPointer<NodeWrapper> > NodeWrapper::getNeighbours(){
	return m_neighbours;
}

QList<NodeWrapper::NodeFile> NodeWrapper::getFiles(){
	return m_files;
}

bool NodeWrapper::isNeighbour(NodeWrapper *n){
	for(const auto node : m_neighbours) {
		if (node->getId() == n->getId())
			return true;
	}
	return false;
}

bool NodeWrapper::isNeighbour(qan::Node *n){
	for(const auto node : m_neighbours) {
		if (node->getNode() == n)
			return true;
	}
	return false;
}

bool NodeWrapper::containsFile(NodeFile *f){
	return m_files.contains(f);
}

bool NodeWrapper::isMalicious(){
	return m_malicious;
}

bool NodeWrapper::operator==(const NodeWrapper &other) const {
	return m_id.compare(other.m_id);
}
