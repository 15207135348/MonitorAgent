#include "ConfigUtil.h"
#include <QFile>
#include <QMap>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDir>
#include <QCoreApplication>

#define XML_PATH "/etc/monitor-agent.xml"


ConfigUtil::ConfigUtil()
{

}

QMap<QString,QString> ConfigUtil::loadConf()
{
    QMap<QString,QString> map;
    QFile file(XML_PATH);
    if(!file.exists() || !file.open(QIODevice::ReadOnly))
    {
        map["host.address"] = "localhost";
        map["host.port"] = "3690";
        map["gather.internal"] = "5000";
        map["user.name"] = "admin";
        map["self.start"] = "true";
        return map;
    }
    QList<QString> names;
    QList<QString> values;
    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement())
        {
            if(reader.name() == "name"){
                names.append(reader.readElementText());
            }
            if(reader.name() == "value"){
                values.append(reader.readElementText());
            }
        }
    }
    if(names.length() == values.length()){
        for(int i=0;i!=names.length();++i){
            map[names.at(i)] = values.at(i);
        }
    }
    file.close();
    return map;
}

void ConfigUtil::saveConf(const QMap<QString,QString>& map)
{
    QFile file(XML_PATH);
    if (!file.open(QIODevice::WriteOnly)){
        printf("/etc/monitor-agent.xml open failed.\r\n");
        return;
    }
    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("configuration");
    for(auto it = map.keyValueBegin(); it!=map.keyValueEnd(); ++it)
    {
        QString name = (*it).first;
        QString value = (*it).second;
        writer.writeStartElement("property");

        writer.writeStartElement("name");
        writer.writeCharacters(name);
        writer.writeEndElement();

        writer.writeStartElement("value");
        writer.writeCharacters(value);
        writer.writeEndElement();

        writer.writeEndElement();
    }
    writer.writeEndElement();
    writer.writeEndDocument();
}
void ConfigUtil::setSelfStart(bool selfstart)
{
    QFile file("/etc/rc.local");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    QString data = file.readAll();
    file.close();
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QList<QString> list = data.split("\n");
    if(selfstart && !list.contains("#set monitor agent self-start"))
    {
        int pos = list.lastIndexOf("exit 0");
        list.insert(pos, "#set monitor agent self-start");
        list.insert(pos+1, QCoreApplication::applicationFilePath());
    }
    else if(!selfstart && list.contains("#set monitor agent self-start"))
    {
        int pos = list.indexOf("#set monitor agent self-start");
        list.removeAt(pos);
        list.removeAt(pos);
    }
    for(QString s : list)
    {
        if(s.isEmpty()) continue;
        file.write(s.append("\n").toLatin1());
    }
    file.close();
}

