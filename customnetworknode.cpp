#include "customnetworknode.h"
#include <QPainter>
#include <QPainterPath>

NodeFile::NodeFile(QString filename, QString path, FileType filetype, double size, QByteArray hash)
	:filename(filename), path(path), filetype(filetype), size(size), hashBytes(hash)
{
}

bool NodeFile::operator==(const NodeFile *other) const {
	return hashBytes == other->hashBytes;
}

QPointer<NodeFile> NodeFile::fileFromJSON(QJsonObject fileObj) {
	QString filename = fileObj["filename"].toString();
	QString path = fileObj["path"].toString();
	FileType filetype = FileType::generic; //TODO: implement this
	double size = fileObj["size"].toDouble();
	QByteArray hash = QByteArray::fromHex(fileObj["hash"].toString().toLatin1());

	return new NodeFile(filename, path, filetype, size, hash);
}

QJsonObject NodeFile::fileToJSON(NodeFile *f) {
	QJsonObject fileObj;
	fileObj.insert("filename", f->filename);
	fileObj.insert("path", f->path);
	fileObj.insert("size", f->size);
	fileObj.insert("filetype", f->filetype);
	fileObj.insert("hash", QString::fromLatin1(f->hashBytes.toHex()));

	return fileObj;
}

CustomNetworkNode::CustomNetworkNode(QString id, bool malicious, bool active)
	:m_id(id), m_malicious(malicious), m_active(active)
{
	setNodeShape();
	setNodeStyle();
}

QString CustomNetworkNode::getNodeInfo() {
	return QString("ID: %1\nNeighbours: %2\n# of files: %3\nActive: %4\nMalicious: %5\n")
		.arg(m_id).arg(m_neighbours.length())
		.arg(m_files.length())
		.arg(m_active ? "True" : "False")
		.arg(m_malicious ? "True" : "False");
}

void CustomNetworkNode::setId(QString id) {
	m_id = id;
}

void CustomNetworkNode::addNeighbour(QPointer<CustomNetworkNode> n) {
	if(!m_neighbours.contains(n))
		m_neighbours.append(n);
}

void CustomNetworkNode::removeNeighbour(QPointer<CustomNetworkNode> n) {
	for(int i = 0; i < m_neighbours.size(); i++) {
			if(m_neighbours.at(i) == n)
				m_neighbours.remove(i);
	}
}

void CustomNetworkNode::clearNeighbours() {
	m_neighbours.clear();
}

void CustomNetworkNode::addFile(QPointer<NodeFile> file) {
	if (!m_files.contains(file))
			m_files.append(file);
}

void CustomNetworkNode::setMalicious(bool malicious) {
	m_malicious = malicious;
	setNodeStyle();
	setNodeShape();
}

void CustomNetworkNode::setActive(bool active) {
	m_active = active;
	setNodeStyle();
	setNodeShape();
}

QString CustomNetworkNode::getID() {
	return m_id;
}

QList<QPointer<CustomNetworkNode> > CustomNetworkNode::getNeighbours() {
	return m_neighbours;
}

QList<QPointer<NodeFile> > CustomNetworkNode::getFiles() {
	return m_files;
}

bool CustomNetworkNode::isNeighbour(CustomNetworkNode *n) {
	return m_neighbours.contains(n);
}

bool CustomNetworkNode::containsFile(QPointer<NodeFile> f) {
	return m_files.contains(f);
}

bool CustomNetworkNode::isMalicious() {
	return m_malicious;
}

bool CustomNetworkNode::isActive() {
	return m_active;
}

bool CustomNetworkNode::operator==(const CustomNetworkNode &other) const {
	return QString::compare(other.m_id, m_id);
}

QQmlComponent* CustomNetworkNode::delegate(QQmlEngine& engine) noexcept {
	static std::unique_ptr<QQmlComponent>   customNetworkNode_delegate;
	if ( !customNetworkNode_delegate )
		//I don't know why it can't find file when it is inside .qrc
		customNetworkNode_delegate = std::make_unique<QQmlComponent>(&engine, "qrc:/NetworkVisualizer/RoundNode.qml");
	return customNetworkNode_delegate.get();
}

qan::NodeStyle* CustomNetworkNode::style(QObject* parent) noexcept
{
	Q_UNUSED(parent)
	static std::unique_ptr<qan::NodeStyle>  customNetworkNode_style;
	if ( !customNetworkNode_style ) {
		customNetworkNode_style = std::make_unique<qan::NodeStyle>();
		customNetworkNode_style->setBackColor(QColor("#0770ff"));
	}


	return customNetworkNode_style.get();
}

QPointer<CustomNetworkNode> CustomNetworkNode::nodeFromJSON(CustomNetworkGraph *g, QJsonObject nodeObj) {
	QPointF center(QRandomGenerator::global()->bounded(1280), QRandomGenerator::global()->bounded(720));

	auto n = dynamic_cast<CustomNetworkNode*>(g->insertCustomNode());
	n->setLabel(nodeObj["label"].toString());
	n->getItem()->setRect({center.x(), center.y(), NODE_DIMEN, NODE_DIMEN});
	n->setId(nodeObj["id"].toString());

	n->setMalicious(nodeObj["malicious"].toBool());
	n->setActive(nodeObj["active"].toBool());

	QJsonObject filesObj = nodeObj["files"].toObject();
	for(const auto &fileKey : filesObj.keys()) {
		QJsonObject fileObj = filesObj[fileKey].toObject();
		n->addFile(NodeFile::fileFromJSON(fileObj));
	}

	return n;
}

QJsonObject CustomNetworkNode::nodeToJSON(CustomNetworkNode *n) {
	QJsonObject nodeObj;
	nodeObj.insert("label", n->getLabel());
	nodeObj.insert("id", n->getID());
	nodeObj.insert("malicious", n->isMalicious());
	nodeObj.insert("active", n->isActive());


	QJsonObject filesObj;
	for(int i = 0; i < n->getFiles().count(); i++) {
		QJsonObject fileObj = NodeFile::fileToJSON(n->getFiles().at(i));
		filesObj.insert(QString::number(i), fileObj);
	}

	nodeObj.insert("files", filesObj);

	return nodeObj;
}

void CustomNetworkNode::setNodeStyle() {
	QColor base, back, border;
	QList<double> saturation, brightness; //{base, back, border}
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

	qan::NodeStyle s = getItem()->getStyle();
	s.setBackColor(back);
	s.setBaseColor(base);
	s.setBorderColor(border);

	getItem()->setItemStyle(&s);
}

void CustomNetworkNode::setNodeShape() {
	if(m_shapeSet)
		return;

	getItem()->setResizable(false);

	/* Generates round bounding polygon so
	 * there is never a gap between edges
	 * and nodes
	 */
	QPainterPath path;
	qreal shapeRadius = 100.;   // In percentage = 100% !
	path.addRoundedRect(QRectF{ 0., 0., NODE_DIMEN, NODE_DIMEN}, shapeRadius, shapeRadius);
	QPolygonF boundingShape =  path.toFillPolygon(QTransform{});
	getItem()->setBoundingShape(boundingShape);

	style()->setBackRadius(NODE_RADIUS);
	style()->setBorderWidth(2);

	style()->setFillType(qan::NodeStyle::FillType::FillGradient);
	style()->setBackOpacity(70);

	style()->setEffectType(qan::NodeStyle::EffectType::EffectGlow);
	style()->setEffectColor("black");
	style()->setEffectRadius(7);

	m_shapeSet = true;
}
