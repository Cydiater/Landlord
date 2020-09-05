#include <sstream>

#include <QPropertyAnimation>
#include <QMessageBox>
#include <QGraphicsSceneHoverEvent>
#include <QParallelAnimationGroup>
#include <QTimer>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QGraphicsTextItem>
#include <QTextDocument>
#include <QBitmap>
#include <QPainter>

#include "landlordview.h"
#include "ui_landlordview.h"

const int PokerView::width = 100;
const int PokerView::height = 140;
const int PokerView::overlap = PokerView::width / 3;
const qreal PokerView::scaleRate = 0.5;
const int PokerView::verticalOverlap = PokerView::width * PokerView::scaleRate / 3;
const int PokerView::hoverGap = PokerView::height / 3;
const int PokerView::animationDuration = 50;
const QString PokerView::backImagePath = ":/PokerImage/back.png";

const int LandlordView::animationDuration = 100;
const int LandlordView::margin = 20;

const QSize LandlordView::msgSize = QSize(250, 40);
const QSize LandlordView::buttonSize = QSize(80, 40);
const QSize LandlordView::viewSize = QSize(1000, 800);

const QColor LandlordView::msgBackgroundColor = QColor("#555555");

const QPointF LandlordView::upstreamMsgPos = QPointF(200, 50);
const QPointF LandlordView::downstreamMsgPos = QPointF(800 - msgSize.width(), 50);
const QPointF LandlordView::myMsgPos = QPointF((LandlordView::viewSize.width() - LandlordView::msgSize.width()) / 2, 
												LandlordView::viewSize.height() / 2 + 80);
const QPointF LandlordView::leftButtonPos = QPointF(myMsgPos.x() + msgSize.width() + 20, myMsgPos.y());
const QPointF LandlordView::rightButtonPos = QPointF(myMsgPos.x() - 20 - LandlordView::buttonSize.width(), myMsgPos.y());
const QPointF LandlordView::upperPos = QPointF(LandlordView::viewSize.width() / 2 - 40, LandlordView::margin);

PokerView::PokerView(QGraphicsItem *parent, const Poker &_poker) : 
	QGraphicsPixmapItem(parent),
	poker {_poker},
	imagePath {QString(":/PokerImage/%1").arg(poker.getImageName())}, 
	isUp {true},
	isSelec {false},
	canSelect {true} {
	QPixmap image(imagePath);
	setPixmap(image.scaled(QSize(width, height)));
}

Poker PokerView::getPoker() const {
	return poker;
}

void PokerView::flip() {
	QPixmap image(isUp ? backImagePath : imagePath);
	setPixmap(image.scaled(QSize(width, height)));
	isUp ^= 1;
	if (isUp == false)
		canSelect = false;
}

bool PokerView::isUpside() const {
	return isUp;
}

void PokerView::setUpperStyle() {
	canSelect = false;
	setScale(PokerView::scaleRate);
	if (isUp == false)
		flip();
}

void PokerView::setUpstreamStyle() {
	canSelect = false;
	setRotation(-90);
	setScale(PokerView::scaleRate);
	if (isUp == true)
		flip();
}

void PokerView::setDownstreamStyle() {
	canSelect = false;
	setRotation(90);
	setScale(PokerView::scaleRate);
	if (isUp == true)
		flip();
}

void PokerView::setNormalStyle() {
	setRotation(0);
	setScale(1);
	if (isUp == false)
		flip();
}

void PokerView::setStdpos(const QPointF &_stdpos) {
	stdpos = _stdpos;
}

void PokerView::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
	QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");

	animation -> setDuration(animationDuration);
	animation -> setEndValue(stdpos + QPointF(0, -hoverGap));

	animation -> start();;
}

void PokerView::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
	if (isSelec) return;

	QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");

	animation -> setDuration(animationDuration);
	animation -> setEndValue(stdpos);

	animation -> start();;
}

