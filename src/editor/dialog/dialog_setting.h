#ifndef DIALOG_SETTING_H
#define DIALOG_SETTING_H

#include <QDialog>

namespace Ui {
class DialogSetting;
}

class DialogSetting : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetting(QWidget *parent = nullptr);
    ~DialogSetting();

private:
    Ui::DialogSetting *ui;
};

#endif // DIALOG_SETTING_H
