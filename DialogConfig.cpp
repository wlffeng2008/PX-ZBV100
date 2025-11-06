#include "DialogConfig.h"
#include "ui_DialogConfig.h"

#include <QTimer>
#include <QSettings>
#include <QThread>

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
    m_pSet = new QSettings(QCoreApplication::applicationDirPath()+"/PXTestConfig.ini",QSettings::IniFormat) ;
    m_pSet->beginGroup("Config") ;

    pConfig = this ;

    m_pCOM1 = new GenComport(this);
    m_pCOM2 = new GenComport(this);

    m_pModelTGroup = new QStandardItemModel(this);
    m_pModelTGroup->setHorizontalHeaderLabels(QString("测试项目,起始端口,下限,上限,读取值,单位,读取").split(','));
    ui->tableViewTGroup->setModel(m_pModelTGroup);
    QHeaderView *pHeader = ui->tableViewTGroup->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    pHeader->setSectionResizeMode(0,QHeaderView::Fixed) ;
    pHeader->resizeSection(0,70) ;
    ui->tableViewTGroup->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList setPorts = m_pSet->value("485Ports" ,"1,7,7,20,30,40").toString().split(',') ;
    QStringList setMins  = m_pSet->value("MinValues","5.00,1000,100,4150,3500,-200").toString().split(',') ;
    QStringList setMaxs  = m_pSet->value("MaxValues","7.00,3000,600,4250,3600,-100").toString().split(',') ;

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

        m_pModelTGroup->item(i,1)->setText(setPorts[i]) ;
        m_pModelTGroup->item(i,2)->setText(setMins[i]) ;
        m_pModelTGroup->item(i,3)->setText(setMaxs[i]) ;
        for(int j=0; j<=6; j++)
        {
            QStandardItem *item = m_pModelTGroup->item(i,j) ;
            if(j == 0 || j == 4 || j == 5 || j == 6) item->setEditable(false);
            if(j > 0) item->setTextAlignment(Qt::AlignCenter) ;
        }
    }
    connect(m_pModelTGroup,&QStandardItemModel::itemChanged,this,[=](QStandardItem *item){
        QString strPorts ;
        QString strMins ;
        QString strMaxs ;
        for(int i=0; i<6; i++)
        {
            strPorts += m_pModelTGroup->item(i,1)->text() + ",";
            strMins  += m_pModelTGroup->item(i,2)->text() + ",";
            strMaxs  += m_pModelTGroup->item(i,3)->text() + ",";
        }
        m_pSet->setValue("485Ports" ,strPorts) ;
        m_pSet->setValue("MinValues",strMins) ;
        m_pSet->setValue("MaxValues",strMaxs) ;
    }) ;

    connect(ui->tableViewTGroup,&QTableView::clicked,this,[=](const QModelIndex &index){
        if(index.column() == 6)
        {
            readGroup(index.row());
            //m_pModelTGroup->item(index.row(),4)->setText("正在读取");
            //getMeter(m_pModelTGroup->item(index.row(),1)->text().trimmed().toInt()) ;
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

    m_pModelCOM->appendRow({new QStandardItem("串口"),new QStandardItem(m_pSet->value("COMPort1","COM4").toString().trimmed()),new QStandardItem(m_pSet->value("COMPort2","COM5").toString().trimmed())}) ;
    m_pModelCOM->appendRow({new QStandardItem("波特率"),new QStandardItem(m_pSet->value("COMBaud1","115200").toString().trimmed()),new QStandardItem(m_pSet->value("COMBaud2","9600").toString().trimmed())}) ;
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

    connect(m_pModelCOM,&QStandardItemModel::itemChanged,this,[=](QStandardItem *item){
        m_pSet->setValue("COMPort1",m_pModelCOM->item(0,1)->text().trimmed());
        m_pSet->setValue("COMPort2",m_pModelCOM->item(0,2)->text().trimmed());
        m_pSet->setValue("COMBaud1",m_pModelCOM->item(1,1)->text().trimmed());
        m_pSet->setValue("COMBaud2",m_pModelCOM->item(1,2)->text().trimmed());
    }) ;
    QTimer::singleShot(100,this,[=]{
        m_pModelCOM->item(6,1)->setCheckState(Qt::Checked) ;
        m_pModelCOM->item(6,2)->setCheckState(Qt::Checked) ;
    }) ;

    m_pModelGSwitch = new QStandardItemModel(this);
    m_pModelGSwitch->setHorizontalHeaderLabels(QString("名称,地址").split(','));
    ui->tableViewGSwitch->setModel(m_pModelGSwitch);
    pHeader = ui->tableViewGSwitch->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    pHeader->setSectionResizeMode(1,QHeaderView::Fixed) ;
    pHeader->resizeSection(1,70) ;
    ui->tableViewGSwitch->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pModelGSwitch->appendRow({new QStandardItem("静态电流总控1"),new QStandardItem(m_pSet->value("GSwitch1","20").toString())});
    m_pModelGSwitch->appendRow({new QStandardItem("静态电流总控2"),new QStandardItem(m_pSet->value("GSwitch2","22").toString())});
    m_pModelGSwitch->appendRow({new QStandardItem("吸烟气泵开关"),new QStandardItem(m_pSet->value("GSwitch3","10").toString())});
    for(int i=0; i<3; i++)
    {
        m_pModelGSwitch->item(i,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        m_pModelGSwitch->item(i,0)->setSelectable(false);
        m_pModelGSwitch->item(i,0)->setEditable(false);
        m_pModelGSwitch->item(i,1)->setCheckable(true);
    }

    connect(m_pModelGSwitch,&QStandardItemModel::itemChanged,this,[=](QStandardItem *item)
    {
        m_pSet->setValue("GSwitch1",m_pModelGSwitch->item(0,1)->text());
        m_pSet->setValue("GSwitch2",m_pModelGSwitch->item(1,1)->text());
        m_pSet->setValue("GSwitch3",m_pModelGSwitch->item(2,1)->text());
        if(item->column() != 0)
        {
            setWitch(item->text().trimmed().toUInt(nullptr,16),item->checkState() == Qt::Checked) ;
        }
    });

    m_pModelChan = new QStandardItemModel(this);
    m_pModelChan->setHorizontalHeaderLabels(QString("通道号,使能,静态电流开关,切换电流表,充电开关,红灯开关,绿灯开关").split(','));
    ui->tableViewChan->setModel(m_pModelChan);
    pHeader = ui->tableViewChan->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    ui->tableViewChan->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pHeader->setSectionResizeMode(0,QHeaderView::Fixed) ;
    pHeader->resizeSection(0,50) ;
    pHeader->setSectionResizeMode(1,QHeaderView::Fixed) ;
    pHeader->resizeSection(1,30) ;
    pHeader->setSectionResizeMode(2,QHeaderView::Fixed) ;
    pHeader->resizeSection(2,80) ;

    int chanCount = m_pSet->value("chanCount",12).toInt();
    for(int i=0; i<chanCount; i++)
        m_pModelChan->appendRow({new QStandardItem(QString::number(i+1)),new QStandardItem(""),new QStandardItem("30"),new QStandardItem("5C"),new QStandardItem("41"),new QStandardItem("51"),new QStandardItem("50")});

    connect(m_pModelChan,&QStandardItemModel::itemChanged,this,[=](QStandardItem *item) {
        int row = item->row();
        int col = item->column();
        if(col >= 2 && !m_bOutset) setWitch(item->text().trimmed().toUInt(nullptr,16),item->checkState() == Qt::Checked) ;
        if(col == 1)
        {
            emit onChanState(row,item->checkState()) ;
            for(int i=2; i<=6; i++)
                m_pModelChan->item(row,i)->setEnabled(item->checkState()==Qt::Checked);
        }

        QString strCol1;
        QString strCol2;
        QString strCol3;
        QString strCol4;
        QString strCol5;
        QString strCol6;
        for(int i=0; i<m_pModelChan->rowCount(); i++)
        {
            strCol1 += (m_pModelChan->item(i,1)->checkState() == Qt::Checked) ? "1,":"0,";
            strCol2 += m_pModelChan->item(i,2)->text() + "," ;
            strCol3 += m_pModelChan->item(i,3)->text() + "," ;
            strCol4 += m_pModelChan->item(i,4)->text() + "," ;
            strCol5 += m_pModelChan->item(i,5)->text() + "," ;
            strCol6 += m_pModelChan->item(i,6)->text() + "," ;
        }
        m_pSet->setValue("Col1",strCol1);
        m_pSet->setValue("Col2",strCol2);
        m_pSet->setValue("Col3",strCol3);
        m_pSet->setValue("Col4",strCol4);
        m_pSet->setValue("Col5",strCol5);
        m_pSet->setValue("Col6",strCol6);
    });

    QStringList Col1 = m_pSet->value("Col1","1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0").toString().split(',') ;
    QStringList Col2 = m_pSet->value("Col2","30,31,32,33,34,35,30,31,32,33,34,35,30,31,32,33,34,35,30,31,32,33,34,35,30,31,32,33,34,35,30,31,32,33,34,35,30,31,32,33,34,35,30,30,31,32,33,34,35,30,50,52,54,56,58,5A,0").toString().split(',') ;
    QStringList Col3 = m_pSet->value("Col3","5C,5D,5E,5F,11,12,5C,5D,5E,5F,11,12,5C,5D,5E,5F,11,12,5C,5D,5E,5F,11,12,5C,5D,5E,5F,11,12,5C,5D,5E,5F,11,12,5C,5D,5E,5F,11,12,5C,5D,5E,5F,11,12,50,52,54,56,58,5A,58,5A,0").toString().split(',') ;
    QStringList Col4 = m_pSet->value("Col4","41,42,43,44,45,46,41,42,43,44,45,46,41,42,43,44,45,46,41,42,43,44,45,46,41,42,43,44,45,46,41,42,43,44,45,46,41,42,43,44,45,46,41,42,43,44,45,46,50,52,54,56,58,5A,56,58,0").toString().split(',') ;
    QStringList Col5 = m_pSet->value("Col5","51,53,55,57,59,5B,51,53,55,57,59,5B,51,53,55,57,59,5B,51,53,55,57,59,5B,51,53,55,57,59,5B,51,53,55,57,59,5B,51,53,55,57,59,5B,51,51,53,55,57,59,5B,51,50,52,54,56,58,5A,0").toString().split(',') ;
    QStringList Col6 = m_pSet->value("Col6","50,52,54,56,58,5A,50,52,54,56,58,5A,50,52,54,56,58,5A,50,52,54,56,58,5A,50,52,54,56,58,5A,50,52,54,56,58,5A,50,52,54,56,58,5A,50,52,54,56,58,5A,50,52,54,56,58,5A,54,56,0").toString().split(',') ;
    for(int i=0; i<m_pModelChan->rowCount(); i++)
    {
        m_pModelChan->item(i,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        m_pModelChan->item(i,0)->setSelectable(false);
        m_pModelChan->item(i,0)->setEditable(false);
        m_pModelChan->item(i,1)->setEditable(false);
        for(int j=1; j<=6; j++)
            m_pModelChan->item(i,j)->setCheckable(true);
        if(i >= Col1.count() || Col1[i].trimmed().isEmpty())
            continue;

        m_pModelChan->item(i,1)->setCheckState(Col1[i] == "1" ? Qt::Checked:Qt::Unchecked);
        m_pModelChan->item(i,2)->setText(Col2[i]) ;
        m_pModelChan->item(i,3)->setText(Col3[i]) ;
        m_pModelChan->item(i,4)->setText(Col4[i]) ;
        m_pModelChan->item(i,5)->setText(Col5[i]) ;
        m_pModelChan->item(i,6)->setText(Col6[i]) ;
    }

    QTimer::singleShot(500,this,[=]{
        srand(time(nullptr));
        for(int i=0; i<m_pModelChan->rowCount(); i++)
        {
            emit onChanState(i,m_pModelChan->item(i,1)->checkState());
            // emit onReadBack(i,1,100.0,1) ;
            // emit onReadBack(i,2,120.0,1) ;
            // emit onReadBack(i,3,130.0,1) ;
        }
    });


    ui->spinBox->setValue(chanCount);
    connect(ui->spinBox,&QSpinBox::valueChanged,this,[=](int value){
        m_pSet->setValue("chanCount",value);
    }) ;

    connect(ui->tableViewCOM,&QTableView::clicked,this,[=](const QModelIndex &index){ });

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
                    m_pCOM1->setPortName (m_pModelCOM->item(0,1)->text().trimmed()) ;
                    m_pCOM1->setPortParam(m_pModelCOM->item(1,1)->text().toInt()) ;
                    m_pCOM1->openPort() ;
                }
            }

            if(col == 2)
            {
                if(m_pCOM2) m_pCOM2->closePort() ;

                if( checked )
                {
                    m_pCOM2->setPortName (m_pModelCOM->item(0,2)->text().trimmed()) ;
                    m_pCOM2->setPortParam(m_pModelCOM->item(1,2)->text().toInt()) ;
                    m_pCOM2->openPort() ;
                }
            }

            updateCOMStatus() ;
        }
    });

    static QByteArray Acq = QByteArray::fromHex(QString("55 AA 00 00 53 00 01 00 33 D6 7D").toLatin1()) ;

    connect(m_pCOM1,&GenComport::onReceive,this,[=](const QByteArray &data){
        if(ui->checkBoxHex1->isChecked())
            ui->textEdit1->setText(data.toHex(' ').toUpper());
        else
            ui->textEdit1->setText(data.data());

        if(Acq == data) emit onAutoTestAcq() ;
    });

    connect(m_pCOM2,&GenComport::onReceive,this,[=](const QByteArray &data){
        if(ui->checkBoxHex2->isChecked())
            ui->textEdit2->setText(data.toHex(' ').toUpper());
        else
            ui->textEdit2->setText(data.data());

        m_buf2.append(data) ;
        if(m_buf2.size() >= 9)
        {
            QByteArray tmp;
            tmp.append(m_buf2[6]);
            tmp.append(m_buf2[5]);
            tmp.append(m_buf2[4]);
            tmp.append(m_buf2[3]);

            float value =  *(float *) (tmp.data()) ;

            int chanId = m_buf2[0] - m_readPort;

            bool ok = false ;
            if(m_max >= value && value >= m_min) ok = true ;

            emit onReadBack(chanId,m_readType + 1,value,ok);

            QTimer::singleShot(50,this,[=]{ readNext(); });
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

    connect(ui->checkBoxRed,&QCheckBox::clicked,this,[=](bool checked){
        for(int i=0; i<m_pModelChan->rowCount(); i++)
        {
            if(m_pModelChan->item(i,1)->checkState() == Qt::Checked)
                setLED_R(i,checked) ;
        }
    });

    connect(ui->checkBoxGreen,&QCheckBox::clicked,this,[=](bool checked){
        for(int i=0; i<m_pModelChan->rowCount(); i++)
        {
            if(m_pModelChan->item(i,1)->checkState() == Qt::Checked)
                setLED_G(i,checked) ;
        }
    });

    ui->checkBoxNoitfy->setChecked(m_pSet->value("TestNotify").toBool());
    connect(ui->checkBoxNoitfy,&QCheckBox::clicked,this,[=](bool checked){
        m_pSet->setValue("TestNotify",checked);
    });
}

DialogConfig::~DialogConfig()
{
    delete ui;
}

bool DialogConfig::isTestNotify()
{
    return ui->checkBoxNoitfy->isChecked() ;
}

bool DialogConfig::isConnected()
{
    return (m_pCOM1 && m_pCOM1->isOpen() && m_pCOM2 && m_pCOM2->isOpen());
}

void DialogConfig::reset()
{
    for(int i=0; i<m_pModelChan->rowCount(); i++)
    {
        setLED_R(i,false) ;
        setLED_G(i,false) ;
    }
}


int DialogConfig::getChanCount()
{
    return m_pModelChan->rowCount();
}


void DialogConfig::updateCOMStatus()
{
    m_pModelCOM->item(5,1)->setCheckState(m_pCOM1->isOpen() ? Qt::Checked : Qt::Unchecked) ;
    m_pModelCOM->item(5,2)->setCheckState(m_pCOM2->isOpen() ? Qt::Checked : Qt::Unchecked) ;

    m_pModelCOM->item(5,1)->setText(m_pCOM1->isOpen() ? "串口已打开" : "串口未打开") ;
    m_pModelCOM->item(5,2)->setText(m_pCOM2->isOpen() ? "串口已打开" : "串口未打开") ;
}

static void AppendCrc16(QByteArray&pack)
{
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
}

void DialogConfig::setWitch(quint8 addr,bool on)
{
    QByteArray pack ;
    pack.append((char)0x55) ;
    pack.append((char)0xAA) ;
    pack.append((char)0xFF) ;
    pack.append((char)0xFF) ;
    pack.append((char)0x43) ;
    pack.append((char)0x00) ;

    quint16 len = 0x03 ;
    pack.append((char *)&len,2) ;

    pack.append(addr) ;
    pack.append((char)0x01) ;
    pack.append((char)(on ? 1 : 0)) ;

    AppendCrc16(pack) ;

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

    AppendCrc16(pack) ;

    m_buf2.clear();

    if(m_pCOM2) m_pCOM2->send(pack) ;
}


void DialogConfig::setLED_R(int chanId,bool on)
{
    m_bOutset=true ;
    QStandardItem *item = m_pModelChan->item(chanId,5) ;
    Qt::CheckState state = on ? Qt::Checked:Qt::Unchecked;
    item->setCheckState(state) ;
    setWitch(item->text().toInt(nullptr,16),on) ;
    m_bOutset=false ;
}

void DialogConfig::setLED_G(int chanId,bool on)
{
    m_bOutset=true ;
    QStandardItem *item = m_pModelChan->item(chanId,6) ;
    Qt::CheckState state = on ? Qt::Checked:Qt::Unchecked;
    item->setCheckState(state) ;
    setWitch(item->text().toInt(nullptr,16),on) ;
    m_bOutset=false ;
}

void DialogConfig::setGSwitch(int type,bool on)
{
    if(type==0)
    {
        m_pModelGSwitch->item(0,1)->setCheckState(on ?Qt::Checked:Qt::Unchecked) ;
        m_pModelGSwitch->item(1,1)->setCheckState(on ?Qt::Checked:Qt::Unchecked) ;
    }
    else
    {
        m_pModelGSwitch->item(2,1)->setCheckState(on ?Qt::Checked:Qt::Unchecked) ;
    }
}

void DialogConfig::readGroup(int type)
{
    m_max = m_pModelTGroup->item(type,3)->text().trimmed().toFloat() ;
    m_min = m_pModelTGroup->item(type,2)->text().trimmed().toFloat() ;

    quint8 port = m_pModelTGroup->item(type,1)->text().trimmed().toInt() ;
    m_readPort = port;
    m_readType = type;
    m_reads.clear() ;
    for(int i=0; i<m_pModelChan->rowCount(); i++)
    {
        if(m_pModelChan->item(i,1)->checkState() == Qt::Checked)
        {
            m_reads.push_back(port + i);
        }
    }
    readNext() ;
}

void DialogConfig::readNext()
{
    if(m_reads.count())
    {
        quint8 port = m_reads[0] ;
        m_reads.pop_front();
        getMeter(port) ;
    }
}

void DialogConfig::setGroupSwitch(int type,bool on)
{
    for(int i=0; i<m_pModelTGroup->rowCount(); i++)
    {
        if(m_pModelChan->item(i,1)->checkState() == Qt::Checked)
            m_pModelChan->item(i,type+2)->setCheckState(on?Qt::Checked:Qt::Unchecked);
    }
}

void DialogConfig::doTesting(int index)
{
    switch(index)
    {
    case 0:
        m_pModelGSwitch->item(0,1)->setCheckState(Qt::Checked);
        m_pModelGSwitch->item(1,1)->setCheckState(Qt::Checked);
        break;
    case 1: setGroupSwitch(0,true); break;
    case 2: break;
    case 3: readGroup(0); break;
    case 4: setGroupSwitch(1,true); break;
    case 5:
        m_pModelGSwitch->item(0,1)->setCheckState(Qt::Unchecked);
        m_pModelGSwitch->item(1,1)->setCheckState(Qt::Unchecked);
        break;
    case 6: setGroupSwitch(0,false); break;
    case 7:m_pModelGSwitch->item(2,1)->setCheckState(Qt::Checked); break;
    case 8:break;
    case 9:readGroup(3);break;
    case 10:readGroup(1);break;
    case 11:readGroup(4);break;
    case 12:break;
    case 13:m_pModelGSwitch->item(2,1)->setCheckState(Qt::Unchecked);break;
    case 14:break;
    case 15:setGroupSwitch(2,true);break;
    case 16:break;
    case 17:readGroup(2);break;
    case 18:break;
    case 19:readGroup(3);break;
    case 20:setGroupSwitch(2,false);break;
    case 21:setGroupSwitch(1,false);break;
    }
}
