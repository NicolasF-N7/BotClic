#include "Sequence.hpp"
#include <iostream>

using namespace std;

void Sequence::addAction(int16_t posX, int16_t posY, uint16_t command, uint32_t time)
{
    m_x.push_back(posX);
    m_y.push_back(posY);
    m_command.push_back(command);
    m_time.push_back(time);

    /*  On verifie si les tab sont de la même taille avec size()    */
    this->size();
}

void Sequence::addAction(int16_t posX, int16_t posY, uint16_t command, uint32_t time, int index)
{
    /*  Si l'index n'est pas compris entre 0 et this->size() il n'est pas valide    */
    if(index < 0 || index > this->size())
        return;

    m_x.insert(m_x.begin()+index, posX);
    m_y.insert(m_y.begin()+index, posY);
    m_command.insert(m_command.begin()+index, command);
    m_time.insert(m_time.begin()+index, time);

    /*  On verifie si les tab sont de la même taille avec size()    */
    this->size();
}

void Sequence::deleteAction(int i)
{
    /*  Si l'index n'est pas compris entre 0 et this->size() il n'est pas valide    */
    if(i <= this->size() && i >= 0)
    {
        m_x.erase(m_x.begin()+i);
        m_y.erase(m_y.begin()+i);
        m_command.erase(m_command.begin()+i);
        m_time.erase(m_time.begin()+i);
    }
}

void Sequence::setName(const char* newName)
{
    m_seqName = newName;
}

/**     GETTER      **/
std::string Sequence::getName()
{
    return m_seqName;
}

int16_t Sequence::getX(int i)
{
    /*  Si l'index n'est pas compris entre 0 et this->size() il n'est pas valide    */
    if(i <= this->size() && i >= 0)
        return m_x[i];
    else
        return NULL;
}

int16_t Sequence::getY(int i)
{
    /*  Si l'index n'est pas compris entre 0 et this->size() il n'est pas valide    */
    if(i <= this->size() && i >= 0)
        return m_y[i];
    else
        return NULL;
}

uint16_t Sequence::getCommand(int i)
{
    /*  Si l'index n'est pas compris entre 0 et this->size() il n'est pas valide    */
    if(i <= this->size() && i >= 0)
        return m_command[i];//    +1 Pour compenser le décalage des valeurs BC_RIGHT_CLIC...
    else
        return 0;
}

uint32_t Sequence::getTime(int i)
{
    /*  Si l'index n'est pas compris entre 0 et this->size() il n'est pas valide    */
    if(i <= this->size() && i >= 0)
        return m_time[i];
    else
        return NULL;
}

void Sequence::clear()
{
    m_x.clear();
    m_y.clear();
    m_command.clear();
    m_time.clear();
}

bool Sequence::isEmpty()
{
    /*  Si la taille de this est inferieur ou égale a 0 la sequence est vide    */
    return (this->size() <= 0);
}

int Sequence::size()
{
    /*  Si l'addition de la taille de tout les tab est différente de la taille d'un tab *4 il y a une erreur */
    if(m_x.size() * 4 != m_x.size() + m_y.size() + m_command.size() + m_time.size())
    {
        cerr << "Erreur : Tableau de taille differente (Fonction size)" << endl;
        return -1;
    }
    else
        return m_x.size();
}




