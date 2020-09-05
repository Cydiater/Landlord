#ifndef WHICHPLAYERDIALOG_H
#define WHICHPLAYERDIALOG_H

#include <QDialog>

#include "utils.h"

namespace Ui {
class WhichPlayerDialog;
}

class WhichPlayerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WhichPlayerDialog(QWidget *parent = nullptr);
    ~WhichPlayerDialog();

	PlayerType getPlayer() const;

private:
    Ui::WhichPlayerDialog *ui;
};

#endif // WHICHPLAYERDIALOG_H
