#include "measurement.h"
#include <QMessageBox>
#include "a2lgrammar.h"

//initialise static variables
Factory<Node,MEASUREMENT> MEASUREMENT::nodeFactory;

// Functions (Predicate)
bool nodeLessThan( const Node *a, const Node *b );
bool itemLessThan( const Item *a, const Item *b );

MEASUREMENT::MEASUREMENT(QTextStream &in, Node *parentNode)
    : Node(parentNode, parentNode->lex, parentNode->errorList)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->measurement.namePar;
    typePar = &gram->measurement.typePar;
    factoryOptNode = &gram->measurement.factoryOptNode;
    factoryOptItem = &gram->measurement.factoryOptItem;

    //Set the line where the Node starts in ASAP file
    this->a2lLine = lex->getLine();

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar, in);
    if (parameters.count() > 0)
        name = parameters.at(0);
    else
        name = (char*)"measurement";

    //Parse optional PARAMETERS
    TokenTyp token = parseOptPar(in);

    //End
    if (token == BlockEnd)
    {
        token = nextToken(in);
        if (token == Keyword && lex->getLexem() == "MEASUREMENT")
        {
            //Sort the childNodes
            //qSort(this->childNodes.begin(), this->childNodes.end(), nodeLessThan);

            //Sort The childItems
            //qSort(this->optItems.begin(), this->optItems.end(), itemLessThan);
        }
        else
        {
            QString s(lex->toString(token).c_str());
            this->showError("expected token : BlockEnd MEASUREMENT\nfind token : " + s);
        }
    }
    else
    {
        QString s1(lex->toString(token).c_str());
        QString s2(lex->getLexem().c_str());
        this->showError("expected end MEASUREMENT\nfind : " + s1 + " " + s2);
    }
}

MEASUREMENT::~MEASUREMENT()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

void MEASUREMENT::parseFixPar(QList<TokenTyp> *typePar, QTextStream &in)
{
    //Mandatory PARAMETERS
    TokenTyp token;
    //parameters = new QMap<const char*, const char*>;
    for (int i = 0; i < typePar->count(); i++)
    {
        token = this->nextToken(in);
        if (token == typePar->at(i))
        {
            //parameters.append(lex->getLexem());
            char *c = new char[lex->getLexem().length()+1];
            strcpy(c, lex->getLexem().c_str());
            parameters.append(c);
        }
        else if(typePar->at(i) == Float && token == Integer)
        {
            char *c = new char[parentNode->lex->getLexem().length()+1];
            strcpy(c, parentNode->lex->getLexem().c_str());
            parameters.append(c);
        }
        else
        {
            QString t(lex->toString(typePar->at(i)).c_str());
            QString s(lex->toString(token).c_str());
            this->showError("expected token : " + t +"\nfind token : " + s);
        }
    }
}

TokenTyp MEASUREMENT::parseOptPar(QTextStream &in)
{
    //opt Parameters
    QMap<std::string, Occurence> nameOptPar;
    nameOptPar.insert("FORMAT", ZeroOrOne);
    nameOptPar.insert("ECU_ADDRESS", ZeroOrOne);
    nameOptPar.insert("BIT_MASK", ZeroOrOne);
    nameOptPar.insert("BYTE_ORDER", ZeroOrOne);
    nameOptPar.insert("IF_DATA", ZeroOrMore);
    nameOptPar.insert("ARRAY_SIZE", ZeroOrOne);

    if (nameOptPar.isEmpty())
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
                    if (nameOptPar.contains(lexem))
                    {
                        if (nameOptPar.value(lexem) == ZeroOrOne)
                        {
                            nameOptPar.insert(lexem, Zero);
                            Node  *instance = factoryOptNode->value(lexem)->createInstance(in, this);
                            this->addChildNode(instance);
                            token = nextToken(in);
                        }
                        else if (nameOptPar.value(lexem) == ZeroOrMore)
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
                        this->showError("unknown Keyword : " + s);
                        return token;
                    }
                }
                else
                {
                    QString s(lex->toString(token).c_str());
                    this->showError("expected token : BlockBegin or Keyword\nfind token : " + s);
                    return token;
                }
            }
            //Items
            else if (token == Keyword)
            {
                std::string lexem = lex->getLexem();
                if (nameOptPar.contains(lexem))
                {
                    if (nameOptPar.value(lexem) == ZeroOrOne)
                    {
                        nameOptPar.insert(lexem, Zero);
                        Item  *instance = factoryOptItem->value(lexem)->createInstance(in, this);
                        this->addOptItem(instance);
                        token = nextToken(in);
                    }
                    else if (nameOptPar.value(lexem) == ZeroOrMore)
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
                    this->showError("unknown Keyword : " + s);
                    return token;
                }
            }
        }
        return token;
    }
}

QMap<std::string, std::string> *MEASUREMENT::getParameters()
{
    QMap<std::string, std::string> *par = new QMap<std::string, std::string>;
    for (int i = 0; i < namePar->count(); i++)
    {
        par->insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

std::string MEASUREMENT::pixmap()
{
    return ":/icones/MEAS.bmp";
}

char* MEASUREMENT::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}
