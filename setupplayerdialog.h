#ifndef SETUPPLAYERDIALOG_H
#define SETUPPLAYERDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QPushButton>

#include "utils.h"
#include "waitforotherplayer.h"

namespace Ui {
class SetupPlayerDialog;
}

class SetupPlayerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupPlayerDialog(QWidget *parent = nullptr);
    ~SetupPlayerDialog();

	PlayerType getPlayer() const;
	QTcpSocket *getUpstreamSocket() const;
	QTcpSocket *getDownstreamSocket() const;
	void setWinner(const QString &info);
	void setPlayer(PlayerType player);

private slots:
	
	void setUpConnection();

private:
    Ui::SetupPlayerDialog *ui;
	WaitForOtherPlayer *setup;	
	PlayerType defaultPlayer;
};

#endif // SETUPPLAYERDIALOG_H
