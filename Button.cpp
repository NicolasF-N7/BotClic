#include "Button.hpp"

void Button::create(UINT style, HWND hwnd, HINSTANCE instance, UINT callBackID, int x, int y, int width, int height)
{
    /*  style -> pour pouvoir rajouter BS_ICON ou BS_BITMAP */
    m_hwndButton = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_MULTILINE | BS_PUSHBUTTON | style,
                                x, y, width, height, hwnd, (HMENU)callBackID, instance, NULL);
}

LRESULT Button::setImage(WPARAM imgType, LPARAM hImage)
{
    return SendMessage(m_hwndButton, BM_SETIMAGE, imgType, hImage);

}

LRESULT Button::setIco(LPARAM hImage)
{
    return SendMessage(m_hwndButton, WM_SETICON, ICON_BIG, hImage);
}

void Button::setText(LPCTSTR text)
{
    SetWindowText(m_hwndButton, text);
}

LRESULT Button::setFont(HFONT font)
{
    // lParam -> true pour redessiner le bouton, pour que la nouvelle police s'applique
    return SendMessage(m_hwndButton, WM_SETFONT,(WPARAM)font, MAKELPARAM(true, 0));// Modifie la police
}

void Button::setFocus(bool focus)
{
    if(focus)
        SendMessage(m_hwndButton, WM_SETFOCUS, NULL, 0);
    else
        SendMessage(m_hwndButton, WM_KILLFOCUS, NULL, 0);
}

bool Button::move(int x, int y, int width, int height)
{
    return MoveWindow(m_hwndButton, x, y, width, height, true);
}

LRESULT Button::getCheck()
{
    return SendMessage(m_hwndButton, BM_GETCHECK, 0, 0);
}

bool Button::enable(bool enable)
{
    return EnableWindow(m_hwndButton, enable);
}

LRESULT Button::clic()
{
    return SendMessage(m_hwndButton, BM_CLICK, 0, 0);
}

void Button::show()
{
    ShowWindow(m_hwndButton,SW_SHOW);
}

void Button::hide()
{
    ShowWindow(m_hwndButton,SW_HIDE);
}

HWND Button::getHWNDButton()
{
    return m_hwndButton;
}
