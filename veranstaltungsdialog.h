#ifndef VERANSTALTUNGSDIALOG_H
#define VERANSTALTUNGSDIALOG_H

#include <QDialog>

namespace Ui {
class veranstaltungsdialog;
}

class veranstaltungsdialog : public QDialog
{
    Q_OBJECT

public:
    explicit veranstaltungsdialog(QWidget *parent = nullptr);
    ~veranstaltungsdialog();
    QString getPortalurl();
    QString getVeranstaltung();
    QString getapiKey();
    void setPortalurl(const QString &portalurl);
    void setVeranstaltung(const QString &veranstaltung);
    void setapikey(const QString &apikey);

private slots:


private:
    Ui::veranstaltungsdialog *ui;
};

#endif // VERANSTALTUNGSDIALOG_H