void PokerView::mousePressEvent(QGraphicsSceneMouseEvent *) {
	if (!canSelect)
		return;

	if (isSelected())
		disselect();
	else 
		select();

	qDebug() << poker.toString() << " got selected";
}

bool PokerViewPtrCmp::operator()(const PokerView *lhs, const PokerView *rhs) const {
	return lhs -> getPoker() < rhs -> getPoker();
}

void PokerView::select() {
	isSelec = true;
	hoverEnterEvent(nullptr);
}

void PokerView::disselect() {
	isSelec = false;
	hoverLeaveEvent(nullptr);
}

bool PokerView::isSelected() const {
	return isSelec;
}

void LandlordView::confMsg(QLabel *msgLabel) {
	msgLabel -> setFixedSize(msgSize);

	QPalette msgTheme;
	msgTheme.setColor(QPalette::Background, msgBackgroundColor);

	msgLabel -> setPalette(msgTheme);
	msgLabel -> setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	QFont font("Monaco", 10);
	font.setBold(true);
	font.setItalic(true);

	msgLabel -> setFont(font);
} 

LandlordView::LandlordView(QWidget *parent, PlayerType _player, QTcpSocket *_upstreamSocket, QTcpSocket *_downstreamSocket) :
      QMainWindow(parent), 
	  ui(new Ui::LandlordView), 
	  core {new LandlordCore},
	  network {new LandlordNetwork(nullptr, _upstreamSocket, _downstreamSocket)},
	  scene {new QGraphicsScene},
	  upstreamMsg {new QLabel}, 
	  downstreamMsg {new QLabel}, 
	  myMsg {new QLabel} {

    ui -> setupUi(this);
	ui -> pokerArea -> setStyleSheet("background-color:transparent");
	ui -> pokerArea -> setScene(scene);
	core -> setPlayer(_player);
	setFixedSize(viewSize);

	setWindowTitle(playerTypeLiteral[static_cast<int>(_player)]);

	confMsg(myMsg);
	confMsg(upstreamMsg);
	confMsg(downstreamMsg);

	auto proxyMyMsg = scene -> addWidget(myMsg);
	auto proxyUpstreamMsg = scene -> addWidget(upstreamMsg);
	auto proxyDownsteramMsg = scene -> addWidget(downstreamMsg);

	proxyMyMsg -> setPos(myMsgPos);
	proxyUpstreamMsg -> setPos(upstreamMsgPos);
	proxyDownsteramMsg -> setPos(downstreamMsgPos);

	proxyMyMsg -> setZValue(1e9);
	proxyUpstreamMsg -> setZValue(1e9);
	proxyDownsteramMsg -> setZValue(1e9);

	connect(network, SIGNAL(receiveFromUpstream(const PokerGroup &, NetworkMsg, int)),
						    SLOT(msgRouter(const PokerGroup &, NetworkMsg, int)));
	connect(network, SIGNAL(receiveFromDownstream(const PokerGroup &, NetworkMsg, int)),
						    SLOT(msgRouter(const PokerGroup &, NetworkMsg, int)));

	QTimer *timer = new QTimer(nullptr);
	timer -> setSingleShot(true);
	if (core -> getPlayer() == PlayerType::A) 
		connect(timer, SIGNAL(timeout()), SLOT(initPokers()));
	else 
		connect(timer, SIGNAL(timeout()), SLOT(initScene()));

	timer -> start();
}

void LandlordView::updateMsg(PlayerType player, const QString &msg) {
	QString id = player == landlord ? "Landlord: " : "Farmer: ";

	if (msg == "call" || msg == "pass" || msg == "rob")
		id = "";

	if (player == core -> getPlayer()) 
		myMsg -> setText(id + msg + " | " + QString::number(myPokers.size()));

	if (player == core -> getUpstreamPlayer())
		upstreamMsg -> setText(id + msg + " | " + QString::number(upstreamPokers.size()));

	if (player == core -> getDownstreamPlayer())
		downstreamMsg -> setText(id + msg + " | " + QString::number(downstreamPokers.size()));
}

