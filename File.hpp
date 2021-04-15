#ifndef SAVEFILE_HPP_INCLUDED
#define SAVEFILE_HPP_INCLUDED

#include <iostream>
#include <windows.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include "Sequence.hpp"

class File
{
public:
    bool open(const char* fileName);
    bool open(const char* fileName, std::ios_base::openmode mode);
    void write(int16_t &posX, int16_t &posY, unsigned int &command, int16_t &time);
    void write(Sequence *sequence);
    /*  line -> zero based line to read    */
    void read(int16_t &posX, int16_t &posY, unsigned int &command, int16_t &time, int16_t &line);
    void read(Sequence *sequence);
    bool isEmpty();
    int getNbrLine();
    void close();

private:
    std::fstream m_flux;
    HANDLE m_hFile;

};

#endif // SAVEFILE_HPP_INCLUDED
