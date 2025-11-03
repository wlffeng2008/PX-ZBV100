#include "DialogTestLog.h"
#include "ui_DialogTestLog.h"
#include <QDir>
#include <QFile>

DialogTestLog::DialogTestLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTestLog)
{
    ui->setupUi(this);

    m_pModel1 = new QStandardItemModel(ui->tableView1);
    ui->tableView1->setModel(m_pModel1);
    m_pModel1->setHorizontalHeaderLabels({"测试日期"});

    connect(ui->pushButtonList,&QPushButton::clicked,this,[=]{
        QStringList nameFilters;
        nameFilters << "*.txt";

        m_pModel1->setRowCount(0);
        QString strPath(QCoreApplication::applicationDirPath() + "/TestLog/");
        QDir dir(strPath) ;
        QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::NoDotAndDotDot);

        if(files.count())
        {
            for(const QString&file:std::as_const(files))
            {
                m_strLogFile = strPath + file ;
                m_pModel1->appendRow(new QStandardItem(file));
            }
            ui->tableView1->selectRow(m_pModel1->rowCount()-1) ;
            ui->pushButtonCount->click() ;
        }

    }) ;

    ui->tableView1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_pModel2 = new QStandardItemModel(ui->tableView2);
    ui->tableView2->setModel(m_pModel2);
    QHeaderView *pHeader = ui->tableView2->horizontalHeader() ;
    m_pModel2->setHorizontalHeaderLabels(QString("测试时间,通道号,静态电流(uA),吸烟电流(mA),充电电流(mA),充满电压(mV),输出电压(mV),启动气压(Pa),测试结果").split(','));

    pHeader->setSectionResizeMode(QHeaderView::Stretch);
    pHeader->setSectionResizeMode(0,QHeaderView::Fixed);
    pHeader->resizeSection(0,150) ;

    pHeader->setSectionResizeMode(1,QHeaderView::Fixed);
    pHeader->resizeSection(1,40) ;

    connect(ui->tableView1,&QTableView::clicked,this,[=](const QModelIndex &index){
        QString strFile = QCoreApplication::applicationDirPath() + "/TestLog/" +m_pModel1->itemFromIndex(index)->text().trimmed();
        m_strLogFile = strFile ;
        ui->pushButtonCount->click() ;
    });

    connect(ui->pushButtonCount,&QPushButton::clicked,this,[=]{
        QFile file(m_strLogFile);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "文件打开失败：" << file.errorString()  << m_strLogFile;
            return false;
        }

        m_total = 0 ;
        m_good = 0 ;
        m_pModel2->setRowCount(0) ;

        QTextStream in(&file);
        in.readLine() ;
        while (!in.atEnd()) addItem(in.readLine());
        file.close();
        QString strCount=QString::asprintf("测试总数：%d     通过数量：%d     通过率：%.2f%%",m_total,m_good,m_good*100.0/m_total);
        ui->labelCount->setText(strCount) ;
    });

    ui->pushButtonList->click() ;
}

DialogTestLog::~DialogTestLog()
{
    delete ui;
}

void DialogTestLog::addItem(QString strLine)
{
    QStringList strItems=strLine.trimmed().split(',');
    if(strItems.count() != 9)
        return ;
    QList<QStandardItem*> list ;
    for(int i=0;i<strItems.size(); i++){
        QStandardItem *item=new QStandardItem(strItems[i]) ;
        item->setEditable(false) ;
        list.append(item) ;
    }
    m_pModel2->appendRow(list) ;

    m_total ++ ;
    if(strItems[8].trimmed().toUpper() == "PASS")
        m_good++;
}
