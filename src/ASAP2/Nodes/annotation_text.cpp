#include "Nodes/annotation_text.h"
#include "characteristic.h"
#include <QMessageBox>
#include "a2lgrammar.h"


//initialise static variables
Factory<Node,ANNOTATION_TEXT> ANNOTATION_TEXT::nodeFactory;

// Functions (Predicate)
bool nodeLessThan( const Node *a, const Node *b );
bool itemLessThan( const Item *a, const Item *b );

ANNOTATION_TEXT::ANNOTATION_TEXT(QTextStream &in, Node *parentNode )
    : Node(parentNode, parentNode->lex, parentNode->errorList)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->annotation_text.namePar;
    typePar = &gram->annotation_text.typePar;
    factoryOptNode = &gram->annotation_text.factoryOptNode;
    factoryOptItem = &gram->annotation_text.factoryOptItem;

    //Set the line where the Node starts in ASAP file
    this->a2lLine = lex->getLine();

    //Parse Mandatory PARAMETERS
    //parseFixPar(typePar ,in);
    name = (char*)"ANNOTATION_TEXT";

    TokenTyp token = parseListString(in);

    //End
    if (token == BlockEnd)
    {
        token = nextToken(in);
        if (token == Keyword && lex->getLexem() == "ANNOTATION_TEXT")
        {
        }
        else
        {
            QString s(lex->toString(token).c_str());
            this->showError("expected token : BlockEnd ANNOTATION_TEXT\nfind token : " + s );
        }
    }
    else
    {
        QString s1(lex->toString(token).c_str());
        QString s2(lex->getLexem().c_str());
        this->showError("expected end ANNOTATION_TEXT\nfind : " + s1 + " " + s2 );
    }
}

ANNOTATION_TEXT::~ANNOTATION_TEXT()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

void ANNOTATION_TEXT::parseFixPar(QList<TokenTyp> *typePar, QTextStream &in)
{
    //Mandatory PARAMETERS
    TokenTyp token;
    token = this->nextToken(in);
    if (token == String)
    {
        std::string str;
        while (token == String)
        {
            str += lex->getLexem();
            token = this->nextToken(in);
        }
        //parameters.append(str);
        char *c = new char[str.length()+1];
        strcpy(c, str.c_str());
        parameters.append(c);
    }
    else
    {
        QString t(lex->toString(typePar->at(0)).c_str());
        QString s(lex->toString(token).c_str());
        this->showError("expected token : " + t +"\nfind token : " + s );
    }

}

TokenTyp ANNOTATION_TEXT::parseOptPar(QMap<std::string, Occurence> *nameOptPar, QTextStream &in)
{

    if (nameOptPar->isEmpty())
        return nextToken(in);
    else
    {
        TokenTyp token = nextToken(in);
        while (token == BlockBegin || token == Keyword)
        {
            //Nodes
            if (token == BlockBegin)
            {
                token = this->nextToken(in);
                if (token == Keyword)
                {
                    std::string lexem = lex->getLexem();
                    if (factoryOptNode->contains(lexem))
                    {
                        if (this->occOptPar->value(lexem) == ZeroOrOne)
                        {
                            this->occOptPar->insert(lexem, Zero);
                            Node  *instance = factoryOptNode->value(lexem)->createInstance(in, this);
                            this->addChildNode(instance);
                            token = nextToken(in);
                        }
                        else if (this->occOptPar->value(lexem) == ZeroOrMore)
                        {
                            Node  *instance = factoryOptNode->value(lexem)->createInstance(in, this);
                            this->addChildNode(instance);
                            token = nextToken(in);
                        }
                        else
                        {
                            QString s(lexem.c_str());
                            this->showError(" Keyword : " + s + " can only be once declared");
                            return token;
                        }
                    }
                    else
                    {
                        QString s(lexem.c_str());
                        this->showError("unknown Keyword : " + s );
                        return token;
                    }
                }
                else
                {
                    QString s(lex->toString(token).c_str());
                    this->showError("expected token : BlockBegin or Keyword\nfind token : " + s );
                    return token;
                }
            }
            //Items
            else if (token == Keyword)
            {
                std::string lexem = lex->getLexem();
                if (factoryOptItem->contains(lexem))
                {
                    if (this->occOptPar->value(lexem) == ZeroOrOne)
                    {
                        this->occOptPar->insert(lexem, Zero);
                        Item  *instance = factoryOptItem->value(lexem)->createInstance(in, this);
                        this->addOptItem(instance);
                        token = nextToken(in);
                    }
                    else if (this->occOptPar->value(lexem) == ZeroOrMore)
                    {
                        Item  *instance = factoryOptItem->value(lexem)->createInstance(in, this);
                        this->addOptItem(instance);
                        token = nextToken(in);
                    }
                    else
                    {
                        QString s(lexem.c_str());
                        this->showError(" Keyword : " + s + " can only be once declared");
                        return token;
                    }
                }
                else
                {
                    QString s(lexem.c_str());
                    this->showError("unknown Keyword : " + s );
                    return token;
                }
            }
        }
        return token;
    }
}

QMap<std::string, std::string> *ANNOTATION_TEXT::getParameters()
{
    QMap<std::string, std::string> *par = new  QMap<std::string, std::string>;
    for (int i = 0; i < namePar->count(); i++)
    {
        par->insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char* ANNOTATION_TEXT::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

// only for ANNOTATION_TEXT
TokenTyp ANNOTATION_TEXT::parseListString(QTextStream &in)
{
    TokenTyp token = lex->getNextToken(in);
    QString str;

    while (token == String)
    {
        str.append(lex->getLexem().c_str());
        token = lex->getNextToken(in);
    }

    char *c = new char[str.length() + 1];
    strcpy(c, str.toLocal8Bit().data());
    parameters.append(c);
    return token;
}
