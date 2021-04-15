#include "BotClic.hpp"

using namespace std;

void BotClic::createStatusBar(HWND hwnd, int nbrParts, UINT windowWidth, UINT callbackID, HFONT font)
{
    /*  Le nombre de pixels pour chaques partis */
    int pixelsByPart(windowWidth / nbrParts);
    int statusBarArray[nbrParts];
    for(int i(0); i < nbrParts; i++)
    {
        statusBarArray[i] = pixelsByPart * (i+1);
    }
    /*  Equivalent a CreateWindow, pour les statusWindow  */
    m_statusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | SBT_TOOLTIPS, "",
                                   hwnd, callbackID);
    /*  Affectation du nombre de parties    */
    SendMessage(m_statusBar, SB_SETPARTS , nbrParts, (LPARAM)statusBarArray);
    SendMessage(m_statusBar, WM_SETFONT,(WPARAM)font,0);// Modifie la police
}

void BotClic::statusBarProc(LPARAM lParam)
{
    RECT sbRect;
    UINT sbheight;
    GetWindowRect(m_statusBar, &sbRect);
    sbheight = sbRect.bottom - sbRect.top;
    MoveWindow(m_statusBar, 0, HIWORD(lParam)-sbheight, LOWORD(lParam), sbheight, TRUE);
}

void BotClic::setStatusBarText(int part, const char* text)
{
    if(part <= SendMessage(m_statusBar, SB_GETPARTS , 0, 0))//  on vérifie que la partie existe
        SendMessage(m_statusBar, SB_SETTEXT, MAKEWPARAM(part, 0), (LPARAM)text);
    else
    {
        #ifdef DEBUG
            std::cout << "Partie inexistante" << std::endl;
        #endif // DEBUG
    }
}

void BotClic::getStatusBarText(int part, char* text)
{
    if(part <= SendMessage(m_statusBar, SB_GETPARTS , 0, 0))//  on vérifie que la partie existe
        SendMessage(m_statusBar, SB_GETTEXT, (WPARAM)part, (LPARAM)text);
    else
    {
        #ifdef DEBUG
            std::cout << "Partie inexistante" << std::endl;
        #endif // DEBUG
    }
}


