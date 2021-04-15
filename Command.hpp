#ifndef CONTROLS_HPP_INCLUDED
#define CONTROLS_HPP_INCLUDED

#include <windows.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include "textRc.hpp"

#define NBR_COMMAND         13

#define BC_MOUSE_MOVE       1
#define BC_RIGHT_CLIC       2
#define BC_LEFT_CLIC        3
#define BC_COPY             4
#define BC_PASTE            5
#define BC_SELECT_ALL       6
#define BC_CONTROL_TAB      7
#define BC_ENTER            8
#define BC_LEFT_CLIC_DOWN   9
#define BC_LEFT_CLIC_UP     10
#define BC_RIGHT_CLIC_DOWN  11
#define BC_RIGHT_CLIC_UP    12
#define BC_DELETE           13

class Command
{
public:
    Command();
    void init(bool writeFile);
    void setCurPos(int posX, int posY);
    int getCurPosX();
    int getCurPosY();
    char* getDescByCommand(unsigned int command);
    int getKeyByCommand(unsigned int command);
    void doCommand(unsigned int command);
    bool editKey(unsigned int command, uint8_t key, bool writeFile);

protected:
    void initDescTab();
    void initKeyTab(bool writeFile);

private:
    char* descriptionTab[NBR_COMMAND];
    uint8_t keyTab[NBR_COMMAND];

};

#endif // CONTROLS_HPP_INCLUDED
