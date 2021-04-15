#ifndef SPINBOX_HPP_INCLUDED
#define SPINBOX_HPP_INCLUDED

#include <iostream>
#include <sstream>
#define _WIN32_IE 0x401
#include <windows.h>
#include <commctrl.h>

class SpinBox
{
public:
    SpinBox();
    /**     ATTENTION : Si heightEdit < 24 la bordure aura des problemes de d'affichage     **/
    void create(int charLimit, HWND hwnd, HINSTANCE instance,
                int xPos, int yPos, int widthEdit, int heightEdit, int widthUpDown, int heightUpDown, UINT editId);
    bool move(int x, int y, int width, int height, int widthUpDown, int heightUpDown);

    void show();
    void hide();
    bool enable(bool enable);

    void setPos(int nbr);
    void setRange(int minRange, int maxRange);  /*  L'intervalle par deffaut est 0, UD_MAXVAL   */
    LRESULT setFont(HFONT font);
    WNDPROC setWndProc(LONG_PTR newProc);

    int getPos();
    HWND getHWNDUpDown();
    HWND getHWNDEdit();
    //  Operator [] ?
private:
    HWND m_upDown;
    HWND m_edit;
    HINSTANCE m_instance;

    UDACCEL m_accel[6];
};

#endif // SPINBOX_HPP_INCLUDED
