#include <stdexcept>

#include "whichplayerdialog.h"
#include "ui_whichplayerdialog.h"

WhichPlayerDialog::WhichPlayerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WhichPlayerDialog) {
    ui -> setupUi(this);
	ui -> AButton -> setChecked(true);
}

WhichPlayerDialog::~WhichPlayerDialog() {
    delete ui;
}

PlayerType WhichPlayerDialog::getPlayer() const {
	if (ui -> AButton -> isChecked())
		return PlayerType::A;
	if (ui -> BButton -> isChecked())
		return PlayerType::B;
	if (ui -> CButton -> isChecked())
		return PlayerType::C;
	throw std::runtime_error("bad player type");
}
