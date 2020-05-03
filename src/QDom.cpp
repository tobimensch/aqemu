/***************************************************************************
** The MIT License (MIT)
** Copyright (c) 2016 Tobias Gläßer
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the "Software"),
** to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense,
** and/or sell copies of the Software, and to permit persons to whom the Software
** is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
** DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
** ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/

#include "QDom.h"

using namespace TinyXML2QDomWrapper;
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
        for (auto el = element->FirstChildElement(); el != nullptr; el = el->NextSiblingElement())
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
        return QString("");
}

const QString QDomElement::text() const
{
    if ( element )
        return element->GetText();
    else
        return QString("");
}

bool QDomElement::isNull()
{
    //TODO
    if ( element )
    {
        if ( element->NoChildren() )
        {
            if ( element->GetText() != NULL && strcmp(element->GetText(),"") != 0 )
                return false;
            else if ( element->FirstAttribute() != NULL )
                return false;
            else if  ( tagName() != "" )
                return false;
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
        return QString("");
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
            auto test =  node->ToElement();
            if ( test != NULL )
            {
                if ( test->GetText() != NULL && strcmp(test->GetText(),"") != 0 )
                    return false;
                else if ( test->FirstAttribute() != NULL )
                    return false;
                else if ( test->Name() != NULL )
                    return false;
                else
                    return true;
            }
            auto test2 = node->ToText();
            if ( test2 != NULL )
                return false;
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

QDomDocument::~QDomDocument()
{
    delete document;
    document = nullptr; 
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
    return QDomProcessingInstruction();
}

bool QDomDocument::setContent(QFile* file, bool b, QString* s, int* i, int* i2)
{
    if ( document )
    {
        XMLError err = document->LoadFile(file->fileName().toUtf8());
        if ( err == XML_SUCCESS )
        {
            return true;
        }
        else
        {
            const char* err_str =  document->ErrorStr();
            if ( err_str != NULL )
                s->fromLatin1(err_str);
            
            if ( i )
            {
                auto lineNum = document->GetLineNum();
                if ( lineNum != 0 )
                {
                    *i = lineNum;
                }
                else
                {
                    *i = -1;
                }
            }
            //can't really implement col of err, since TinyXML2 doesn't seem to support that
            if ( i2 )
                *i2 = -1;

            return false;
        }
    }
    else
    {
        return false;
    }
}