void LandlordView::clearMsg() {
	updateMsg(PlayerType::A, "");
	updateMsg(PlayerType::B, "");
	updateMsg(PlayerType::C, "");
}

QPointF LandlordView::getMyAreaLeftUpPoint() const {
	QRectF sceneRect = scene -> sceneRect();
	qreal pokerWidth = PokerView::width + PokerView::overlap * (static_cast<qreal>(myPokers.size()) - 1);
	qreal x = (sceneRect.width() - pokerWidth) / 2;
	qreal y = (sceneRect.height() - PokerView::height - margin);
	return QPointF(x, y);
}

QPointF LandlordView::getMyAreaMidUpPoint() const {
	QRectF sceneRect = scene -> sceneRect();
	qreal x = sceneRect.width() / 2;
	qreal y = (sceneRect.height() - PokerView::height - margin * 3);
	return QPointF(x, y);
}

QPointF LandlordView::getCentralAreaLeftUpPoint() const {
	QRectF sceneRect = scene -> sceneRect();
	qreal pokerWidth = PokerView::width + PokerView::overlap * (static_cast<qreal>(centralPokers.size()) - 1);
	qreal x = (sceneRect.width() - pokerWidth) / 2;
	qreal y = (sceneRect.height() - PokerView::height - margin * 2) / 2;
	return QPointF(x, y);
}

QPointF LandlordView::getDownstreamAreaLeftUpPoint() const {
	return QPointF(margin + PokerView::height * PokerView::scaleRate, margin);
}

QPointF LandlordView::getDownstreamAreaRightPoint() const {
	return QPointF(margin + PokerView::height * PokerView::scaleRate + 20, margin);
}

QPointF LandlordView::getUpstreamAreaLeftUpPoint() const {
	QRectF sceneRect = scene -> sceneRect();
	return QPointF(sceneRect.width() - PokerView::height * PokerView::scaleRate, PokerView::height * PokerView::scaleRate);
}

QPointF LandlordView::getUpstreamAreaRightPoint() const {
	QRectF sceneRect = scene -> sceneRect();
	return QPointF(sceneRect.width() - PokerView::height * PokerView::scaleRate - 50, PokerView::height * PokerView::scaleRate);
}

void LandlordView::initScene() {
	scene -> setSceneRect( ui -> pokerArea -> geometry() );
}

void LandlordView::initPokers() {/*{{{*/
	initScene();
	assert(core -> getPlayer() == PlayerType::A);

	PokerGroup allPokers = PokerGroup::genFullPokers();
	allPokers.shuffle();

	int size = static_cast<int>(allPokers.size());
	
	for (int i = 0; i < size - 3; i++) {
		PokerGroup single;
		single.addPoker(allPokers.group[i]);

		if (i % 3 == 0) {
			network -> send2Upstream(single, NetworkMsg::distributeToDownstream);
			network -> send2Downstream(single, NetworkMsg::distributeToUpstream);
			msgRouter(single, NetworkMsg::distributeToMe);
		}

		if (i % 3 == 1) {
			network -> send2Upstream(single, NetworkMsg::distributeToUpstream);
			network -> send2Downstream(single, NetworkMsg::distributeToMe);
			msgRouter(single, NetworkMsg::distributeToDownstream);
		}

		if (i % 3 == 2) {
			network -> send2Upstream(single, NetworkMsg::distributeToMe);
			network -> send2Downstream(single, NetworkMsg::distributeToDownstream);
			msgRouter(single, NetworkMsg::distributeToUpstream);
		}
	}

	PokerGroup bonus;
	for (int i = size - 3; i < size; i++) 
		bonus.addPoker(allPokers.group[i]);

	network -> send2Upstream(bonus, NetworkMsg::setBonus);
	network -> send2Downstream(bonus, NetworkMsg::setBonus);
	msgRouter(bonus, NetworkMsg::setBonus);
}/*}}}*/

