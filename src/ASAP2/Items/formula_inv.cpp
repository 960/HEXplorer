#include "formula_inv.h"
#include <QMessageBox>
#include "a2lgrammar.h"

//initialise static variables
Factory<Item,FORMULA_INV> FORMULA_INV::itemFactory;

FORMULA_INV::FORMULA_INV(QTextStream &in, Node *parentNode)  : Item(parentNode)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->formula_inv.namePar;
    typePar = &gram->formula_inv.typePar;

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar ,in);
    if (parameters.count() > 0)
        name = parameters.at(0);
    else
        name = (char*)"formula_inv";
}

FORMULA_INV::~FORMULA_INV()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

void FORMULA_INV::parseFixPar(QList<TokenTyp> *typePar,  QTextStream &in)
{
    //Mandatory PARAMETERS
    TokenTyp token;
    for (int i = 0; i < typePar->count(); i++)
    {
        token = this->nextToken(in);
        if (token == typePar->at(i))
        {
            char *c = new char[parentNode->lex->getLexem().length()+1];
            strcpy(c, parentNode->lex->getLexem().c_str());
            parameters.append(c);
        }
        else
        {
            QString t(this->parentNode->lex->toString(typePar->at(i)).c_str());
            QString s(this->parentNode->lex->toString(token).c_str());
            this->showError("expected token : " + t +"\nfind token : " + s);
        }
    }
}

QMap<std::string, std::string> FORMULA_INV::getParameters()
{
    QMap<std::string, std::string> par;
    for (int i = 0; i < namePar->count(); i++)
    {
        par.insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char* FORMULA_INV::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}
