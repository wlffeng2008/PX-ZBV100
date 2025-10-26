#include "DialogConfig.h"
#include "ui_DialogConfig.h"

#include <QTimer>

static DialogConfig *pConfig = nullptr ;
DialogConfig *DialogConfig::instance()
{
    return pConfig ;
}

DialogConfig::DialogConfig(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogConfig)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    pConfig = this ;

    m_pCOM1 = new GenComport(this);
    m_pCOM2 = new GenComport(this);

    m_pModelTGroup = new QStandardItemModel(this);
    m_pModelTGroup->setHorizontalHeaderLabels(QString("测试项目,端口号,下限,上限,读取值,单位,读取").split(','));
    ui->tableViewTGroup->setModel(m_pModelTGroup);
    QHeaderView *pHeader = ui->tableViewTGroup->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    pHeader->setSectionResizeMode(0,QHeaderView::Fixed) ;
    pHeader->resizeSection(0,70) ;
    ui->tableViewTGroup->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_pModelTGroup->appendRow({new QStandardItem("静态电流"),new QStandardItem("1"), new QStandardItem("5.00"),new QStandardItem("7.00"),new QStandardItem("--"),new QStandardItem("uA"),new QStandardItem("单击读取")}) ;
    m_pModelTGroup->appendRow({new QStandardItem("吸烟电流"),new QStandardItem("7"), new QStandardItem("1000"),new QStandardItem("3000"),new QStandardItem("--"),new QStandardItem("mA"),new QStandardItem("单击读取")}) ;
    m_pModelTGroup->appendRow({new QStandardItem("充电电流"),new QStandardItem("7"), new QStandardItem("100" ),new QStandardItem("600" ),new QStandardItem("--"),new QStandardItem("mA"),new QStandardItem("单击读取")}) ;
    m_pModelTGroup->appendRow({new QStandardItem("充满电压"),new QStandardItem("20"),new QStandardItem("4150"),new QStandardItem("4250"),new QStandardItem("--"),new QStandardItem("mV"),new QStandardItem("单击读取")}) ;
    m_pModelTGroup->appendRow({new QStandardItem("输出电压"),new QStandardItem("30"),new QStandardItem("3500"),new QStandardItem("3600"),new QStandardItem("--"),new QStandardItem("mV"),new QStandardItem("单击读取")}) ;
    m_pModelTGroup->appendRow({new QStandardItem("启动气压"),new QStandardItem("40"),new QStandardItem("-200"),new QStandardItem("-100"),new QStandardItem("--"),new QStandardItem("Pa"),new QStandardItem("单击读取")}) ;
    for(int i=0; i<6; i++)
    {
        m_pModelTGroup->item(i,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter) ;
        m_pModelTGroup->item(i,0)->setSelectable(false);
        for(int j=0; j<=6; j++)
        {
            QStandardItem *item = m_pModelTGroup->item(i,j) ;
            if(j == 0 || j == 4 || j == 5|| j == 6) item->setEditable(false);
            if(j > 0) item->setTextAlignment(Qt::AlignCenter) ;
        }
    }

    connect(ui->tableViewTGroup,&QTableView::clicked,this,[=](const QModelIndex &index){
        if(index.column() == 6)
        {
            m_pModelTGroup->item(index.row(),4)->setText("正在读取");
            getMeter(m_pModelTGroup->item(index.row(),1)->text().trimmed().toInt()) ;
        }
    });

    m_pModelCOM = new QStandardItemModel(this);
    m_pModelCOM->setHorizontalHeaderLabels(QString("参数,单板串口,仪表485串口").split(','));
    ui->tableViewCOM->setModel(m_pModelCOM);
    pHeader = ui->tableViewCOM->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    ui->tableViewCOM->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pHeader->setSectionResizeMode(0,QHeaderView::Fixed) ;
    pHeader->resizeSection(0,70) ;

    m_pModelCOM->appendRow({new QStandardItem("串口"),new QStandardItem("COM4"),new QStandardItem("COM5")}) ;
    m_pModelCOM->appendRow({new QStandardItem("波特率"),new QStandardItem("115200"),new QStandardItem("9600")}) ;
    m_pModelCOM->appendRow({new QStandardItem("数据位"),new QStandardItem("8位"),new QStandardItem("8位")}) ;
    m_pModelCOM->appendRow({new QStandardItem("校验位"),new QStandardItem("无校验"),new QStandardItem("无校验")}) ;
    m_pModelCOM->appendRow({new QStandardItem("停止位"),new QStandardItem("1位"),new QStandardItem("1位")}) ;
    m_pModelCOM->appendRow({new QStandardItem("连接状态"),new QStandardItem("串口未打开"),new QStandardItem("串口未打开")}) ;
    m_pModelCOM->appendRow({new QStandardItem("控制"),new QStandardItem("打开串口"),new QStandardItem("打开串口")}) ;
    for(int i=0; i<7; i++)
    {
        m_pModelCOM->item(i,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter) ;
        m_pModelCOM->item(i,0)->setSelectable(false) ;
        for(int j=0; j<3; j++)
        {
            QStandardItem *item = m_pModelCOM->item(i,j) ;
            item->setEditable(false);
        }
    }

    m_pModelCOM->item(0,1)->setEditable(true);
    m_pModelCOM->item(0,2)->setEditable(true);
    m_pModelCOM->item(1,1)->setEditable(true);
    m_pModelCOM->item(1,2)->setEditable(true);

    m_pModelCOM->item(5,1)->setCheckable(true);
    m_pModelCOM->item(5,2)->setCheckable(true);
    m_pModelCOM->item(5,1)->setEnabled(false);
    m_pModelCOM->item(5,2)->setEnabled(false);

    m_pModelCOM->item(6,1)->setCheckable(true);
    m_pModelCOM->item(6,2)->setCheckable(true);


    m_pModelDelay = new QStandardItemModel(this);
    m_pModelDelay->setHorizontalHeaderLabels(QString("控制项,时间(ms)").split(','));
    ui->tableViewDelay->setModel(m_pModelDelay);
    pHeader = ui->tableViewDelay->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    pHeader->setSectionResizeMode(1,QHeaderView::Fixed) ;
    pHeader->resizeSection(1,70) ;
    ui->tableViewDelay->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_pModelDelay->appendRow({new QStandardItem("上电后等待时间"),new QStandardItem("6000")});
    m_pModelDelay->appendRow({new QStandardItem("开气泵后等待时间"),new QStandardItem("500")});
    m_pModelDelay->appendRow({new QStandardItem("吸烟时间"),new QStandardItem("500")});
    m_pModelDelay->appendRow({new QStandardItem("吸烟结束等待时间"),new QStandardItem("200")});
    m_pModelDelay->appendRow({new QStandardItem("开始充电等待时间"),new QStandardItem("1000")});
    m_pModelDelay->appendRow({new QStandardItem("充电时间"),new QStandardItem("2000")});
    for(int i=0; i<6; i++)
    {
        m_pModelDelay->item(i,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        m_pModelDelay->item(i,0)->setEditable(false);
        m_pModelDelay->item(i,0)->setSelectable(false);
    }

    m_pModelGSwitch = new QStandardItemModel(this);
    m_pModelGSwitch->setHorizontalHeaderLabels(QString("名称,地址").split(','));
    ui->tableViewGSwitch->setModel(m_pModelGSwitch);
    pHeader = ui->tableViewGSwitch->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    pHeader->setSectionResizeMode(1,QHeaderView::Fixed) ;
    pHeader->resizeSection(1,70) ;
    ui->tableViewGSwitch->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pModelGSwitch->appendRow({new QStandardItem("静态电流总控1"),new QStandardItem("20")});
    m_pModelGSwitch->appendRow({new QStandardItem("静态电流总控2"),new QStandardItem("22")});
    m_pModelGSwitch->appendRow({new QStandardItem("吸烟气泵开关"),new QStandardItem("10")});
    for(int i=0; i<3; i++)
    {
        m_pModelGSwitch->item(i,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        m_pModelGSwitch->item(i,0)->setSelectable(false);
        m_pModelGSwitch->item(i,0)->setEditable(false);
        m_pModelGSwitch->item(i,1)->setCheckable(true);
    }

    connect(m_pModelGSwitch,&QStandardItemModel::itemChanged,this,[=](QStandardItem *item)
    {
        if(item->column() != 0)
        {
            setWitch(item->text().trimmed().toUInt(nullptr,16),item->checkState() == Qt::Checked) ;
        }
    });

    m_pModelChan = new QStandardItemModel(this);
    m_pModelChan->setHorizontalHeaderLabels(QString("通道号,启用,静态电流开关,切换电流表,充电开关,红灯开关,绿灯开关").split(','));
    ui->tableViewChan->setModel(m_pModelChan);
    pHeader = ui->tableViewChan->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    //ui->tableViewChan->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pHeader->setSectionResizeMode(0,QHeaderView::Fixed) ;
    pHeader->resizeSection(0,50) ;
    pHeader->setSectionResizeMode(1,QHeaderView::Fixed) ;
    pHeader->resizeSection(1,30) ;
    pHeader->setSectionResizeMode(2,QHeaderView::Fixed) ;
    pHeader->resizeSection(2,80) ;

    m_pModelChan->appendRow({new QStandardItem("1"),new QStandardItem(""),new QStandardItem("30"),new QStandardItem("5C"),new QStandardItem("41"),new QStandardItem("51"),new QStandardItem("50")});
    m_pModelChan->appendRow({new QStandardItem("2"),new QStandardItem(""),new QStandardItem("31"),new QStandardItem("5D"),new QStandardItem("42"),new QStandardItem("53"),new QStandardItem("52")});
    m_pModelChan->appendRow({new QStandardItem("3"),new QStandardItem(""),new QStandardItem("32"),new QStandardItem("5E"),new QStandardItem("43"),new QStandardItem("55"),new QStandardItem("54")});
    m_pModelChan->appendRow({new QStandardItem("4"),new QStandardItem(""),new QStandardItem("33"),new QStandardItem("5F"),new QStandardItem("44"),new QStandardItem("57"),new QStandardItem("56")});
    m_pModelChan->appendRow({new QStandardItem("5"),new QStandardItem(""),new QStandardItem("34"),new QStandardItem("11"),new QStandardItem("45"),new QStandardItem("59"),new QStandardItem("58")});
    m_pModelChan->appendRow({new QStandardItem("6"),new QStandardItem(""),new QStandardItem("35"),new QStandardItem("12"),new QStandardItem("46"),new QStandardItem("5B"),new QStandardItem("5A")});
    m_pModelChan->appendRow({new QStandardItem("7"),new QStandardItem(""),new QStandardItem("30"),new QStandardItem("5C"),new QStandardItem("41"),new QStandardItem("51"),new QStandardItem("50")});
    m_pModelChan->appendRow({new QStandardItem("8"),new QStandardItem(""),new QStandardItem("31"),new QStandardItem("5D"),new QStandardItem("42"),new QStandardItem("53"),new QStandardItem("52")});
    m_pModelChan->appendRow({new QStandardItem("9"),new QStandardItem(""),new QStandardItem("32"),new QStandardItem("5E"),new QStandardItem("43"),new QStandardItem("55"),new QStandardItem("54")});
    m_pModelChan->appendRow({new QStandardItem("10"),new QStandardItem(""),new QStandardItem("33"),new QStandardItem("5F"),new QStandardItem("44"),new QStandardItem("57"),new QStandardItem("56")});
    m_pModelChan->appendRow({new QStandardItem("11"),new QStandardItem(""),new QStandardItem("34"),new QStandardItem("11"),new QStandardItem("45"),new QStandardItem("59"),new QStandardItem("58")});
    m_pModelChan->appendRow({new QStandardItem("12"),new QStandardItem(""),new QStandardItem("35"),new QStandardItem("12"),new QStandardItem("46"),new QStandardItem("5B"),new QStandardItem("5A")});
    for(int i=0; i<12; i++)
    {
        m_pModelChan->item(i,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        m_pModelChan->item(i,0)->setSelectable(false);
        m_pModelChan->item(i,0)->setEditable(false);
        m_pModelChan->item(i,1)->setEditable(false);
        for(int j=1; j<=6; j++)
            m_pModelChan->item(i,j)->setCheckable(true);
        m_pModelChan->item(i,1)->setCheckState(Qt::Checked);
    }

    connect(m_pModelChan,&QStandardItemModel::itemChanged,this,[=](QStandardItem *item)
    {
        if(item->column() >= 2)
        {
            setWitch(item->text().trimmed().toUInt(nullptr,16),item->checkState() == Qt::Checked) ;
        }

        if(item->column() >= 1) emit onChanState(item->column(),item->checkState()) ;
    });

    // connect(ui->spinBox,&QSpinBox::valueChanged,this,[=](int value){
    //     m_pModelChan->setRowCount(0) ;
    //     for(int i=0; i<value; i++)
    //     {
    //         QStandardItem *item1 = new QStandardItem(QString("%1").arg(i+1));
    //         QStandardItem *item2 = new QStandardItem("");
    //         item1->setEditable(false);
    //         item1->setTextAlignment(Qt::AlignVCenter|Qt::AlignRight) ;
    //         item2->setEditable(false);
    //         item2->setCheckable(true);
    //         item2->setCheckState(Qt::Checked);
    //         m_pModelChan->appendRow({item1,item2});
    //     }
    // }) ;

    // ui->spinBox->setValue(1) ;
    // QTimer::singleShot(200,[=]{ ui->spinBox->setValue(12); });

    connect(ui->tableViewCOM,&QTableView::clicked,this,[=](const QModelIndex &index){

    });

    connect(m_pModelCOM,&QStandardItemModel::itemChanged,this,[=](QStandardItem *item){
        int row = item->row();
        int col = item->column();

        if(row == 6)
        {
            bool checked = (item->checkState() == Qt::Checked) ;
            if(col == 1)
            {
                if(m_pCOM1) m_pCOM1->closePort() ;

                if( checked )
                {
                    m_pCOM1->setPortName(m_pModelCOM->item(0,1)->text().trimmed()) ;
                    m_pCOM1->setPortParam(m_pModelCOM->item(1,1)->text().toInt()) ;
                    m_pCOM1->openPort() ;
                }
            }

            if(col == 2)
            {
                if(m_pCOM2) m_pCOM2->closePort() ;

                if( checked )
                {
                    m_pCOM2->setPortName(m_pModelCOM->item(0,2)->text().trimmed()) ;
                    m_pCOM2->setPortParam(m_pModelCOM->item(1,2)->text().toInt()) ;
                    m_pCOM2->openPort() ;
                }
            }

            updateCOMStatus() ;
        }
    });

    connect(m_pCOM1,&GenComport::onReceive,this,[=](const QByteArray &data){
        if(ui->checkBoxHex1->isChecked())
            ui->textEdit1->setText(data.toHex(' ').toUpper());
        else
            ui->textEdit1->setText(data.data());
    });

    connect(m_pCOM2,&GenComport::onReceive,this,[=](const QByteArray &data){
        if(ui->checkBoxHex2->isChecked())
            ui->textEdit2->setText(data.toHex(' ').toUpper());
        else
            ui->textEdit2->setText(data.data());

        m_buf2.append(data) ;
        if(m_buf2.size() == 9)
        {
            QByteArray tmp;
            tmp.append(m_buf2[6]);
            tmp.append(m_buf2[5]);
            tmp.append(m_buf2[4]);
            tmp.append(m_buf2[3]);
            float valueF =  *(float *) (tmp.data());

            int id = m_buf2[0] ;
            for(int i=0; i<6; i++)
            {
                if(m_pModelTGroup->item(i,1)->text().trimmed().toInt() == id)
                {
                    QStandardItem *item = m_pModelTGroup->item(i,5) ;
                    item->setText(QString::asprintf("%d",(int)(valueF*1000))) ;
                }
            }
        }
    });

    connect(ui->pushButton1,&QPushButton::clicked,this,[=]{
        m_buf1.clear();
        QString text = ui->lineEdit1->text().trimmed();
        m_pCOM1->send(text,!ui->checkBoxHex1->isChecked());
    });
    connect(ui->pushButton2,&QPushButton::clicked,this,[=]{
        m_buf2.clear();
        QString text = ui->lineEdit2->text().trimmed();
        m_pCOM2->send(text,!ui->checkBoxHex2->isChecked());
    });

}

DialogConfig::~DialogConfig()
{
    delete ui;
}


int DialogConfig::getChanCount()
{
    return m_pModelChan->rowCount();
}

Qt::CheckState DialogConfig::getChanState(int chanId)
{
    return m_pModelChan->item(chanId,1)->checkState() ;
}

void DialogConfig::updateCOMStatus()
{
    m_pModelCOM->item(5,1)->setCheckState(m_pCOM1->isOpen() ? Qt::Checked : Qt::Unchecked) ;
    m_pModelCOM->item(5,2)->setCheckState(m_pCOM2->isOpen() ? Qt::Checked : Qt::Unchecked) ;

    m_pModelCOM->item(5,1)->setText(m_pCOM1->isOpen() ? "串口已打开" : "串口未打开") ;
    m_pModelCOM->item(5,2)->setText(m_pCOM2->isOpen() ? "串口已打开" : "串口未打开") ;
}


void DialogConfig::setWitch(quint8 addr,bool on)
{
    QByteArray pack ;
    pack.append(0x55) ;
    pack.append(0xAA) ;
    pack.append(0xFF) ;
    pack.append(0xFF) ;
    pack.append(0x43) ;
    pack.append((char)0x00) ;

    quint16 len = 0x03 ;
    pack.append((char *)&len,2) ;

    pack.append(addr) ;
    pack.append((char)0x01) ;
    pack.append((char)(on ? 1 : 0)) ;

    quint16 crc = 0xFFFF;
    for (int pos = 0; pos < pack.size(); pos++)
    {
        crc ^= (quint8)pack[pos];
        for (int i = 8; i != 0; i--)
        {
            if ((crc & 0x0001) != 0)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    pack.append((char *)&crc,2);
    m_buf1.clear();

    if(m_pCOM1) m_pCOM1->send(pack) ;
}

void DialogConfig::SendTextCmd(const QString&text)
{
    if(m_pCOM2) m_pCOM2->send(text) ;
}

void DialogConfig::getMeter(quint8 id)
{
    // 01 03 00 02 00 02 65 CB
    QByteArray pack ;
    pack.append(id);
    pack.append((char)0x03);
    pack.append((char)0x00);
    pack.append((char)0x02);
    pack.append((char)0x00);
    pack.append((char)0x02);

    quint16 crc = 0xFFFF;
    for (int pos = 0; pos < pack.size(); pos++)
    {
        crc ^= (quint8)pack[pos];
        for (int i = 8; i != 0; i--)
        {
            if ((crc & 0x0001) != 0)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    pack.append((char *)&crc,2);
    m_buf2.clear();

    if(m_pCOM2) m_pCOM2->send(pack) ;
}