void LandlordView::updateMyAreaLayout() {/*{{{*/
	if (myPokers.empty())
		return;

	QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup;
	QPointF pos = getMyAreaLeftUpPoint();
	for (auto pokerView: myPokers) {

		pokerView -> setStdpos(pos);
		pokerView -> setZValue(pos.x());
		QPropertyAnimation *animation = new QPropertyAnimation(pokerView, "pos");

		animation -> setDuration(LandlordView::animationDuration);
		animation -> setEndValue(pos);
		animationGroup -> addAnimation(animation);

		pos += QPointF(PokerView::overlap, 0.0);
	}

	QEventLoop animationBlock;
	connect(animationGroup, SIGNAL(finished()), &animationBlock, SLOT(quit()));

	animationGroup -> start();
	animationBlock.exec();

	for (auto pokerView: myPokers)
		if (!pokerView -> isUpside())
			pokerView -> flip();
}/*}}}*/

void LandlordView::updateCentralAreaLayout() {/*{{{*/
	if (centralPokers.empty()) 
		return;

	QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup;
	QPointF pos = getCentralAreaLeftUpPoint();
	for (auto pokerView: centralPokers) {

		QPropertyAnimation *animation = new QPropertyAnimation(pokerView, "pos");
		pokerView -> setZValue(pos.x());

		animation -> setDuration(LandlordView::animationDuration);
		animation -> setEndValue(pos);
		animationGroup -> addAnimation(animation);

		pos += QPointF(PokerView::overlap, 0.0);
	}

	QEventLoop animationBlock;
	connect(animationGroup, SIGNAL(finished()), &animationBlock, SLOT(quit()));

	animationGroup -> start();
	animationBlock.exec();
}/*}}}*/

void LandlordView::updateDownstreamAreaLayout() {/*{{{*/
	if (downstreamPokers.empty())
		return;

	QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup;
	QPointF pos = getDownstreamAreaLeftUpPoint();
	for (auto pokerView: downstreamPokers) {

		QPropertyAnimation *animation = new QPropertyAnimation(pokerView, "pos");

		pokerView -> setZValue(pos.y());

		animation -> setDuration(LandlordView::animationDuration);
		animation -> setEndValue(pos);
		animationGroup -> addAnimation(animation);

		pos += QPointF(0.0, PokerView::verticalOverlap);
	}

	QEventLoop animationBlock;
	connect(animationGroup, SIGNAL(finished()), &animationBlock, SLOT(quit()));

	animationGroup -> start();
	animationBlock.exec();
}/*}}}*/

void LandlordView::updateUpstreamAreaLayout() {/*{{{*/
	if (upstreamPokers.empty())
		return;

	QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup;
	QPointF pos = getUpstreamAreaLeftUpPoint();
	for (auto pokerView: upstreamPokers) {

		QPropertyAnimation *animation = new QPropertyAnimation(pokerView, "pos");

		pokerView -> setZValue(pos.y());

		animation -> setDuration(LandlordView::animationDuration);
		animation -> setEndValue(pos);
		animationGroup -> addAnimation(animation);

		pos += QPointF(0.0, PokerView::verticalOverlap);
	}

	QEventLoop animationBlock;
	connect(animationGroup, SIGNAL(finished()), &animationBlock, SLOT(quit()));

	animationGroup -> start();
	animationBlock.exec();
}/*}}}*/

void LandlordView::setUpperAreaLayout() {/*{{{*/
	if (upperPokers.empty()) 
		return;

	QPointF pos = upperPos;
	for (auto pokerView: upperPokers) {

		pokerView -> setZValue(pos.x());
		pokerView -> setPos(pos);
		pokerView -> setUpperStyle();

		scene -> addItem(pokerView);

		pos += QPointF(PokerView::overlap * PokerView::scaleRate, 0.0);
	}

}/*}}}*/

