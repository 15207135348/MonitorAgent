#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <setdialog.h>
#include <QMessageBox>
#include "QTime"
#include "QList"
#include "qmath.h"
#include "QPointF"
#include "QDebug"
#include <gather/UserGather.h>
#include <gather/HistoryGather.h>
#include <QScrollBar>
#include <QFileDialog>

#define MAX_SIZE 200

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initWindow();
    initTab1();
    initTab2();
    initTab3();
    initTab4();
    initClient();
    initGather();
}
void MainWindow::initWindow()
{
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    // 禁止最大化按钮
    setFixedSize(this->width(),this->height());                     // 禁止拖动窗口大小
    connect(ui->action_setting,&QAction::triggered,[this](){
        SetDialog *dialog = new SetDialog(this);
        dialog->setModal(true);
        dialog->show();
    });
}

void MainWindow::initTab1()
{
    ui->dateEdit_1->setDate(QDate::currentDate());
    ui->dateEdit_2->setDate(QDate::currentDate());
    ui->dateEdit_1->setCalendarPopup(true);
    ui->dateEdit_2->setCalendarPopup(true);

    m_barseries = new QBarSeries();
    m_chart = new QChart();
    m_chart->addSeries(m_barseries);
    m_chart->setTitle("login failure statistics");
    m_chart->setAnimationOptions(QChart::SeriesAnimations);
    m_axisX = new QBarCategoryAxis();
    m_axisY = new QValueAxis();
    m_axisY->setRange(0,5);
    m_axisY->setTitleText("failure count");
    m_chart->setAxisX(m_axisX, m_barseries);//设置坐标轴
    m_chart->setAxisY(m_axisY, m_barseries);//设置坐标轴
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    ui->verticalLayout_0->addWidget(m_chartView);
}
void MainWindow::initTab2()
{
    QString title[4][2] = {
        {"CPU","Usage"},
        {"Memory","Usage"},
        {"Disk","Speed"},
        {"Network","Speed"}
    };
    QVBoxLayout *layout[4] = {
        ui->verticalLayout_1,
        ui->verticalLayout_2,
        ui->verticalLayout_3,
        ui->verticalLayout_4
    };
    for(int i=0;i!=4;++i){
        m_charts[i] = new QChart();
        m_series[i] = new QLineSeries();
        m_charts[i]->addSeries(m_series[i]);
        m_chartViews[i] = new QChartView(m_charts[i]);
        m_chartViews[i]->setRubberBand(QChartView::RectangleRubberBand);

        for(int j=0;j<MAX_SIZE;++j){
           m_series[i]->append(j,0);
        }

        m_axisXs[i] = new QValueAxis();
        m_axisXs[i]->setRange(0,MAX_SIZE);
        m_axisXs[i]->setLabelFormat("%g");
        m_axisXs[i]->setTitleText("Time");

        m_axisYs[i] = new QValueAxis();
        m_axisYs[i]->setTitleText(title[i][1]);

        m_charts[i]->setAxisX(m_axisXs[i], m_series[i]);
        m_charts[i]->setAxisY(m_axisYs[i], m_series[i]);
        m_charts[i]->legend()->hide();
        m_charts[i]->setTitle(title[i][0]);

        layout[i]->addWidget(m_chartViews[i]);
    }
}
void MainWindow::initTab3()
{
    m_tree = new QTreeWidgetItem(ui->treeWidget,QStringList(QString("user@ip")));
    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this,SLOT(showItem(QTreeWidgetItem*, int)));
    QStringList hists = HistoryGather::getInstance()->ls();
    for(const QString& hist : hists)
    {
        QTreeWidgetItem *child = new QTreeWidgetItem(m_tree,QStringList(hist));
        m_tree->addChild(child); //添加子节点
    }
    ui->treeWidget->expandAll(); //结点全部展开
    ui->treeWidget->header()->setDefaultSectionSize(20); //结点全部展开
    QFont font = ui->treeWidget->header()->font();
    font.setBold(true);
    ui->treeWidget->header()->setFont(font);

    ui->tableWidget_1->setColumnCount(2); //设置列数
    QStringList header;
    header<<tr("time")<<tr("command");
    ui->tableWidget_1->setHorizontalHeaderLabels(header);
    font = ui->tableWidget_1->horizontalHeader()->font();
    font.setBold(true);
    ui->tableWidget_1->horizontalHeader()->setFont(font);
    ui->tableWidget_1->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_1->verticalHeader()->setDefaultSectionSize(20); //设置行高
    ui->tableWidget_1->verticalHeader()->setVisible(false); //设置垂直头不可见
    ui->tableWidget_1->setSelectionMode(QAbstractItemView::ExtendedSelection);  //可多选（Ctrl、Shift、  Ctrl+A都可以）
    ui->tableWidget_1->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
    ui->tableWidget_1->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->tableWidget_1->horizontalHeader()->resizeSection(0,250); //设置表头第一列的宽度为250
    //设置水平、垂直滚动条样式
    ui->tableWidget_1->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
    "QScrollBar::handle{bckground:lightgray; border:2px solid transparent; border-radius:5px;}"
    "QScrollBar::handle:hover{background:gray;}"
    "QScrollBar::sub-line{background:transparent;}"
    "QScrollBar::add-line{background:transparent;}");
    ui->tableWidget_1->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 10px;}"
    "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
    "QScrollBar::handle:hover{background:gray;}"
    "QScrollBar::sub-line{background:transparent;}"
    "QScrollBar::add-line{background:transparent;}");

}
void MainWindow::initTab4()
{
    ui->dateEdit_3->setDate(QDate::currentDate());
    ui->dateEdit_4->setDate(QDate::currentDate());
    ui->dateEdit_3->setCalendarPopup(true);
    ui->dateEdit_4->setCalendarPopup(true);

    ui->tableWidget_2->setColumnCount(3); //设置列数
    QStringList header;
    header<<tr("time")<<tr("user@ip")<<tr("command");
    ui->tableWidget_2->setHorizontalHeaderLabels(header);
    QFont font = ui->tableWidget_2->horizontalHeader()->font();
    font.setBold(true);
    ui->tableWidget_2->horizontalHeader()->setFont(font);
    ui->tableWidget_2->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_2->verticalHeader()->setDefaultSectionSize(20); //设置行高
    ui->tableWidget_2->verticalHeader()->setVisible(false); //设置垂直头不可见
    ui->tableWidget_2->setSelectionMode(QAbstractItemView::ExtendedSelection);  //可多选（Ctrl、Shift、  Ctrl+A都可以）
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->tableWidget_2->horizontalHeader()->resizeSection(0,250); //设置表头宽度
    ui->tableWidget_2->horizontalHeader()->resizeSection(1,250); //设置表头宽度
    //设置水平、垂直滚动条样式
    ui->tableWidget_2->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
    "QScrollBar::handle{bckground:lightgray; border:2px solid transparent; border-radius:5px;}"
    "QScrollBar::handle:hover{background:gray;}"
    "QScrollBar::sub-line{background:transparent;}"
    "QScrollBar::add-line{background:transparent;}");
    ui->tableWidget_2->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 10px;}"
    "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
    "QScrollBar::handle:hover{background:gray;}"
    "QScrollBar::sub-line{background:transparent;}"
    "QScrollBar::add-line{background:transparent;}");
}
void MainWindow::initClient()
{
    QMap<QString,QString> map = ConfigUtil::loadConf();
    QString host = map["host.address"];
    quint16 port = map["host.port"].toUShort();
    m_client = new Client(host,port);
    connect(m_client,&Client::connected,[this](){
        ui->statusBar->setStatusTip("Connected to cloud ");
        ui->actionConnect->setEnabled(false);
    });
    connect(m_client,&Client::disconnected,[this](){
        ui->statusBar->setStatusTip("Disconnected from cloud ");
        ui->actionConnect->setEnabled(true);
    });
    if(m_client->is_connected())
    {
        ui->statusBar->addPermanentWidget(new QLabel("Connected to cloud "));
        ui->actionConnect->setEnabled(false);
    }
    else
    {
        ui->statusBar->addPermanentWidget(new QLabel("Disconnected from cloud "));
        ui->actionConnect->setEnabled(true);
    }
    connect(ui->actionConnect,&QAction::triggered,[this](){
        m_client->connectHost();
        if(!m_client->is_connected())
        {
            QMessageBox::information(this, "error", "Can not connect to cloud!");
        }
        else
        {
            QMessageBox::information(this, "info", "Connect to cloud successfully!");
        }
    });
}
void MainWindow::initGather()
{
    QMap<QString,QString> map = ConfigUtil::loadConf();
    quint32 internal = map["gather.internal"].toUInt();
    m_gather = new GatherThread(internal);
    connect(m_gather,SIGNAL(gatherDone(const QMap<QString,QVariant>&)),
            this,SLOT(onGatherDone(const QMap<QString,QVariant>&)));
    m_gather->start();
}
void MainWindow::showItem(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem *parent=item->parent();
    if(NULL==parent)
    {
        QStringList hists = HistoryGather::getInstance()->ls();
        for(const QString& hist : hists)
        {
            QTreeWidgetItem *child = new QTreeWidgetItem(m_tree,QStringList(hist));
            m_tree->addChild(child); //添加子节点
        }
        ui->treeWidget->expandAll(); //结点全部展开
        return;
    }
    QString filename = item->text(column);
    QList<HistRecord> records = HistoryGather::getInstance()->history(filename);
    ui->tableWidget_1->setRowCount(0);
    ui->tableWidget_1->clearContents();
    for(const HistRecord& record : records)
    {
        int count = ui->tableWidget_1->rowCount(); //获取表单行数
        ui->tableWidget_1->insertRow(count); //插入新行
        QTableWidgetItem *item1 = new QTableWidgetItem();
        QTableWidgetItem *item2 = new QTableWidgetItem();
        item1->setText(QDateTime::fromTime_t(record.timestamp).toString("yyyy-MM-dd hh:mm:ss"));
        item2->setText(record.command);
        ui->tableWidget_1->setItem(count, 0, item1);
        ui->tableWidget_1->setItem(count, 1, item2);
    }
}

