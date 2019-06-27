#include "ConfigUtil.h"
#include <QFile>
#include <QMap>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

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

