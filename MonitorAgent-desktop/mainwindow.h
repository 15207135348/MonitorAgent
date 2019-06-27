#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>

#include <QValueAxis>
#include <QLineSeries>
#include <QtCharts/QChart>
#include <QChartView>
#include <QTreeWidgetItem>

#include <client.h>
#include <GatherThread.h>
#include <util/ConfigUtil.h>

#include <util/Fileutil.h>

using namespace QtCharts;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void initWindow();
    void initTab1();
    void initTab2();
    void initTab3();
    void initTab4();
    void initClient();
    void initGather();
    void update(int index, double value);

private:
    Ui::MainWindow *ui;

    //tab1
    QBarSeries *m_barseries;
    QBarCategoryAxis *m_axisX;
    QValueAxis *m_axisY;
    QChart *m_chart;
    QChartView *m_chartView;


    //tab2
    QChart *m_charts[4];
    QLineSeries *m_series[4];
    QValueAxis *m_axisXs[4];
    QValueAxis *m_axisYs[4];
    QChartView *m_chartViews[4];

    //tab3
    QTreeWidgetItem *m_tree;

    GatherThread *m_gather;
    Client *m_client;

    bool m_save = false;
    QString m_savepath;

public slots:
    void onGatherDone(const QMap<QString,QVariant>& map);
    void showItem(QTreeWidgetItem *item, int column);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_actionSaveSystemLoad_triggered();
};

#endif // MAINWINDOW_H
