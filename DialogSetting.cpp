#include "DialogSetting.h"
#include "ui_DialogSetting.h"

#include <QHeaderView>
#include <QSettings>
#include <QTimer>

DialogSetting::DialogSetting(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSetting)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    m_cfg = DialogConfig::instance() ;

    m_pSet = new QSettings(QCoreApplication::applicationDirPath()+"/PXTestConfig.ini",QSettings::IniFormat) ;
    m_pSet->beginGroup("Features") ;

    m_pModel1 = new QStandardItemModel(this);
    m_pModel1->setHorizontalHeaderLabels(QString("测试内容,使能,参数,状态").split(','));
    ui->tableView1->setModel(m_pModel1);
    QHeaderView *pHeader = ui->tableView1->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    pHeader->setSectionResizeMode(0,QHeaderView::Fixed) ;
    pHeader->setSectionResizeMode(1,QHeaderView::Fixed) ;
    pHeader->setSectionResizeMode(2,QHeaderView::Fixed) ;
    pHeader->resizeSection(0,200);
    pHeader->resizeSection(1,30);
    pHeader->resizeSection(2,50);
    ui->tableView1->setVerticalHeader(new ZeroBasedHeader(ui->tableView1));
    ui->tableView1->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    addTestItem("静态电流总控开关1/2打开") ;
    addTestItem("各通道静态电流开关打开") ;

    addTestItem("上电后等待时间") ; // 2----------

    addTestItem("读取各通道静态电流") ;  // 3 x
    addTestItem("各通道切换电流表开关打开") ;

    addTestItem("静态电流总控开关1/2关闭") ;
    addTestItem("各通道静态电流开关关闭") ;

    addTestItem("吸烟气泵开关打开") ;

    addTestItem("开气泵后等待时间") ; // 8-----------

    addTestItem("读取各通道启动气压",true) ;  //9 x

    addTestItem("读取各通道吸烟电流") ; // 10 x

    addTestItem("读取各通道输出电压",true) ; //11 x
    addTestItem("吸烟时间",true) ;  //12 -----------

    addTestItem("吸烟气泵开关关闭") ;

    addTestItem("吸烟结束等待时间",true) ; //14   ----------
    addTestItem("各通道充电开关打开",true) ; //15
    addTestItem("开始充电等待时间",true) ;  //16 ------------
    addTestItem("读取各通道充电电流",true) ;  //17 x
    addTestItem("充电时间",true) ;  //18 ---------------
    addTestItem("读取各通道充满电压",true) ; // 19 x
    addTestItem("各通道充电开关关闭",true) ; // S20

    addTestItem("各通道切换电流表开关关闭") ;

    m_pModel1->item( 9,1)->setCheckState(m_pSet->value("ChanAirPress"  ).toBool()?Qt::Checked:Qt::Unchecked) ;      //9
    m_pModel1->item(11,1)->setCheckState(m_pSet->value("ChanOutVoltage").toBool()?Qt::Checked:Qt::Unchecked) ;      //11
    m_pModel1->item(12,1)->setCheckState(m_pSet->value("SmokingEnable" ).toBool()?Qt::Checked:Qt::Unchecked) ;      //12
    m_pModel1->item(14,1)->setCheckState(m_pSet->value("SmokingWaiting").toBool()?Qt::Checked:Qt::Unchecked) ;      //14
    m_pModel1->item(15,1)->setCheckState(m_pSet->value("ChanChargeOn"  ).toBool()?Qt::Checked:Qt::Unchecked) ;      //15
    m_pModel1->item(16,1)->setCheckState(m_pSet->value("ChargeWaiting" ).toBool()?Qt::Checked:Qt::Unchecked) ;      //16
    m_pModel1->item(17,1)->setCheckState(m_pSet->value("ChargeCurrent" ).toBool()?Qt::Checked:Qt::Unchecked) ;      //17
    m_pModel1->item(18,1)->setCheckState(m_pSet->value("ChargeOccuring").toBool()?Qt::Checked:Qt::Unchecked) ;      //18
    m_pModel1->item(19,1)->setCheckState(m_pSet->value("ChargeVoltage" ).toBool()?Qt::Checked:Qt::Unchecked) ;      //19
    m_pModel1->item(20,1)->setCheckState(m_pSet->value("ChanChargeOff" ).toBool()?Qt::Checked:Qt::Unchecked) ;      //20

    QString strTimes= m_pSet->value("WaitTimes","6000,500,500,200,1000,2000,1000").toString();
    QStringList Times = strTimes.split(',');
    m_pModel1->item(2, 2)->setText(Times[0]) ;
    m_pModel1->item(8, 2)->setText(Times[1]) ;
    m_pModel1->item(12,2)->setText(Times[2]) ;
    m_pModel1->item(14,2)->setText(Times[3]) ;
    m_pModel1->item(16,2)->setText(Times[4]) ;
    m_pModel1->item(18,2)->setText(Times[5]) ;

    m_pModel1->item(2, 2)->setEditable(true) ;
    m_pModel1->item(8, 2)->setEditable(true) ;
    m_pModel1->item(12,2)->setEditable(true) ;
    m_pModel1->item(14,2)->setEditable(true) ;
    m_pModel1->item(16,2)->setEditable(true) ;
    m_pModel1->item(18,2)->setEditable(true) ;

    connect(m_pModel1,&QStandardItemModel::itemChanged,this,[=](QStandardItem *item){
        m_pSet->setValue("ChanAirPress"  ,m_pModel1->item( 9,1)->checkState() == Qt::Checked);     //9
        m_pSet->setValue("ChanOutVoltage",m_pModel1->item(11,1)->checkState() == Qt::Checked);     //11
        m_pSet->setValue("SmokingEnable" ,m_pModel1->item(12,1)->checkState() == Qt::Checked);     //12
        m_pSet->setValue("SmokingWaiting",m_pModel1->item(13,1)->checkState() == Qt::Checked);     //14
        m_pSet->setValue("ChanChargeOn"  ,m_pModel1->item(15,1)->checkState() == Qt::Checked);     //15
        m_pSet->setValue("ChargeWaiting" ,m_pModel1->item(16,1)->checkState() == Qt::Checked);     //16
        m_pSet->setValue("ChargeCurrent" ,m_pModel1->item(17,1)->checkState() == Qt::Checked);     //17
        m_pSet->setValue("ChargeOccuring",m_pModel1->item(18,1)->checkState() == Qt::Checked);     //18
        m_pSet->setValue("ChargeVoltage" ,m_pModel1->item(19,1)->checkState() == Qt::Checked);     //19
        m_pSet->setValue("ChanChargeOff" ,m_pModel1->item(20,1)->checkState() == Qt::Checked);     //20

        QString strTime;
        strTime += m_pModel1->item(2, 2)->text() + ",";
        strTime += m_pModel1->item(8, 2)->text() + ",";
        strTime += m_pModel1->item(12,2)->text() + ",";
        strTime += m_pModel1->item(14,2)->text() + ",";
        strTime += m_pModel1->item(16,2)->text() + ",";
        strTime += m_pModel1->item(18,2)->text() ;
        m_pSet->setValue("WaitTimes",strTime) ;
        if(item->column() == 1)
        {
            emit onTestOption(item->row(),item->checkState() == Qt::Checked);
        }
    });

    QTimer::singleShot(100,this,[=]{
        emit onTestOption(9, m_pModel1->item(9, 1)->checkState() == Qt::Checked);
        emit onTestOption(11,m_pModel1->item(11,1)->checkState() == Qt::Checked);
        emit onTestOption(19,m_pModel1->item(19,1)->checkState() == Qt::Checked);
    });

    connect(&m_TMTest,&QTimer::timeout,this,[=]{
        m_TMTest.stop() ;
        int index = m_nTestIndex;

        if(index >= 0) m_pModel1->item(index,3)->setText("Test OK!") ;

        if(index == 21)
        {
            m_nTestIndex = -1 ;
            emit onTestToEnd();
            return ;
        }

        index++ ;
        m_nTestIndex = index ;
        QString strTime = m_pModel1->item(index,2)->text() ;

        bool toTest = true ;
        if(m_pModel1->item(index,1)->isCheckable())
        {
            toTest = (m_pModel1->item(index,1)->checkState() == Qt::Checked) ;
            strTime = "0" ;
        }

        if(toTest)
        {
            m_pModel1->item(index,3)->setText("Testing ......") ;
            m_cfg->doTesting(index) ;
        }
        else
        {
            m_pModel1->item(index,3)->setText("Pass by ......") ;
        }

        int wait = 100 ;
        switch(index)
        {
        case 3:
        case 8:
        case 9:
        case 10:
        case 11:
        case 17:
        case 19:
            wait = 1000;
            break;
        }

        if(!toTest) wait = 100 ;

        m_TMTest.start(strTime.toInt() + wait) ;
    });
}

DialogSetting::~DialogSetting()
{
    delete ui;
}

void DialogSetting::toTheEnd()
{
    emit onTestToEnd();
}

void DialogSetting::startTest()
{
    if(m_nTestIndex != -1)
        return ;
    m_nTestIndex = -1 ;
    for(int i=0; i<m_pModel1->rowCount(); i++)
        m_pModel1->item(i,3)->setText("") ;
    m_TMTest.start(10) ;
}

void DialogSetting::addTestItem(const QString&text,bool option)
{
   QStandardItem *item0 = new QStandardItem(text);
   QStandardItem *item1 = new QStandardItem("");
   QStandardItem *item2 = new QStandardItem("");
   QStandardItem *item3 = new QStandardItem("");

   item0->setEditable(false);
   item1->setEditable(false);
   item2->setEditable(false);
   item3->setEditable(false);
   item1->setCheckable(option);

   m_pModel1->appendRow({item0, item1, item2, item3});
   //ui->tableView1->setRowHeight(m_pModel1->rowCount(),18);
}