PokerView* LandlordView::addPoker2Central(const Poker &poker) {/*{{{*/
	PokerView *pokerView = new PokerView(nullptr, poker);
	pokerView -> flip();
	pokerView -> setPos(getCentralAreaLeftUpPoint());
	pokerView -> canSelect = false; 

	scene -> addItem(pokerView);
	centralPokers.insert(pokerView);

	updateCentralAreaLayout();
	return pokerView;
}/*}}}*/

void LandlordView::handleCall() {/*{{{*/
	Call4Landlord::status status = callButton -> text() == "call" ? Call4Landlord::status::call : Call4Landlord::status::rob;
	callButton -> deleteLater();
	passButton -> deleteLater();
	updateCallPass(core -> getPlayer(), status);
}/*}}}*/

void LandlordView::handlePass() {/*{{{*/
	callButton -> deleteLater();
	passButton -> deleteLater();
	updateCallPass(core -> getPlayer(), Call4Landlord::status::pass);
}/*}}}*/

void LandlordView::handleFollow() {/*{{{*/

	PokerGroup selectedCards, centralCards;

	for (auto pokerView: myPokers)
		if (pokerView -> isSelected())
			selectedCards.addPoker(pokerView -> getPoker());

	for (auto pokerView: centralPokers)
		centralCards.addPoker(pokerView -> getPoker());

	ComboType type = core -> getComboType(selectedCards.toComboMeta());
	if (!core -> isBigger(selectedCards, centralCards) && !(lastPlayer == core -> getPlayer() && type != ComboType::undefined)) {
		QMessageBox::warning(this, "Invaild", "This combo is not vaild");
		return;	
	}


	msgRouter(PokerGroup(), NetworkMsg::eraseCentral);
	network -> send2Upstream(PokerGroup(), NetworkMsg::eraseCentral);
	network -> send2Downstream(PokerGroup(), NetworkMsg::eraseCentral);

	msgRouter(selectedCards, NetworkMsg::updateCentralCards, static_cast<int>(core -> getPlayer()));
	network -> send2Upstream(selectedCards, NetworkMsg::updateCentralCards, static_cast<int>(core -> getPlayer()));
	network -> send2Downstream(selectedCards, NetworkMsg::updateCentralCards, static_cast<int>(core -> getPlayer()));

	network -> send2Downstream(PokerGroup(), NetworkMsg::playerChoose);

	followButton -> deleteLater();
	rejectButton -> deleteLater();
}/*}}}*/

void LandlordView::handleReject() {/*{{{*/

	for (auto pokerView: myPokers) 
		pokerView -> disselect();

	msgRouter(PokerGroup(), NetworkMsg::updateCentralCards, static_cast<int>(core -> getPlayer()));
	network -> send2Upstream(PokerGroup(), NetworkMsg::updateCentralCards, static_cast<int>(core -> getPlayer()));
	network -> send2Downstream(PokerGroup(), NetworkMsg::updateCentralCards, static_cast<int>(core -> getPlayer()));

	network -> send2Downstream(PokerGroup(), NetworkMsg::playerChoose);

	followButton -> deleteLater();
	rejectButton -> deleteLater();
}/*}}}*/

void LandlordView::okChoose() {/*{{{*/

	QPushButton *follow = new QPushButton("follow", nullptr),
				*reject = new QPushButton("reject", nullptr);

	follow -> resize(buttonSize);
	reject -> resize(buttonSize);

	if (lastPlayer == core -> getPlayer())
		reject -> setEnabled(false);

	connect(follow, SIGNAL(clicked()), SLOT(handleFollow()));
	connect(reject, SIGNAL(clicked()), SLOT(handleReject()));

	followButton = follow;
	rejectButton = reject;

	auto button = scene -> addWidget(follow);
	button -> setPos(leftButtonPos);

	button = scene -> addWidget(reject);
	button -> setPos(rightButtonPos);
}/*}}}*/

