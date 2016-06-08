#pragma once

#include <QFrame>
#include <QtNetwork/QtNetwork>

namespace Ui {
class BannerFrame;
}

class BannerFrame : public QFrame
{
    Q_OBJECT
    
public:
    explicit BannerFrame(QWidget *parent = 0);
    ~BannerFrame();

private slots:
    void onNetwManagerFinished(QNetworkReply *reply);
    void onPledgieClicked();
    void onCrossTalkClicked();

private:
    Ui::BannerFrame *ui;
    QNetworkAccessManager *m_netwManager;
};
