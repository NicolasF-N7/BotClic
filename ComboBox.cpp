#include "ComboBox.hpp"

void ComboBox::create(HWND hwnd, HINSTANCE instance, int xPos, int yPos, int width, int height, UINT IDCallBack)
{
    /*INITCOMMONCONTROLSEX iccs;
    iccs.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccs.dwICC  = ICC_PROGRESS_CLASS;
    InitCommonControlsEx(&iccs);*/

    /*m_hwndComboBox = CreateWindow(WC_COMBOBOX, "",
                              CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
                              xPos, yPos, width, height, hwnd, NULL, instance, NULL);*/
    m_hwndComboBox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, TEXT(""),
                                 CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
                                 xPos, yPos, width, height, hwnd, (HMENU)IDCallBack, instance,
                                 NULL);
    if(!m_hwndComboBox)
        {
            MessageBox(hwnd, "Could not create the combo box", "Failed Control Creation", MB_OK);
        }
}

LRESULT ComboBox::addLine(const char* text)
{
    return SendMessage(m_hwndComboBox, CB_ADDSTRING, 0, LPARAM(text));
}

LRESULT ComboBox::clear()
{
    return SendMessage(m_hwndComboBox, CB_RESETCONTENT, 0, 0);
}

bool ComboBox::move(int x, int y, int width, int height)
{
    return MoveWindow(m_hwndComboBox, x, y, width, height, true);
}

void ComboBox::show()
{
    ShowWindow(m_hwndComboBox,SW_SHOW);
}

void ComboBox::hide()
{
    ShowWindow(m_hwndComboBox,SW_HIDE);
}

bool ComboBox::enable(bool enable)
{
    return EnableWindow(m_hwndComboBox, enable);
}

LRESULT ComboBox::setPosition(UINT16 pos)
{
    return SendMessage(m_hwndComboBox, CB_SETCURSEL, pos, 0);
}

LRESULT ComboBox::setFont(HFONT font)
{
    return SendMessage(m_hwndComboBox, WM_SETFONT,(WPARAM)font, MAKELPARAM(true, 0));// Modifie la police
}


LRESULT ComboBox::getPosition()
{
    return SendMessage(m_hwndComboBox, CB_GETCURSEL, 0, 0);
}

 LRESULT ComboBox::sendMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
 {
     return SendMessage(hwnd, msg, wParam, lParam);
 }
