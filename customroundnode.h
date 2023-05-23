#ifndef CUSTOMROUNDNODE_H
#define CUSTOMROUNDNODE_H

#include <QObject>
#include <QuickQanava>

class CustomRoundNode : public QObject, public qan::NodeItem
{
	Q_OBJECT
  public:
	explicit CustomRoundNode(QObject *parent = nullptr);
	qan::NodeStyle styleType()


  signals:

};

#endif // CUSTOMROUNDNODE_H