void LandlordView::updateCallPass(PlayerType player, Call4Landlord::status w, bool isFirst) {/*{{{*/

	QString status;

	if (w == Call4Landlord::status::call)
		status = "call";

	if (w == Call4Landlord::status::rob)
		status = "rob";

	if (w == Call4Landlord::status::pass)
		status = "pass";

	updateMsg(player, status);

	if (core -> set(player, w)) {
		PlayerType candidate = core -> getCandidate();

		std::vector<PokerView*> tmp {centralPokers.begin(), centralPokers.end()};

		lastPlayer = core -> getCandidate();

		landlord = core -> getCandidate();

		for (auto pokerView: centralPokers) {
			PokerView *fixedView = new PokerView(nullptr, pokerView -> getPoker());
			upperPokers.insert(fixedView);
		}

		setUpperAreaLayout();

		for (auto pokerView: tmp) {
			if (candidate == core -> getPlayer()) {
				pokerView -> setNormalStyle();
				pokerView -> canSelect = true;
				myPokers.insert(pokerView);
				pokerView -> setAcceptHoverEvents(true);
				updateMyAreaLayout();
				centralPokers.erase(pokerView);
				updateCentralAreaLayout();
			} 

			if (candidate == core -> getUpstreamPlayer()) {
				pokerView -> setUpstreamStyle();
				upstreamPokers.insert(pokerView);
				updateUpstreamAreaLayout();
				centralPokers.erase(pokerView);
				updateCentralAreaLayout();
			} 
			
			if (candidate == core -> getDownstreamPlayer()) {
				pokerView -> setDownstreamStyle();
				downstreamPokers.insert(pokerView);
				updateDownstreamAreaLayout();
				centralPokers.erase(pokerView);
				updateCentralAreaLayout();
			}
		}

		clearMsg();

		if (core -> getPlayer() == candidate) 
			okChoose();

	} else if (isFirst) 
		network -> send2Downstream(PokerGroup(), NetworkMsg::playerCallPass);

	if (isFirst) {
		int value = static_cast<int>(w) * 10 + static_cast<int>(player);
		network -> send2Upstream(PokerGroup(), NetworkMsg::playerCallPassSelected, value);
		network -> send2Downstream(PokerGroup(), NetworkMsg::playerCallPassSelected, value);
	}
}/*}}}*/

