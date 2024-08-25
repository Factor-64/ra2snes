#ifndef RA2SNES_H
#define RA2SNES_H

#include <QMainWindow>
#include "usb2snes.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ra2snes;
}
QT_END_NAMESPACE

class ra2snes : public QMainWindow
{
    Q_OBJECT

public:
    ra2snes(QWidget *parent = nullptr);
    ~ra2snes();

private slots:

    void on_signin_button_clicked();
    void resizeWindow(int width, int height);
    void refreshStatus();
    void getFileMD5(const QString& path);

private:
    Ui::ra2snes *ui;
};
#endif // RA2SNES_H
