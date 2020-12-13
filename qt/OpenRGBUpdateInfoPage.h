#ifndef OPENRGBUPDATEINFOPAGE_H
#define OPENRGBUPDATEINFOPAGE_H

#include <QFrame>
#include "ui_OpenRGBUpdateInfoPage.h"

namespace Ui {
class OpenRGBUpdateInfoPage;
}

class Ui::OpenRGBUpdateInfoPage : public QFrame
{
    Q_OBJECT

public:
    explicit OpenRGBUpdateInfoPage(QWidget *parent = nullptr);
    ~OpenRGBUpdateInfoPage();



private slots:
    void on_CheckButton_clicked();
    void on_UpdateButton_clicked();

private:
    Ui::OpenRGBUpdateInfoPageUi *ui;
    QByteArray GetPageSourceOrFile(QString Link);

    #ifdef _WIN32
    bool WinExtensions = true;
    #else
    bool WinExtensions = false;
    #endif

    /*-------------------------------------------------*\
    | Defaults to primary master if it hasn't been set  |
    \*-------------------------------------------------*/
    QString CheckBranch = "master";
    QString Fork        = "CalcProgrammer1";

    QString CommitLink;
    std::vector<QStringList> Parsed;

    QString PreferedPlatform;
    QString JobID;

    std::vector<QStringList> ParseCommitList(QString ContentToParse);

    bool OwnedByUpdateButton = false;
    bool ContentMissing = false;
    void UpdateProgressBar(bool Show, QString Text = NULL, int Value = NULL);
};

#endif // OPENRGBUPDATEINFOPAGE_H
