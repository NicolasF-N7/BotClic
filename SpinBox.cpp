#include "SpinBox.hpp"

SpinBox::SpinBox()
{
    m_accel[0].nSec = 0;
    m_accel[0].nInc = 10;

    m_accel[1].nSec = 1;
    m_accel[1].nInc = 100;

    m_accel[2].nSec = 2;
    m_accel[2].nInc = 1000;

    m_accel[3].nSec = 3;
    m_accel[3].nInc = 10000;

    m_accel[4].nSec = 5;
    m_accel[4].nInc = 100000;

    m_accel[5].nSec = 7;
    m_accel[5].nInc = 1000000;
}

void SpinBox::create(int charLimit, HWND hwnd, HINSTANCE instance,
                     int xPos, int yPos, int widthEdit, int heightEdit, int widthUpDown, int heightUpDown, UINT editId)
{
    /*INITCOMMONCONTROLSEX iccs;
    iccs.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccs.dwICC  = ICC_UPDOWN_CLASS;
    InitCommonControlsEx(&iccs);*/

    m_upDown = CreateWindow(UPDOWN_CLASS, 0, WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT,
                            xPos + widthEdit, yPos - (heightUpDown/2) + (heightEdit/2), widthUpDown, heightUpDown,
                            hwnd, (HMENU)0, instance, NULL);

    SendMessage(m_upDown, UDM_SETRANGE32, 0, UD_MAXVAL);
    SendMessage(m_upDown, UDM_SETPOS, 0, 0);
    SendMessage(m_upDown, UDM_SETACCEL , 6, (LPARAM)&m_accel);

    m_edit = CreateWindow("EDIT", "0", WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_BORDER,
                         xPos, yPos, widthEdit, heightEdit,
                         hwnd, (HMENU)editId, instance, NULL);

    SendMessage(m_upDown, UDM_SETBUDDY, (WPARAM)m_edit, 0);
    SendMessage(m_edit, EM_SETLIMITTEXT , charLimit, 0);
}




bool SpinBox::move(int x, int y, int width, int height, int widthUpDown, int heightUpDown)
{
    //  Si un des deux renvois false, SpinBox::move renvois false
    return std::min(MoveWindow(m_edit, x, y, width, height, true), MoveWindow(m_upDown, (x+width), (y-(heightUpDown/2)+(height/2)), widthUpDown, heightUpDown, true));
}

void SpinBox::show()
{
    ShowWindow(m_upDown,SW_SHOW);
    ShowWindow(m_edit,SW_SHOW);
}

void SpinBox::hide()
{
    ShowWindow(m_upDown,SW_HIDE);
    ShowWindow(m_edit,SW_HIDE);
}

bool SpinBox::enable(bool enable)
{
    EnableWindow(m_upDown, enable);
    return EnableWindow(m_edit, enable);
}
/**     SETTER      **/
void SpinBox::setPos(int pos)
{
    SendMessage(m_upDown, UDM_SETPOS32, 0, pos);
}

void SpinBox::setRange(int minRange, int maxRange)
{
    /*                                    UD_MINVAL UD_MAXVAL   */
    SendMessage(m_upDown, UDM_SETRANGE32, minRange, maxRange);
}

LRESULT SpinBox::setFont(HFONT font)
{
    return SendMessage(m_edit, WM_SETFONT,(WPARAM)font, MAKELPARAM(true, 0));// Modifie la police
}

WNDPROC SpinBox::setWndProc(LONG_PTR newProc)
{
    return (WNDPROC)SetWindowLong(m_edit, GWL_WNDPROC, newProc);
}
/**     GETTER      **/
int SpinBox::getPos()
{
    return SendMessage(m_upDown, UDM_GETPOS32, 0, 0);
}

HWND SpinBox::getHWNDUpDown()
{
    return m_upDown;
}

HWND SpinBox::getHWNDEdit()
{
    return m_edit;
}
