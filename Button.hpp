#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <windows.h>

class Button
{
    public:
        /*               hwnd -> fenetre parent */
        /*                                              callBackID -> LOWORD(wParam) du message WM_COMMAND  */
        void create(UINT style, HWND hwnd, HINSTANCE instance, UINT callBackID, int x, int y, int width, int height);    /*  Méthode a mettre dans le WM_CREATE  */

        /*                   imgType -> IMAGE_ICON ou IMAGE_BITMAP */
        /*                                   hImage -> (LPARAM) HICON ou HBITMAP    */
        LRESULT setImage(WPARAM imgType, LPARAM hImage);
        LRESULT setIco(LPARAM hImage);
        void setText(LPCTSTR text);     /*  Changer le texte du bouton  */
        LRESULT setFont(HFONT font);
        void setFocus(bool focus);
        bool move(int x, int y, int width, int height);

        LRESULT getCheck();
        bool enable(bool enable);       /*  Si false, le bouton ne pourra pas être appuyé   */
        LRESULT clic();
        void show();
        void hide();
        HWND getHWNDButton();

    private:
        HWND m_hwndButton;

};

#endif // BUTTON_HPP
