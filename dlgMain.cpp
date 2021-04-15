#include <iostream>
#include "DlgResource.hpp"

std::string* charParam;

BOOL CALLBACK DlgSeqName(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            if(lParam != 0)
            {
                //  On récupère le string passé en paramèttres
                charParam = (std::string*)lParam;

                HWND hEdit = GetDlgItem(hwndDlg, ID_EDIT);
                //std:cout << "Setting text : " << charParam << std::endl;
                SetWindowText(hEdit, (LPSTR)charParam->c_str());
            }
            else
                std::cout << "Give a lParam to dlg" << std::endl;

            //  On récupère le hWnd du texte pour changer la police
            HWND hText = GetDlgItem(hwndDlg, ID_TEXT);
            HFONT hTextFont = CreateFont(18, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Calibri");
            SendMessage(hText, WM_SETFONT, (WPARAM)hTextFont, 0);
        }
        return TRUE;

        case WM_CLOSE:
        {
            EndDialog(hwndDlg, false);
        }
        return TRUE;

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case ID_OK:
                {
                    //  On récupère le hWnd de l'editbox
                    HWND hEdit = GetDlgItem(hwndDlg, ID_EDIT);
                    int fileNameLen = GetWindowTextLength(hEdit);// On récupère la longueur du texte dans l'editBox
                    //  On récupère le texte si il y en a un
                    if(fileNameLen > 0)
                    {
                        char fileName[fileNameLen+1];
                        //  On récupère le texte de l'edit box
                        GetWindowText(hEdit, fileName, fileNameLen+1);
                        //  On modifie le texte avec le pointeur passé en param
                        *charParam = fileName;
                        EndDialog(hwndDlg, (INT_PTR)true);
                    }
                    else
                        MessageBox(hwndDlg, "Veuillez saisir un nom pour votre séquence", "Trop court", MB_OK);

                    break;
                }

                case ID_CANCEL:
                {
                    EndDialog(hwndDlg, false);
                }
            }
        }
        return TRUE;
    }
    return FALSE;
}
