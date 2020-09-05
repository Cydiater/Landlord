#include "landlordview.h"
#include "waitforotherplayer.h"
#include "setupplayerdialog.h"
#include "utils.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	SetupPlayerDialog *setup {nullptr};
	LandlordView *view {nullptr};
	PlayerType defaultPlayer = PlayerType::undefined;
	QString info = "";

	while (true) {
		setup = new SetupPlayerDialog;
		if (info.length() > 0)
			setup -> setWinner(info);
		if (defaultPlayer != PlayerType::undefined)
			setup -> setPlayer(defaultPlayer);
		setup -> show();

		if (setup -> exec() == QDialog::Rejected) {
			qDebug() << "setup rejected";
			return 0;
		}

		qDebug() << "player = " << playerTypeLiteral[static_cast<int>(setup -> getPlayer())];
		defaultPlayer = setup -> getPlayer();

		view = new LandlordView(nullptr, setup -> getPlayer(), setup -> getUpstreamSocket(), setup -> getDownstreamSocket());
		delete setup;

		view -> show();
		a.exec();

		info = view -> getWinner();
		delete view;
	}

	return  0;
}
