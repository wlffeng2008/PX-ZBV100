#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QTableView>
#include <QHeaderView>
#include <QTimer>
#include <QWindow>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QApplication>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    for(int i=0;i<10; i++) m_colEnable[i] = true;
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    //qApp->setStyle(new CustomStyle);

    {
        QString strBuild ;
#ifdef _MSC_VER

        QString strName = "MSVC2017";

#if _MSC_VER >= 1930
        strName = "MSVC2022";
#elif _MSC_VER >= 1920
        strName = "MSVC2019";
#else
        strName = "MSVC2017";
#endif

        strBuild = QString("使用 %1 或更高版本编译, 版本号: %2").arg(strName).arg(_MSC_VER)  ;

#else
        strBuild = "非 MSVC 编译(如 MinGW, GCC 等)";
#endif

        QString strTitle = QString("鹏翔测试装备上位机(PX-ZBV100) (V1.00) -- [Build: %1] [By Qt%2] -- [%3]").arg(__TIMESTAMP__,QT_VERSION_STR,strBuild) ;
        setWindowTitle(strTitle);
        qDebug() << strTitle;
    }

    m_pDlgCfg = new DialogConfig(this) ;
    m_pDlgSet = new DialogSetting(this);
    m_pDlgLog = new DialogTestLog(this);

    connect(ui->checkBoxOntop,&QCheckBox::clicked,this,[=](bool checked){
        QWindow *pWin = windowHandle() ;
        if(checked)
            pWin->setFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        else
            pWin->setFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    });


    connect(ui->pushButtonSetting,&QPushButton::clicked,this,[=]{
        m_pDlgSet->show();
    });

    connect(ui->pushButtonConfig,&QPushButton::clicked,this,[=]{
        m_pDlgCfg->show();
    });

    connect(ui->pushButtonLog,&QPushButton::clicked,this,[=]{
        m_pDlgLog->show();
    });

    connect(m_pDlgCfg,&DialogConfig::onChanState,this,[=](int row,Qt::CheckState state){
        m_pModel1->item(row,0)->setCheckState(state);
        for(int i=1; i<7; i++)
        {
            m_pModel1->item(row,i)->setEnabled((state == Qt::Checked) && m_colEnable[i]);
        }
    });

    connect(m_pDlgCfg,&DialogConfig::onAutoTestAcq,this,[=](){
        ui->pushButtonRun->click() ;
    });

    m_pModel1 = new QStandardItemModel(ui->tableView1);
    ui->tableView1->setModel(m_pModel1);
    QHeaderView *pHeader = ui->tableView1->horizontalHeader() ;
    m_pModel1->setHorizontalHeaderLabels(QString("通道号(使能),静态电流(uA),吸烟电流(mA),充电电流(mA),充满电压(mV),输出电压(mV),启动气压(Pa),测试结果").split(','));
    for(int i=0; i<m_pDlgCfg->getChanCount(); i++)
    {
        QStandardItem *item0 = new QStandardItem(QString::number(i+1)) ;
        QStandardItem *item1 = new QStandardItem("--") ;
        QStandardItem *item2 = new QStandardItem("--") ;
        QStandardItem *item3 = new QStandardItem("--") ;
        QStandardItem *item4 = new QStandardItem("--") ;
        QStandardItem *item5 = new QStandardItem("--") ;
        QStandardItem *item6 = new QStandardItem("--") ;
        QStandardItem *item7 = new QStandardItem("--") ;
        QList<QStandardItem *>items{item0,item1,item2,item3,item4,item5,item6,item7} ;
        for(QStandardItem *item:items)
            item->setTextAlignment(Qt::AlignCenter) ;
        item0->setBackground(QBrush(QColorConstants::Svg::skyblue)) ;
        item0->setCheckable(true) ;
        item0->setCheckState(Qt::Checked) ;
        item0->setTextAlignment(Qt::AlignVCenter|Qt::AlignLeft) ;
        m_pModel1->appendRow(items);
    }
    pHeader->setSectionResizeMode(QHeaderView::Stretch);
    pHeader->setSectionResizeMode(0,QHeaderView::Fixed);
    pHeader->resizeSection(0,100) ;
    //ui->tableView1->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch) ;

    for(int i=0; i<pHeader->count(); i++)
    {
        ColorDelegate *colDele = new ColorDelegate(ui->tableView1);
        colDele->setTableView(ui->tableView1) ;
        ui->tableView1->setItemDelegateForColumn(i,colDele) ;
    }

    ui->tableView1->update();
    setResult(0) ;

    m_pModel2 = new QStandardItemModel(ui->tableView2);
    ui->tableView2->setModel(m_pModel2);
    m_pModel2->setHorizontalHeaderLabels({"",""});
    m_pModel2->appendRow({new QStandardItem("测试总数"),new QStandardItem("0")});
    m_pModel2->appendRow({new QStandardItem("通过总数"),new QStandardItem("0")});
    m_pModel2->appendRow({new QStandardItem("通过比例"),new QStandardItem("0.00%")});
    ui->tableView2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView2->horizontalHeader()->setVisible(false) ;
    {
        ColorDelegate *calcDele = new ColorDelegate(ui->tableView2);
        calcDele->setTableView(ui->tableView2) ;
        ui->tableView2->setItemDelegateForColumn(0,calcDele) ;
    }
    m_pModel2->item(0,0)->setBackground(QBrush(Qt::lightGray)) ;
    m_pModel2->item(1,0)->setBackground(QBrush(Qt::lightGray)) ;
    m_pModel2->item(2,0)->setBackground(QBrush(Qt::lightGray)) ;
    m_pModel2->item(0,0)->setTextAlignment(Qt::AlignVCenter|Qt::AlignRight) ;
    m_pModel2->item(1,0)->setTextAlignment(Qt::AlignVCenter|Qt::AlignRight) ;
    m_pModel2->item(2,0)->setTextAlignment(Qt::AlignVCenter|Qt::AlignRight) ;

    ui->tableView2->setStyleSheet("QTableView { border-right: none; border-bottom: none; font: bold 12px 微软雅黑;}") ;
    ui->labelStatus->setStyleSheet("QLabel{border:2px solid gray;}") ;

    connect(ui->pushButtonRun,&QPushButton::clicked,this,[=]{
        if(!m_pDlgCfg->isConnected())
        {
            QMessageBox::critical(this,"错误","串口尚未全部连接，请检查后重试！");
            return;
        }

        setResult(0);
        ui->labelStatus->setText("正在测试") ;
        ui->labelStatus->setStyleSheet("QLabel{border:2px solid skyblue;border-radius:10px;background-color:gray;color:white}") ;

        m_pDlgCfg->reset() ;
        m_pDlgSet->startTest() ;
    });

    connect(m_pDlgSet,&DialogSetting::onTestOption,this,[=](int item,bool enable){
        int col = -1 ;
        switch(item)
        {
        case 19: col = 4; break;
        case 11: col = 5; break;
        case  9: col = 6; break;
        }

        if(col>0)
        {
            m_colEnable[col] = enable ;
            for(int i=0; i<m_pModel1->rowCount(); i++)
                m_pModel1->item(i,col)->setEnabled(enable && m_pModel1->item(i,0)->checkState() == Qt::Checked) ;
        }
    });

    connect(m_pDlgCfg,&DialogConfig::onReadBack,this,[=](int chan,int col,float value,bool ok){

        if(m_pModel1->item(chan,0)->checkState() != Qt::Checked)
            return ;

        m_pModel1->item(chan,col)->setText(QString::asprintf("%.3f",value));
        setItemResult(chan, col, ok ? 1 : 2) ;
    }) ;

    connect(m_pDlgSet,&DialogSetting::onTestToEnd,this,[=]{
        bool bAllOk = true ;
        int total = 0 ;
        int good =0 ;
        for(int i=0; i<m_pModel1->rowCount(); i++)
        {
            if(m_pModel1->item(i,0)->checkState() == Qt::Checked)
            {
                m_total ++ ;
                total++ ;
                bool ok = isRowPassed(i);
                if(ok)
                {
                    m_good ++ ;
                    good++ ;
                    m_pDlgCfg->setLED_R(i,false) ;
                    m_pDlgCfg->setLED_G(i,true);
                }
                else
                {
                    bAllOk = false ;
                    m_pDlgCfg->setLED_R(i,true) ;
                    m_pDlgCfg->setLED_G(i,false);
                }
            }
        }

        if(bAllOk)
        {
            ui->labelStatus->setText("PASS") ;
            ui->labelStatus->setStyleSheet("QLabel{border:2px solid gray;border-radius:10px; background-color:green; color:white;}") ;
        }
        else
        {
            ui->labelStatus->setText("NG") ;
            ui->labelStatus->setStyleSheet("QLabel{border:2px solid gray;border-radius:10px;background-color:red;color:white;}") ;
        }

        QString strTotal = QString::asprintf("%d",m_total);
        QString strPass  = QString::asprintf("%d",m_good);
        QString strRate  = QString::asprintf("%.2f%%",m_good*100.0/m_total);
        m_pModel2->item(0,1)->setText(strTotal);
        m_pModel2->item(1,1)->setText(strPass);
        m_pModel2->item(2,1)->setText(strRate);

        addLog() ;

        QMessageBox::information(this,"提示",QString("测试完毕！") + (good== total ? "(全部通过)" : "(部分通过)") );

    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addLog()
{
    QString strPath(QCoreApplication::applicationDirPath() + "/TestLog");
    QDir dir(strPath);
    if(!dir.exists()) dir.mkpath(strPath);
    QString strFile = strPath + QString("/TestLog-%1.txt").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd")) ;

    QFile file(strFile);
    bool bExist = file.exists();
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        QMessageBox::critical(this,"错误","文件创建失败，无法写入测试记录！");
        return ;
    }

    QTextStream out(&file);
    if(!bExist)
    {
        QString strLine= "测试时间,通道号,静态电流(uA),吸烟电流(mA),充电电流(mA),充满电压(mV),输出电压(mV),启动气压(Pa),结果\n" ;
        out << strLine ;
    }

    QString strTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") ;
    int nCount = m_pModel1->rowCount();
    for(int i=0; i<nCount; i++)
    {
        if(m_pModel1->item(i,0)->checkState() == Qt::Unchecked)
            continue ;
        QString strLine = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9\n").arg(strTime)
            .arg(m_pModel1->item(i,0)->text().trimmed())
            .arg(m_pModel1->item(i,1)->text().trimmed())
            .arg(m_pModel1->item(i,2)->text().trimmed())
            .arg(m_pModel1->item(i,3)->text().trimmed())
            .arg(m_pModel1->item(i,4)->text().trimmed())
            .arg(m_pModel1->item(i,5)->text().trimmed())
            .arg(m_pModel1->item(i,6)->text().trimmed())
            .arg(m_pModel1->item(i,7)->data(Qt::UserRole+1).value<int>() == 1 ? "PASS" : "NG");
        out << strLine ;
    }
}

bool MainWindow::isRowPassed(int row)
{
    for(int col=1; col<7; col++)
    {
        int result = m_pModel1->item(row,col)->data(Qt::UserRole+1).value<int>() ;
        if(result != 1 && m_pModel1->item(row,col)->isEnabled()) return false ;
    }
    return true;
}

void MainWindow::setItemResult(int row, int col, int result)
{
    m_pModel1->item(row,col)->setData(result,Qt::UserRole+1) ;
    if(result != 0)
        m_pModel1->item(row,7)->setData(isRowPassed(row)?1:2,Qt::UserRole+1) ;
    else
        m_pModel1->item(row,col)->setText("--") ;
}

void MainWindow::setRowResult(int row, int result)
{
    for(int col=1; col<7; col++)
    {
        setItemResult(row,col,result) ;
    }
}

void MainWindow::setResult(int result)
{
    for(int row=0; row<m_pModel1->rowCount(); row++)
    {
        for(int col=1; col<8; col++)
        {
            setItemResult(row,col,result) ;
        }
    }
}