void MainWindow::update(int index, double value)
{
    QLineSeries* series =  m_series[index];
    QVector<QPointF> points = series->pointsVector();
    for(int i=0;i<MAX_SIZE-1;++i)
    {
        points[i].setY(points[i+1].y());
    }
    points[MAX_SIZE-1].setY(value);
    series->replace(points);
    bool flag = false;
    double minY = 10000000;
    double maxY = -10000000;
    for(int i=0;i<MAX_SIZE;++i)
    {
        double y = points[i].y();
        if(y < minY)
        {
            minY = y;
            flag = true;
        }
        if(y > maxY)
        {
            maxY = y;
            flag = true;
        }
    }
    if(flag && minY!=maxY)
    {
        m_axisYs[index]->setRange(minY,maxY);
    }
    series->replace(points);
}
MainWindow::~MainWindow()
{
    m_gather->stop();
    m_gather->quit();
    m_gather->wait();
    delete m_gather;
    delete m_client;
    delete ui;
}
void MainWindow::onGatherDone(const QMap<QString, QVariant> &map)
{
    m_client->send(map);
    double cpu = map["cpu"].toDouble();
    double ram = map["ram"].toDouble();
    double disk = map["disk"].toDouble();
    double net = map["net"].toDouble();
    update(0,cpu*100);
    update(1,ram*100);
    update(2,disk);
    update(3,net);

    if(m_save)
    {
        FileUtil::append(m_savepath,map);
    }
}

