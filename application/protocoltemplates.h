#ifndef PROTOCOLTEMPLATES_H
#define PROTOCOLTEMPLATES_H

#include <QString>
#include <QHash>
#include <QDomDocument>

template<class ProtocolTemplate>
QString serializeProtocolTemplateMap(const QHash<QString, ProtocolTemplate> &tm);

template<class ProtocolTemplate>
bool parseProtocolTemplate(QDomNode& n, ProtocolTemplate& pt);

template<class ProtocolTemplate>
int deserializeProtocolTemplateMap(const QString &content, QHash<QString, ProtocolTemplate> &tm);

template<class ProtocolTemplate>
unsigned int getProtocolTemplateChannels(const ProtocolTemplate &ptemplate);

template<class ProtocolTemplate>
const QString& getProtocolTemplateName(const ProtocolTemplate& pt);

template<class ProtocolTemplate>
void setProtocolTemplateName(ProtocolTemplate& pt, const QString &name);
#endif // PROTOCOLTEMPLATES_H
