#ifndef COMBOBOX_HPP_INCLUDED
#define COMBOBOX_HPP_INCLUDED

#define _WIN32_IE 0x501

#include <iostream>
#include <windows.h>
#include <commctrl.h>

class ComboBox
{
    public:
        void create(HWND hwnd, HINSTANCE instance, int xPos, int yPos, int width, int height, UINT IDCallBack);
        LRESULT addLine(const char* text);
        LRESULT clear();
        bool move(int x, int y, int width, int height);
        void show();
        void hide();
        bool enable(bool enable);
        LRESULT setPosition(UINT16 pos);
        LRESULT setFont(HFONT font);
        LRESULT getPosition();
        LRESULT sendMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        //  Operator [] ?
    private:
        HWND m_hwndComboBox;


};

#endif // COMBOBOX_HPP