void MainWindow::on_pushButton_clicked()
{

    int max = 5;
    long t1 = ui->dateEdit_1->dateTime().toSecsSinceEpoch();
    long t2 = ui->dateEdit_2->dateTime().addDays(1).toSecsSinceEpoch();
    QList<QString> users = UserGather::logfail().keys();
    QStringList categories;
    QMap<QString,QBarSet*> setmap;
    for(const QString& user : users)
    {
        setmap[user] = new QBarSet(user);
    }
    while (t1 < t2) {

        long start = t1;
        long stop = start + 24*3600;
        QMap<QString,int> map = UserGather::logfail(start, stop);
        if(UserGather::merge(map) > 0)
        {
            categories.append(QDateTime::fromSecsSinceEpoch(start).toString("yyyy-MM-dd"));
            for(auto it = map.begin(); it!=map.end(); ++it)
            {
                if(max < it.value())
                {
                    max = it.value();
                }
                setmap[it.key()]->append(it.value());
            }
        }
        t1 = stop;
    }
    QList<QBarSet*> list = setmap.values();
    for(QBarSet* set : list)
    {
        int merge = 0;
        for(int i=0;i!=set->count();++i)
        {
            merge+=set->at(i);
        }
        if(merge == 0)
        {
            list.removeOne(set);
        }
    }
    m_barseries->clear();
    m_barseries->append(list);
    m_axisX->setCategories(categories);
    m_axisY->setRange(0,max);
}

void MainWindow::on_pushButton_2_clicked()
{
    long t1 = ui->dateEdit_3->dateTime().toSecsSinceEpoch();
    long t2 = ui->dateEdit_4->dateTime().addDays(1).toSecsSinceEpoch();

    QMap<long,QString> map = HistoryGather::getInstance()->history(t1,t2);
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_2->clearContents();
    for(auto it = map.begin(); it != map.end(); ++it)
    {
        int count = ui->tableWidget_2->rowCount(); //获取表单行数
        ui->tableWidget_2->insertRow(count); //插入新行
        QTableWidgetItem *item1 = new QTableWidgetItem();
        QTableWidgetItem *item2 = new QTableWidgetItem();
        QTableWidgetItem *item3 = new QTableWidgetItem();
        item1->setText(QDateTime::fromTime_t(it.key()).toString("yyyy-MM-dd hh:mm:ss"));
        int index = it.value().indexOf(":");
        item2->setText(it.value().mid(0,index));
        item3->setText(it.value().mid(index+1));
        ui->tableWidget_2->setItem(count, 0, item1);
        ui->tableWidget_2->setItem(count, 1, item2);
        ui->tableWidget_2->setItem(count, 2, item3);
    }
}

void MainWindow::on_actionSaveSystemLoad_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Save CSV"),
        "",
        tr("*.csv;; *.txt")); //选择路径
    if(filename.isEmpty())
    {
        m_save = false;
    }
    else
    {
        m_save = true;
        m_savepath = filename;
    }
}
