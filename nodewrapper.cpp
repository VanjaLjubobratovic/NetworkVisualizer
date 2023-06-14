#include "nodewrapper.h"
#include <QPainter>
#include <QPainterPath>

NodeWrapper::NodeWrapper(qan::Node *n, QString id)
	:m_id(id)
{
	m_node = dynamic_cast<CustomNetworkNode*>(n);

	setNodeShape();
	setNodeStyle();
}

NodeWrapper::NodeWrapper(qan::Node *n, QString id, bool malicious, bool active)
	:m_id(id), m_malicious(malicious), m_active(active)
{
	m_node = dynamic_cast<CustomNetworkNode*>(n);

	setNodeShape();
	setNodeStyle();
}

QString NodeWrapper::getNodeInfo() {
	return QString("ID: %1\nNeighbours: %2\n# of files: %3\nActive: %4\nMalicious: %5\n")
					.arg(m_id).arg(m_neighbours.length())
					.arg(m_files.length())
					.arg(m_active ? "True" : "False")
					.arg(m_malicious ? "True" : "False");
}

NodeWrapper::NodeFile::NodeFile(QString filename, QString path, FileType ft)
	:filename(filename), path(path), filetype(ft)
{
}

bool NodeWrapper::NodeFile::operator==(const NodeFile* other) const {
	return filename.compare(other->filename) && path.compare(other->path) && filetype == other->filetype;
}

void NodeWrapper::setNode(CustomNetworkNode *n){
	m_node = n;
	setNodeShape();
	setNodeStyle();
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
	setNodeStyle();
}

void NodeWrapper::setActive(bool active){
	m_active = active;
	setNodeStyle();
}

QPointer<CustomNetworkNode> NodeWrapper::getNode(){
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

bool NodeWrapper::isNeighbour(CustomNetworkNode *n){
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

bool NodeWrapper::isActive(){
	return m_active;
}

bool NodeWrapper::operator==(const NodeWrapper &other) const {
	return m_id.compare(other.m_id);
}



void NodeWrapper::setNodeStyle() {
	QColor base, back, border;
	QList<double> saturation, brightness; //<base, back, border>
	if(m_malicious) {
		base = QColor("#fc3636").toHsv();
		back = QColor("#fac5c5").toHsv();
		border = QColor("darkred").toHsv();
	} else {
		base = QColor("#368bfc").toHsv();
		back = QColor("#c5dbfa").toHsv();
		border = QColor("darkblue").toHsv();
	}

	//Making node a bit greyed out if inactive
	if(!m_active) {
		saturation = {base.saturation() * 0.6, back.saturation() * 0.6, border.saturation() * 0.6};
		brightness = {base.value() * 0.6, base.value() * 0.6, border.value() * 0.6};

		base.setHsv(base.hue(), saturation[0], brightness[0]);
		back.setHsv(back.hue(), saturation[1], brightness[1]);
		border.setHsv(border.hue(), saturation[2], brightness[2]);
	}

	qan::NodeStyle s = m_node->getItem()->getStyle();
	s.setBackColor(back);
	s.setBaseColor(base);
	s.setBorderColor(border);

	m_node->getItem()->setItemStyle(&s);
}

void NodeWrapper::setNodeShape() {
	m_node->getItem()->setResizable(false);

	/* Generates round bounding polygon so
	 * there is never a gap between edges
	 * and dest / src nodes
	 */
	QPainterPath path;
	qreal shapeRadius = 100.;   // In percentage = 100% !
	path.addRoundedRect(QRectF{ 0., 0., NODE_DIMEN, NODE_DIMEN}, shapeRadius, shapeRadius);
	QPolygonF boundingShape =  path.toFillPolygon(QTransform{});
	m_node->getItem()->setBoundingShape(boundingShape);

	m_node->style()->setBackRadius(NODE_RADIUS);
	m_node->style()->setBorderWidth(2);

	m_node->style()->setFillType(qan::NodeStyle::FillType::FillGradient);
	m_node->style()->setBackOpacity(70);

	m_node->style()->setEffectType(qan::NodeStyle::EffectType::EffectGlow);
	m_node->style()->setEffectColor("black");
	m_node->style()->setEffectRadius(7);
}
