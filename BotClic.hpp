#ifndef BOTCLIC_HPP_INCLUDED
#define BOTCLIC_HPP_INCLUDED

#define _WIN32_IE 0x0501

#include <windows.h>
#include <commctrl.h>
#include <iostream>
#include "Command.hpp"
#include "Sequence.hpp"
#include "Layout.hpp"

class BotClic
{
public:
    /*      WM_CREATE       */
    void createStatusBar(HWND hwnd, int nbrParts, UINT windowWidth, UINT callbackID, HFONT font);

    /*      A mettre dans WM_SIZE       */
    void statusBarProc(LPARAM lParam);

    void setStatusBarText(int part, const char* text);
    void getStatusBarText(int part, char* text);
    //  CHANGER ICONE FICHIERS .BCSeq DANS OPTIONS
private:
    HWND m_statusBar;

};


#endif // BOTCLIC_HPP_INCLUDED
