#ifndef OPENRGBSYSTEMINFOPAGE_H
#define OPENRGBSYSTEMINFOPAGE_H

#include <QFrame>
#include "ui_OpenRGBSystemInfoPage.h"
#include "i2c_smbus.h"

namespace Ui {
class OpenRGBSystemInfoPage;
}

class Ui::OpenRGBSystemInfoPage : public QFrame
{
    Q_OBJECT

public:
    explicit OpenRGBSystemInfoPage(std::vector<i2c_smbus_interface *>& bus, QWidget *parent = nullptr);
    ~OpenRGBSystemInfoPage();

public slots:
    void UpdateBusList();

private slots:
    void on_DetectButton_clicked();

    void on_DumpButton_clicked();

    void on_ReadButton_clicked();

    void on_ConstantRead_clicked();

    void on_SwitchConstToCurrent_clicked();

private:
    Ui::OpenRGBSystemInfoPageUi *ui;
    std::vector<i2c_smbus_interface *>& busses;

    bool ConstantRead = false;
    i2c_smbus_interface* ConstBus = nullptr;
    uchar Constaddr   = -1;
    uchar ConstReg    = -1;
    uchar ConstSize   = -1;


    void CreateReadThread();
};

#endif // OPENRGBSYSTEMINFOPAGE_H
