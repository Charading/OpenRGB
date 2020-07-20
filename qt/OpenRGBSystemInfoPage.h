#ifndef OPENRGBSYSTEMINFOPAGE_H
#define OPENRGBSYSTEMINFOPAGE_H

#include <QFrame>
#include "ui_OpenRGBSystemInfoPage.h"
#include "i2c_smbus.h"
#ifdef WIN32
#include "i2c_smbus_nvapi.h"
#endif

namespace Ui {
class OpenRGBSystemInfoPage;
}

class Ui::OpenRGBSystemInfoPage : public QFrame
{
    Q_OBJECT

public:
    explicit OpenRGBSystemInfoPage(std::vector<i2c_smbus_interface *>& bus, QWidget *parent = nullptr);
    ~OpenRGBSystemInfoPage();

private slots:
    void on_DetectButton_clicked();

    void on_DumpButton_clicked();

private:
    Ui::OpenRGBSystemInfoPageUi *ui;
    std::vector<i2c_smbus_interface *>& busses;
};

#endif // OPENRGBSYSTEMINFOPAGE_H
