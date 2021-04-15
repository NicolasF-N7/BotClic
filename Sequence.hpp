#ifndef SEQUENCE_HPP_INCLUDED
#define SEQUENCE_HPP_INCLUDED

#include <iostream>
#include <vector>
#include <string>

class Sequence
{
public:
    /*  Fonction sans index -> ajoute l'action a la fin, sinon a la position renseignée */
    void addAction(int16_t posX, int16_t posY, uint16_t command, uint32_t time);
    void addAction(int16_t posX, int16_t posY, uint16_t command, uint32_t time, int index);
    void deleteAction(int i);   /*  Permet de supprimer la ième action  */
    void setName(const char* newName);
    /**     GETTER      **/
    std::string getName();
    int16_t getX(int i);
    int16_t getY(int i);
    uint16_t getCommand(int i);
    uint32_t getTime(int i);

    void clear();
    bool isEmpty();
    int size();

private:
    std::string m_seqName;
    std::vector<int16_t> m_x;
    std::vector<int16_t> m_y;
    std::vector<uint16_t> m_command;//  unsigned int16_t ?
    std::vector<uint32_t> m_time;//     DWORD = A 32-bit unsigned integer. The range is 0 through 4294967295 decimal
};

#endif // SEQUENCE_HPP_INCLUDED
