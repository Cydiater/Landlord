#include <stdexcept>

#include "setupplayerdialog.h"
#include "ui_setupplayerdialog.h"

SetupPlayerDialog::SetupPlayerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupPlayerDialog),
    setup(new WaitForOtherPlayer(this)), 
	defaultPlayer {PlayerType::undefined} {
    ui -> setupUi(this);
	ui -> AButton -> setChecked(true);
	connect(ui -> buttonBox, SIGNAL(accepted()), SLOT(setUpConnection()));
}

SetupPlayerDialog::~SetupPlayerDialog() {
    delete ui;
}

void SetupPlayerDialog::setPlayer(PlayerType player) {
	defaultPlayer = player;
	setWindowTitle(playerTypeLiteral[static_cast<int>(player)]);
}

PlayerType SetupPlayerDialog::getPlayer() const {
	if (defaultPlayer != PlayerType::undefined)
		return defaultPlayer;

	if (ui -> AButton -> isChecked())
		return PlayerType::A;
	if (ui -> BButton -> isChecked())
		return PlayerType::B;
	if (ui -> CButton -> isChecked())
		return PlayerType::C;
	throw std::runtime_error("bad player type");
}

void SetupPlayerDialog::setUpConnection() {
	ui -> buttonBox -> button(QDialogButtonBox::Ok) -> setEnabled(false);
	setup -> setPlayerType(getPlayer());
	connect(setup, SIGNAL(gameStart()), SLOT(accept()));
}

QTcpSocket *SetupPlayerDialog::getUpstreamSocket() const {
	return setup -> upstreamSocket();
}

QTcpSocket *SetupPlayerDialog::getDownstreamSocket() const {
	return setup -> downstreamSocket();
}

void SetupPlayerDialog::setWinner(const QString &info) {
	ui -> groupBox -> deleteLater();
	ui -> label -> setText(info);
	ui -> buttonBox -> button(QDialogButtonBox::Ok) -> setText("Restart");
}
