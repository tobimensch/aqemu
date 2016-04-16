
#include "QDom.h"

//QDomText

QDomText::QDomText()
{
    text = 0;
}

/*
QDomText::QDomText(const QDomText& t)
{
    //text = new XMLText(*t.text);
}
*/

QDomText::QDomText(XMLText* _text)
{
    text = _text;
}

//QDomElement

QDomElement::QDomElement()
{
    element = 0;
}

QDomElement::QDomElement(XMLElement* _el)
{
    element = _el;
}

void QDomElement::appendChild(const QDomText& t)
{
    if ( element )
        element->InsertEndChild(t.text);
}

void QDomElement::appendChild(const QDomElement& e)
{
    if ( element )
        element->InsertEndChild(e.element);
}

void QDomElement::setAttribute(const QString& s, const QString& s2)
{
    if ( element )
        element->SetAttribute(s.toLatin1().constData(),s2.toLatin1().constData());
}

const QDomNode QDomElement::firstChild() const
{
    if ( element )
        return QDomNode(element->FirstChild());
    else
        return QDomNode();
}

const QDomElement QDomElement::firstChildElement() const
{
    if ( element )
        return QDomElement(element->FirstChildElement());
    else
        return QDomElement();
}

const QDomElement QDomElement::firstChildElement(const QString& s) const
{
    if ( element )
        return QDomElement(element->FirstChildElement(s.toLatin1()));
    else
        return QDomElement();
}

QList<QDomElement> QDomElement::childNodes()
{
    if ( element )
    {
        QList<QDomElement> list;
        for (auto el = element->FirstChildElement(); el = el->NextSiblingElement();)
        {
            list.append(el);
        }
        return list;
    }
    else
        return QList<QDomElement>();
}

bool QDomElement::hasAttribute(const QString& s)
{
    if ( element )
    {
        const char* c = element->Attribute(s.toLatin1());
        if ( c == NULL )
            return false;
        else
            return true;
    }
    else
    {
        return false;
    }
}

QString QDomElement::attribute(const QString& s)
{
    if ( element )
        return element->Attribute(s.toLatin1());
    else
        return QString();
}

const QString QDomElement::text() const
{
    if ( element )
        return element->GetText();
    else
        return QString();
}

bool QDomElement::isNull()
{
    //TODO
    if ( element )
    {
        if ( element->NoChildren() )
        {
            return true;
        }
        else
            return false;
    }
    else
        return true;
}

QString QDomElement::tagName()
{
    if ( element )
        return element->Name();
    else
        return QString();
}

//QDomNode

QDomNode::QDomNode(XMLNode* _node)
{
    node = _node;
}

QDomNode::QDomNode()
{
    node = 0;
}

const QDomNode QDomNode::nextSibling()
{
    if ( node )
        return QDomNode(node->NextSibling());
    else
        return QDomNode();
}

bool QDomNode::isNull()
{
    if ( node )
    {
        if ( node->NoChildren() )
        {
            return true;
        }
        else
            return false;
    }
    else
        return true;
    //TODO
}

const QDomElement QDomNode::toElement() const
{
    if ( node )
        return QDomElement(node->ToElement());
    else
        return QDomElement();
}

//QDomDocument

QDomDocument::QDomDocument()
{
    document = new XMLDocument();
}


QDomDocument::QDomDocument(const QString&)
{
    document = new XMLDocument();
    //TODO
}

void QDomDocument::appendChild(const QDomText& t)
{
    if ( document )
        document->InsertEndChild(t.text);
}
    

void QDomDocument::appendChild(const QDomElement& e)
{
    if ( document )
        document->InsertEndChild(e.element);
}

void QDomDocument::appendChild(const QDomProcessingInstruction& p)
{
    //TODO: stub
}

QDomElement QDomDocument::documentElement()
{   
    if ( document )
        return QDomElement(document->RootElement());
    else
        return QDomElement();
}

QDomElement QDomDocument::createElement(const QString& s)
{
    if ( document )
        return QDomElement(document->NewElement(s.toLatin1()));
    else
        return QDomElement();
}

QDomText QDomDocument::createTextNode(const QString& s)
{
    if ( document )
        return QDomText(document->NewText(s.toLatin1()));
    else
        return QDomText();
}

void QDomDocument::save(QTextStream& stream,int)
{
    if ( document )
    {
        XMLPrinter printer;
        document->Print(&printer);
        stream << printer.CStr();
    }
}

QDomProcessingInstruction QDomDocument::createProcessingInstruction(const QString&, const QString&)
{
    //TODO
}

bool QDomDocument::setContent(QFile* file, bool b, QString* s, int* i, int* i2)
{
    if ( document )
    {
        XMLError err = document->LoadFile(file->fileName().toLatin1());
        if ( err == XML_NO_ERROR )
        {
            return true;
        }
        else
        {
            const char* err_str =  document->GetErrorStr1();
            if ( err_str != NULL )
                s->fromLatin1(err_str);
            else
                s = new QString(err_str);

            //can't really implement line/col of err, since TinyXML2 doesn't seem to support that
            *i = -1;
            *i2 = -1;

            return false;
        }
    }
    else
    {
        return false;
    }
}

