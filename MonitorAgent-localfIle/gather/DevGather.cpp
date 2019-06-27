#include "DevGather.h"

#include <QDir>

//采集硬件温度信息
static const QString DEVICES_TEMP_BASE_PATH = "/sys/class/hwmon";
static const QString DEVICES_TEMP_HWOMN_PATH = "/sys/class/hwmon/%1";
static const QString DEVICES_TEMP_OTHER_PATH = "/sys/class/hwmon/%1/%2";
static const QString DEVICES_TEMP_NAME_PATH = "/sys/class/hwmon/%1/name";

static const QString DEV_INFO = "%1,%2,%3,%4,%5,%6";

DevGather::DevGather()
{
    m_init_success = false;
}

bool DevGather::initHwmonNames()
{
    if(m_init_success)
    {
        return true;
    }
    //获取/sys/class/hwmon下所有hwmon文件夹名
    QDir dir(DEVICES_TEMP_BASE_PATH);
    if(!dir.exists())
    {
         return false;
    }
    QStringList hwmonList = dir.entryList();

    for(QString& hwmon : hwmonList)
    {
        if(!(hwmon.contains("hwmon")))
        {
            hwmonList.removeOne(hwmon);
        }
    }
    if(hwmonList.isEmpty())
    {
        return false;
    }
    //打开每一个文件夹，获取文件夹内部的"name"文件内容
    for(QString& hwmon : hwmonList)
    {
        //进入某一个hwmon文件夹
        dir.cd(DEVICES_TEMP_HWOMN_PATH.arg(hwmon));
        //打开文件夹内的"name"文件
        QFile file(DEVICES_TEMP_NAME_PATH.arg(hwmon));
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            continue;
        }
        //读取"name"文件内容
        QString nameContent = file.readAll();
        file.close();
        //如果name文件内容为"coretemp"，说明该文件夹下是CPU温度文件
        if(nameContent.contains("coretemp"))
        {
            //列出该hwmon文件夹下所有的文件名,按从小到大的顺序
            QStringList fileNames = dir.entryList();
            for(QString& name : fileNames)
            {
                //找到所有含有"temp"的文件
                if(!name.contains("temp"))
                {
                    fileNames.removeOne(name);
                }
            }
            //如果文件数量不是5的倍数，说明有问题
            int length = fileNames.size();
            int count = length/5;
            if(length%5 != 0)
            {
                continue;
            }
            for(int i=0;i!=count;++i)
            {
                m_cputemp_crits.append(DEVICES_TEMP_OTHER_PATH.arg(hwmon,fileNames.at(i*5+0)));
                m_cputemp_alarms.append(DEVICES_TEMP_OTHER_PATH.arg(hwmon,fileNames.at(i*5+1)));
                m_cputemp_inputs.append(DEVICES_TEMP_OTHER_PATH.arg(hwmon,fileNames.at(i*5+2)));
                m_cputemp_labels.append(DEVICES_TEMP_OTHER_PATH.arg(hwmon,fileNames.at(i*5+3)));
                m_cputemp_maxs.append(DEVICES_TEMP_OTHER_PATH.arg(hwmon,fileNames.at(i*5+4)));
            }
        }
        //获取GPU的电压和风扇的转速
        else
        {
            QStringList gpuNames;
            QStringList fanNames;
            QStringList fileNames = dir.entryList();
            for(QString &fileName : fileNames)
            {
                if(fileName.indexOf("in")==0)
                {
                    gpuNames.append(fileName);
                }
                else if(fileName.indexOf("fan")==0)
                {
                    fanNames.append(fileName);
                }
            }
            //GPU文件数量应该是4的倍数
            int length = gpuNames.size();
            int count = length/4;
            if(length%4 != 0)
            {
                continue;
            }
            for(int i=0;i!=count;++i)
            {
                m_gpuload_inputs.append(DEVICES_TEMP_OTHER_PATH.arg(hwmon,gpuNames.at(i*4+0)));
                m_gpuload_labels.append(DEVICES_TEMP_OTHER_PATH.arg(hwmon,gpuNames.at(i*4+1)));
                m_gpuload_maxs.append(DEVICES_TEMP_OTHER_PATH.arg(hwmon,gpuNames.at(i*4+2)));
                m_gpuload_mins.append(DEVICES_TEMP_OTHER_PATH.arg(hwmon,gpuNames.at(i*4+3)));
            }
            length = fanNames.size();
            count = length/2;
            if(length%2 != 0)
            {
                continue;
            }
            for(int i=0;i!=count;++i)
            {
                m_fanspeed_inputs.append(DEVICES_TEMP_OTHER_PATH.arg(hwmon,fanNames.at(i*2+0)));
                m_fanspeed_labels.append(DEVICES_TEMP_OTHER_PATH.arg(hwmon,fanNames.at(i*2+1)));
            }
        }
    }
    return true;
}

/**
 * @brief DevGather::getHwmonInfo
 * @return min_cpu_temp,max_cpu_temp,min_gpu,max_gpu,min_fan,max_fan
 */
QString DevGather::getHwmonInfo()
{
    QFile file;
    int max_cpu = -10000,min_cpu = 10000,
        max_gpu = -10000,min_gpu = 10000,
        max_fan = -10000,min_fan = 10000;

    for(QString& name : m_cputemp_inputs)
    {
        file.setFileName(name);
        if(!file.open(QIODevice::ReadOnly))
        {
            continue;
        }
        int temp = file.readAll().replace("\n","").toInt()/1000;
        if(max_cpu < temp)
        {
            max_cpu = temp;
        }
        if(min_cpu > temp)
        {
            min_cpu = temp;
        }
        file.close();
    }
    for(QString& name : m_gpuload_inputs)
    {
        file.setFileName(name);
        if(!file.open(QIODevice::ReadOnly))
        {
            continue;
        }
        int temp = file.readAll().replace("\n","").toInt();
        if(max_gpu < temp)
        {
            max_gpu = temp;
        }
        if(min_gpu > temp)
        {
            min_gpu = temp;
        }
        file.close();
    }

    for(QString& name : m_fanspeed_inputs)
    {

        file.setFileName(name);
        if(!file.open(QIODevice::ReadOnly))
        {
            continue;
        }
        int temp = file.readAll().replace("\n","").toInt();
        if(max_fan < temp)
        {
            max_fan = temp;
        }
        if(min_fan > temp)
        {
            min_fan = temp;
        }
        file.close();
    }
    return DEV_INFO.arg(QString::number(min_cpu),
                        QString::number(max_cpu),
                        QString::number(min_gpu),
                        QString::number(max_gpu),
                        QString::number(min_fan),
                        QString::number(max_fan)
                        );
}
