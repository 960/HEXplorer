#ifndef DIALOGBITDISPLAY_H
#define DIALOGBITDISPLAY_H

#include "ui_dialogbitdisplay.h"
#include <bitset>

using namespace std;

class DialogBitDisplay : public QDialog, private Ui::DialogBitDisplay
{
    Q_OBJECT

public:
    explicit DialogBitDisplay(ulong & value, int length, QWidget *parent = 0);

private slots:
    void on_buttonBox_accepted();

private:
    QList<QRadioButton*> listRadio;
    bitset<32> myBitset;
    ulong *myValue;
};

#endif // DIALOGBITDISPLAY_H