void LandlordView::msgRouter(const PokerGroup &group, NetworkMsg msg, int value) {/*{{{*/
	switch (msg) {

		case NetworkMsg::distributeToMe: {
			for (auto &poker: group.group) {
				qDebug() << "distribute to me " << poker.toString();
				PokerView *pokerView = addPoker2Central(poker);
				centralPokers.erase(pokerView);
				pokerView -> canSelect = true;
				myPokers.insert(pokerView);
				updateMyAreaLayout();
				pokerView -> setAcceptHoverEvents(true);
			}
			break;
		}

		case NetworkMsg::distributeToDownstream: {
			for (auto &poker: group.group) {
				qDebug() << "distribute to Downstream " << poker.toString();
				PokerView *pokerView = addPoker2Central(poker);
				centralPokers.erase(pokerView);

				pokerView -> setDownstreamStyle();

				downstreamPokers.insert(pokerView);
				updateDownstreamAreaLayout();
			}
			break;
		}

		case NetworkMsg::distributeToUpstream: {
			for (auto &poker: group.group) {
				qDebug() << "distribute to Upstream " << poker.toString();
				PokerView *pokerView = addPoker2Central(poker);
				centralPokers.erase(pokerView);
				
				pokerView -> setUpstreamStyle();
				pokerView -> canSelect = true;

				upstreamPokers.insert(pokerView);
				updateUpstreamAreaLayout();
			}
			break;
		}

		case NetworkMsg::setBonus: {

			for (auto &poker: group.group) {
				qDebug() << "get bonus";
				addPoker2Central(poker);
			}

			if (core -> getPlayer() == PlayerType::A) {
				PlayerType origin = Call4Landlord::random();				
				if (origin == PlayerType::A)
					msgRouter(PokerGroup(), NetworkMsg::playerCallPass, 1);
				if (origin == PlayerType::B)
					network -> send2Downstream(PokerGroup(), NetworkMsg::playerCallPass, 1);
				if (origin == PlayerType::C)
					network -> send2Upstream(PokerGroup(), NetworkMsg::playerCallPass, 1);
			}
			break;
		}

		case NetworkMsg::playerCallPass: {

			ui -> pokerArea -> setFocus(Qt::MouseFocusReason);

			QPushButton *call = new QPushButton(value == 0 ? "rob" : "call", nullptr),
						*pass = new QPushButton("pass", nullptr);

			call -> resize(buttonSize);
			pass -> resize(buttonSize);

			connect(call, SIGNAL(clicked()), SLOT(handleCall()));
			connect(pass, SIGNAL(clicked()), SLOT(handlePass()));
			callButton = call;
			passButton = pass;

			auto button = scene -> addWidget(call);
			button -> setPos(leftButtonPos);

			button = scene -> addWidget(pass);
			button -> setPos(rightButtonPos);

			break;
	    }

		case NetworkMsg::playerCallPassSelected: {
			PlayerType player = static_cast<PlayerType>(value % 10);
			Call4Landlord::status w = static_cast<Call4Landlord::status>(value / 10);

			updateCallPass(player, w, false);	
			break;
		}

		case NetworkMsg::updateCentralCards: {
			PlayerType from = static_cast<PlayerType>(value);
			for (auto poker: group.group) {

				PokerView *item = nullptr;

				if (from == core -> getUpstreamPlayer()) {
					for (auto pokerView: upstreamPokers)
						if (pokerView -> getPoker() == poker)
							item = pokerView;

					upstreamPokers.erase(item);
					updateUpstreamAreaLayout();
				}

				if (from == core -> getDownstreamPlayer()) {
					for (auto pokerView: downstreamPokers)
						if (pokerView -> getPoker() == poker)
							item = pokerView;

					downstreamPokers.erase(item);
					updateDownstreamAreaLayout();
				}

				if (from == core -> getPlayer()) {
					for (auto pokerView: myPokers)
						if (pokerView -> getPoker() == poker)
							item = pokerView;

					myPokers.erase(item);
					updateMyAreaLayout();
				}

				item -> setAcceptHoverEvents(false);
				item -> setNormalStyle();
				item -> canSelect = false;
				centralPokers.insert(item);
				updateCentralAreaLayout();
			}

			if (group.size() > 0) {
				ComboType type = core -> getComboType(group.toComboMeta());
				int sub = core -> getSubComboType(group.toComboMeta(), type);
				updateMsg(from, ComboLiteral[static_cast<int>(type)][sub]);
				lastPlayer = from;
			} else 
				updateMsg(from, "rejected");

			if (upstreamPokers.empty()) {
				winner = core -> getUpstreamPlayer();
				close();
			}

			if (downstreamPokers.empty()) {
				winner = core -> getDownstreamPlayer();
				close();
			}

			if (myPokers.empty()) {
				winner = core -> getPlayer();
				close();
			}

			break;
		}

		case NetworkMsg::playerChoose: {
			okChoose();			
			break;
	    }

		case NetworkMsg::eraseCentral: {
			for (auto pokerView: centralPokers) {
				scene -> removeItem(pokerView);
				pokerView -> deleteLater();
			}
			centralPokers.clear();
			updateCentralAreaLayout();
			break;
	    }		

	}
}/*}}}*/

QString LandlordView::getWinner() const {
	if (winner == landlord) {
		if (core -> getPlayer() == landlord)
			return "You Win!";
		return "You Lose";
	} else {
		if (core -> getPlayer() == landlord)
			return "You Lose";
		return "You Win!";
	}
}

LandlordView::~LandlordView() {
    delete ui;
}
