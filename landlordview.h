#ifndef LANDLORDVIEW_H
#define LANDLORDVIEW_H

#include <set>

#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>
#include <QGraphicsTextItem>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QPushButton>

#include "waitforotherplayer.h"
#include "landlordcore.h"
#include "landlordnetwork.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LandlordView; }
QT_END_NAMESPACE

class PokerView : public QObject, public QGraphicsPixmapItem {
	Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

	Poker poker;
	QString imagePath;
	QPointF stdpos;
	bool isUp;
	bool isSelec;

public:

	bool canSelect;

	static const int width;
	static const int height;
	static const int overlap;
	static const int verticalOverlap;
	static const int hoverGap;
	static const int animationDuration;

	static const qreal scaleRate;

	static const QString backImagePath;


	PokerView(QGraphicsItem *parent, const Poker &_poker);
	~PokerView() = default;

	Poker getPoker() const;
	void setStdpos(const QPointF &_stdpos);
	void flip();
	bool isUpside() const;

	void setUpstreamStyle();
	void setUpperStyle();
	void setDownstreamStyle();
	void setNormalStyle();

	void select();
	void disselect();
	bool isSelected() const;

protected:

	void hoverEnterEvent(QGraphicsSceneHoverEvent *);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
	void mousePressEvent(QGraphicsSceneMouseEvent *);
};

struct PokerViewPtrCmp {
	bool operator()(const PokerView *lhs, const PokerView *rhs) const;
};

class LandlordView : public QMainWindow {
    Q_OBJECT

public:
    LandlordView(QWidget *parent, PlayerType _player, QTcpSocket *_upstreamSocket, QTcpSocket *);
    ~LandlordView();

	QString getWinner() const;

private slots:

	void initPokers();
	void initScene();
	void msgRouter(const PokerGroup &group, NetworkMsg msg, int value = 0);

	void handleCall();
	void handlePass();
	void handleFollow();
	void handleReject();

private:

	static const int animationDuration;
	static const int margin;

	static const QPointF upstreamMsgPos;
	static const QPointF downstreamMsgPos;
	static const QPointF myMsgPos;
	static const QPointF leftButtonPos;
	static const QPointF rightButtonPos;
	static const QPointF upperPos; 

	static const QSize msgSize;
	static const QSize buttonSize;
	static const QSize viewSize;

	static const QColor msgBackgroundColor;

	void confMsg(QLabel *msgLabel);
	void updateCallPass(PlayerType player, Call4Landlord::status w, bool isFirst = true);
	void updateMsg(PlayerType player, const QString &msg);

	void updateCentralAreaLayout();
	void updateMyAreaLayout();
	void updateDownstreamAreaLayout();
	void updateUpstreamAreaLayout();
	void setUpperAreaLayout();

	void clearMsg();
	void okChoose();

	QPointF getMyAreaLeftUpPoint() const;
	QPointF getMyAreaMidUpPoint() const;

	QPointF getCentralAreaLeftUpPoint() const;

	QPointF getDownstreamAreaLeftUpPoint() const;
	QPointF getDownstreamAreaRightPoint() const;

	QPointF getUpstreamAreaLeftUpPoint() const;
	QPointF getUpstreamAreaRightPoint() const;

	PokerView* addPoker2Central(const Poker &poker);

	void move2MyArea();

    Ui::LandlordView *ui;
	LandlordCore *core;
	LandlordNetwork *network;
	QGraphicsScene *scene;
	QPushButton *callButton, *passButton, *followButton, *rejectButton;
	QLabel *upstreamMsg, *downstreamMsg, *myMsg;
	PlayerType lastPlayer, landlord, winner;

	std::set<PokerView*, PokerViewPtrCmp> myPokers, 
										  centralPokers, 
										  upstreamPokers, 
										  downstreamPokers,
										  upperPokers;
};
#endif // LANDLORDVIEW_H
