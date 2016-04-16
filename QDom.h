
#ifndef QDOM_H
#define QDOM_H

#include <QFile>
#include <QTextStream>

#include "tinyxml2.h"

using namespace tinyxml2;

class QDomNode;
class QDomElement;
class QDomProcessingInstruction;

class QDomText
{
    friend class QDomElement;
    friend class QDomDocument;

    public:
        QDomText();
        QDomText(XMLText*);
        //QDomText(const QDomText&);
    private:
        XMLText* text;
};

class QDomElement
{
    friend class QDomDocument;

    public:
        QDomElement();
        QDomElement(XMLElement* _el);
        void appendChild(const QDomText&);
        void appendChild(const QDomElement&);
        void setAttribute(const QString&, const QString&);
        const QDomNode firstChild() const;
        const QDomElement firstChildElement() const;
        const QDomElement firstChildElement(const QString&) const;
        QList<QDomElement> childNodes();
        bool hasAttribute(const QString&);
        QString attribute(const QString&);
        const QString text() const;
        bool isNull();
        QString tagName();
    private:
        XMLElement* element;
};

class QDomNode
{
    friend class QDomElement;
    friend class QDomDocument;

    public:
        QDomNode(XMLNode* _node);
        QDomNode();
        const QDomNode nextSibling();
        bool isNull();
        const QDomElement toElement() const;

    private:
        QDomNode* next;
        XMLNode* node;
};

class QDomDocument : public QDomNode
{
    friend class QDomElement;

    public:
        QDomDocument();
        void appendChild(const QDomText&);
        void appendChild(const QDomElement&);
        void appendChild(const QDomProcessingInstruction&);
        QDomDocument(const QString&);
        QDomElement documentElement();
        QDomElement createElement(const QString&);
        QDomText createTextNode(const QString&);
        void save(QTextStream&,int);
        QDomProcessingInstruction createProcessingInstruction(const QString&, const QString&);
        bool setContent(QFile* file, bool b, QString* s, int* i, int* i2);
    private:
        XMLDocument* document;
};


class QDomProcessingInstruction
{
    public:
    private:
};


#endif

