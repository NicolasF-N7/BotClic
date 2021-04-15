#include "File.hpp"

using namespace std;


bool File::open(const char* fileName)
{
    m_flux.open(fileName, ios::in | ios::out | ios::ate);// par défaut ios::in | ios::out
    if(!m_flux)
    {
        cerr << "Impossible d'ouvrir le fichier choisis. Fonc File::open()" << endl;
        return false;
    }
    return true;
}

bool File::open(const char* fileName, std::ios_base::openmode mode)
{
    m_flux.open(fileName, mode);
    if(!m_flux)
    {
        cerr << "Impossible d'ouvrir le fichier choisis. Fonc File::open()" << endl;
        return false;
    }
    return true;
}

void File::write(int16_t &posX, int16_t &posY, unsigned int &command, int16_t &time)
{
    /*  On se positionne a la fin pour écrire   */
    m_flux.seekg(0, ios::end);
    if(m_flux.is_open())
        m_flux << posX << " " << posY << " " << command << " " << time << endl;
}

void File::write(Sequence *sequence)
{
    if(m_flux.is_open())
    {
        //  Si la séquence n'est pas vide
        if(sequence->size() > 0)
        {
            /*Première ligne hors de la boucle pour ne pas avoir un retour à la ligne de trop  */
            m_flux << sequence->getX(0) << " " << sequence->getY(0) << " " << sequence->getCommand(0) << " " << sequence->getTime(0);
            /*  i = 1 car i(0) a déjà été écrit */
            for(int i(1); i < sequence->size(); i++)
            {
                m_flux << endl << sequence->getX(i) << " " << sequence->getY(i) << " " << sequence->getCommand(i) << " " << sequence->getTime(i);
            }
        }
    }
}

void File::read(int16_t &posX, int16_t &posY, unsigned int &command, int16_t &time, int16_t &line)
{
    if(m_flux.is_open())
    {
        /*  On se positionne au début du fichier    */
        m_flux.seekg(0, ios::beg);
        char buffer;
        int nbrReadLine(0);
        /*  On lit le fichier jusqu'à trouver la ligne que l'on cherche */
        while(m_flux.get(buffer))
        {
            if(buffer == '\n')
            {
                /*  On est a la position désirée    */
                if(nbrReadLine == line)
                    m_flux >> posX >> posY >> command >> time;
                nbrReadLine++;
            }
        }
    }
}

void File::read(Sequence *sequence)
{
    if(m_flux.is_open())
    {
        /*  On se place a la fin pour récuperer la position */
        m_flux.seekg(0, ios::end);
        int fileSize = m_flux.tellg();

        int x(0);
        int y(0);
        uint16_t command;
        uint32_t time(0);

        sequence->clear();
        /*  On se positionne au début du fichier pour lire  */
        m_flux.seekg(0, m_flux.beg);
        //  Tant qu'il n'y a pas eofbit, failbit ou badbit on lit
        while(m_flux.good())
        {
            m_flux >> x >> y >> command >> time;
            sequence->addAction(x, y, command, time);
        }
    }
}

bool File::isEmpty()
{
    if(m_flux.is_open())
    {
        char c('\0');
        m_flux.seekg(0, ios::beg);
        /*  On récupère le premier caractère    */
        m_flux >> c;
        /*  Si c n'a pas changé, c'est qu'on a rien lu donc le fichier est vide */
        return (c == '\0');
    }
    return false;
}

int File::getNbrLine()
{
    if(m_flux.is_open())
    {
        m_flux.seekg(0, ios::beg);
        char buffer;
        int nbrLine(0);
        while(m_flux.get(buffer))
        {
            if(buffer == '\n')
                nbrLine++;
        }
        return nbrLine;
    }
    /*  Si on est ici c'est que le fichier n'est pas ouvert */
    return -1;
}

void File::close()
{
    m_flux.close();
}


