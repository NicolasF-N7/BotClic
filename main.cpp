#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif
///     \%1  en arg Signifie que le chemin du fichier sera passé en argument de l'application
#define _WIN32_IE 0x501
#define _WIN32_WINNT 0x501
#define WINVER 0x0502  // Essential to use SetLayeredWindowAttributes(); funct

#define PBS_MARQUEE  0x08
#define PBM_SETMARQUEE WM_USER + 10
#define ODS_HOTLIGHT     0x0040 /* Highlighted when under mouse */
#define ODS_INACTIVE    0x0080

#include <iostream>
#include <string>// getline
#include <windows.h>
#include <commctrl.h>
#include <Uxtheme.h> /*  Build options -> linkers settings -> add -> UxTheme */
#include <tchar.h>
#include <accctrl.h>
#include <Aclapi.h>
#include <sddl2.h>
/*#include <dwmapi.h>*/     /** Enlever Cet include Si inutilisé    Linkers settings -> Dwmapi **/
#include"BotClic.hpp"
#include"Button.hpp"
#include"ComboBox.hpp"
#include"Command.hpp"
#include"File.hpp"
#include"Sequence.hpp"
#include"SpinBox.hpp"
#include"Layout.hpp"
#include"textRc.hpp"
#include"DlgResource.hpp"
#include"dlgMain.cpp"
#include"Resources.hpp"

#define DEBUG

#define APPEND_ACTION -1

#define SDDL_REVISION_1     1

/**                 ID          **/
/*  IDs Globaux */
#define ID_MAIN_TAB 500
#define ID_STATUS_BAR 501
#define ID_BTN_START_SEQ 502
#define ID_BTN_SIMULATE_SEQ 503
#define ID_PROGRESS_BAR 504

/*  Ids des menus   */
#define ID_MENU_CUR_POS             400
#define ID_MENU_DEFAULT_FONT        401
#define ID_MENU_PBAR_VISUAL_STYLE   402
#define ID_MENU_HOT_KEY_SHOW_WINDOW 403
#define ID_MENU_HOT_KEY_START_SEQ   404
#define ID_MENU_HOT_KEY_CREATE_SEQ  405
#define ID_MENU_SET_SAME_TIME       406
#define ID_MENU_BRING_BACK_CURSOR   407

/*  Onglet Créer   51x */
#define ID_BTN_NEW_SEQ 510
#define ID_TEXT_CREATION_SEQ 511
#define ID_LIST_BOX_KEY 512

/*  Onglet Charger    52x*/
#define ID_BTN_LOAD_FILE 520
#define ID_BTN_SAVE_FILE 521
#define ID_LIST_BOX_FILES 522
#define ID_BTN_DELETE_FILE 523

/*  Onglet Modifier    53x*/
#define ID_LIST_BOX_ACTIONS 530
#define ID_BTN_NEW_ACTION 531
#define ID_BTN_DELETE_ACTION 532
#define ID_HWND_EDIT_ACTION 534//  Le même ID pour EditX editY et editTime

/*  Onglet paramètres    54x*/
#define ID_BTN_EDIT_KEY 541
#define ID_TEXT_EDIT_KEY 542
/// ////////////////////////////////////////////////////////////////// ///
#define MASK 32768  /*  = (2^16)/2  */
#define NBR_KEY_LISTENING 8
#define SLEEP_TIME_60FPS 17
#define SLEEP_TIME_SIMULATION_SEQ 1000
#define EDIT_KEY_TIMEOUT 10000  //  10 sec

#define VK_HOT_KEY_SHOW_WINDOW  0x57 //  W
#define VK_HOT_KEY_START_SEQ    0x4C //  L
#define VK_HOT_KEY_CREATE_SEQ   0x4E //  N

#define VK_END_CREATING_SEQ     0x46 //  F

#define ID_HOT_KEY_SHOW_WINDOW      1
#define ID_HOT_KEY_START_SEQ        2
#define ID_HOT_KEY_CREATE_SEQ       3

#define ID_CALLBACK_TRAY_ICON 987

#define SEQUENCE_NOT_RUNNING    140
#define SEQUENCE_RUNNING        141
#define SEQUENCE_PAUSED         142

#define STATUS_BAR_BOT_CLIC_VERSION_PART    0
#define STATUS_BAR_CUR_POS_PART             1
#define STATUS_BAR_SEQ_NAME_PART            2

/*  Declare Windows procedure  */
bool isBCSeq(const char* fileName);
std::string fileNameByPath(const char* filePath);
void pbBarColor(HWND hwndParent);
bool CreateWritableDirectory(const char* directory);
void readConfig();
void writeConfig();
void createMenu(HWND hwnd);
void createTrayIcon();
void trayIconProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
HBRUSH CreateGradientBrush(COLORREF top, COLORREF bottom, LPDRAWITEMSTRUCT item);
void cursorListener();
void keyboardListener();
void newActionBtnThreadProc();
void editKeyProc();
void addAction(int xCursor, int yCursor, unsigned int command, unsigned int time, int index);
void clearSeq();
void deleteAction(int index);
void loadSeqFromFolder();
void loadSeqFromFile(std::string* ptrSeqName);
void saveSeq(std::string* seqFilePath);
void deleteFile(std::string* filePath);
void runSeq(Sequence* seq);
void simulateSeq(Sequence* seq);
void fillKeyComboBox(int pos);
void fillKeyListBox();
void fillFileListBox();
void setWindowFont(bool setFont);
LRESULT CALLBACK spinBoxesProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void listBoxesControl(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void menusControl(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void buttonsControl(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void initConfig();

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

HINSTANCE instance;
WNDPROC oldSpinBoxesProc;

HANDLE hRunSeqThread = CreateEvent(NULL, true, true, NULL);
HANDLE hSimulateSeqThread = CreateEvent(NULL, true, true, NULL);
HANDLE hPbBarColorThread;
HANDLE hKeyboardListenerThread;
HANDLE hNewActionBtnThread = CreateEvent(NULL, true, true, NULL);
HANDLE hEditActionMutex = CreateEvent(NULL, true, false, NULL);//   Non signalé au départ
HANDLE hCurPosThread;
HANDLE hLoadSeqThread;
HANDLE hSaveSeqThread;
HANDLE hDeleteFileThread;
HANDLE hEditKeyThread;
HANDLE hNewKeyCodeAvailable = CreateEvent(NULL, true, false, NULL);
HANDLE hRunSeqThreadMutex = CreateEvent(NULL, true, false, NULL);

uint8_t newKeyCode(0);//    pour edit key
//uint8_t vkEndCreaSeq = VK_END_CREATING_SEQ;
bool seqThreadPaused(false);
bool editKeyThreadRunning(false);
bool listeningKeyboard(false);
bool listenEditKey(false);
bool newActionBtnThreadRunning(false);
bool displayCurPosStatusBar(false);
bool displayCurPosSpinBox(false);
bool curPosThreadRunning(false);
HWND hTextCreationSeq;
HWND hTextEditKey;
HWND hListBoxActions;
HWND hListBoxFiles;
HWND hListBoxKey;
HWND hMainTab;
TCITEM tie;
HWND hCbCommand;
HWND hCbDisplayCurPos;
HWND hPBarSequence;
HWND hActionTime;
HMENU hMenuBar = CreateMenu();
HMENU hOptionsMenu = CreateMenu();

Button btnStartSeq;
Button btnNewAction;
Button btnDeleteAction;
Button btnSimulateSeq;
Button btnCreateSeq;
Button btnLoadFile;
Button btnSaveFile;
Button btnDeleteFile;
Button btnEditKey;
SpinBox sbActionPosX;
SpinBox sbActionPosY;
ComboBox cbCommand;
ComboBox cbCommandKey;

Command cmd;
BotClic botClic;
Sequence currSeq;

bool isLaunchedFromExe(true);

//  lpszArgument -> arguments passé en ligne de commande
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    if(strlen(lpszArgument) > 1)//  Si BotClic a été lancé avec des arguments
    {
        isLaunchedFromExe = false;
        #ifdef DEBUG
            std::cout << "Launched from sequence file " << isLaunchedFromExe << std::endl;
        #endif // DEBUG
    }
    else
    {
        isLaunchedFromExe = true;
        #ifdef DEBUG
            std::cout << "Launched from exe file " << isLaunchedFromExe << std::endl;
        #endif // DEBUG
    }
    cmd.init(isLaunchedFromExe);//   Si lancé depuis exe, alors on écrit keytab.BC

    HWND mainWindow;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = CLASS_NAME;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(2), IMAGE_ICON, 0, 0, 0 );
    wincl.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(2), IMAGE_ICON, 0, 0, 0 );
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);/* Use default icon and mouse-pointer */
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.style = 0;    /*  CS_DBLCLKS pour intercepter les doubles clics dans la window procedure  */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hbrBackground = (HBRUSH) (1 + COLOR_BTNFACE);/* Couleur de fond de la feneter */
    //wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND +2;/* NOIR */
    wincl.cbSize = sizeof(WNDCLASSEX);
    /* Register the window class, and if it fails quit the program */
    if(!RegisterClassEx (&wincl))
    {
        MessageBox(mainWindow, "BotClic n'a pas pu s'ouvrir :\nErreur lors de l'enregistrement de la classe\nCode d'erreur : " + GetLastError(),
                   "ERREUR", MB_OK | MB_ICONERROR);
        return 0;
    }

    /* The class is registered, let's create the program*/
    mainWindow = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           CLASS_NAME,         /* Classname */
           _T("BotClic"),       /* Title Text */
           WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION, /*environ égale à WS_OVERLAPPEDWINDOW sans WS_MAXIMIZEBOX*/
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           WINDOW_WIDTH,                 /* The programs width */
           WINDOW_HEIGHT,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
    if(!mainWindow)
    {
        SetForegroundWindow(mainWindow);
        MessageBox(mainWindow, "BotClic n'a pas pu s'ouvrir :\nErreur lors de la création de la fenêtre :\n" + GetLastError(), "ERREUR", MB_OK | MB_ICONERROR);
    }

    INITCOMMONCONTROLSEX iccs;
    iccs.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccs.dwICC  = ICC_PROGRESS_CLASS;
    InitCommonControlsEx(&iccs);

    instance = hThisInstance;

    /* Make the window visible on the screen */
    ShowWindow(mainWindow, SW_SHOW);

    /// chargement de la séquence en paramettres
    if(!isLaunchedFromExe)
    {
        std::string* seqName = new std::string;
        *seqName = lpszArgument;
        seqName->erase(0, 1);// On efface le premier char car -> "
        seqName->pop_back();// on efface le dernier -> "
        #ifdef DEBUG
            std::cout << "Sequence path : " << lpszArgument << std::endl;
        #endif // DEBUG

        hLoadSeqThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&loadSeqFromFile, (PVOID)seqName, 0, 0);
        if(hLoadSeqThread == NULL)
        {
            delete(seqName);//   On libère le string car il ne sera pas libéré dans le thread

            std::ostringstream codeError;
            codeError << "Impossible de lancer le thread de chargement de séquence\nCode d'erreur : ";
            codeError << GetLastError();
            MessageBox(0, codeError.str().c_str(), "Error", MB_OK);
        }
        else
        {
            #ifdef DEBUG
                std::cout << "loadSeqFromFile thread created" << std::endl;
            #endif // DEBUG
        }
    }

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage(&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

bool isBCSeq(const char* fileName)
{
    int fileLength = strlen(fileName);
    int totalPoint(0);

    /** Determine le nombre de points dans le nom du fichier **/
    for(int i(0); i < fileLength; i++)
    {
        if(fileName[i] == '.')
            totalPoint++;
    }

    /** On lit le nom du fichier jusqu'à trouver le dernier point   **/
    int readPoint(0);
    for(int i(0); i < fileLength; i++)
    {
        if(fileName[i] == '.')
            readPoint++;

        /* Compare de nombre de points total et le nombre de point lu   */
        if(readPoint == totalPoint)
        {
            if(fileName[i] == '.' and fileName[i+1] == 'B' and fileName[i+2] == 'C' and fileName[i+3] == 'S' and fileName[i+4] == 'e' and fileName[i+5] == 'q')
                return true;
            else
                return false;
        }
    }
    /*  Si on arrive ici il y a eu on problème, on renvoie false    */
    return false;
}

std::string fileNameByPath(const char* filePath)
{
    //  lastSlash est la dernière position où \ apparais dans le chemin du fichier
    char* lastSlash = strrchr(filePath, '\\');
    int lastSlashPos = lastSlash-filePath+1;
    std::string fileName;

    //  On copie le nom du fichier dans fileName. On le fait pour chaque caracères du nom de fichier : (strlen(filePath)-lastSlashPos)
    for(int i(0); i < (strlen(filePath)-lastSlashPos); i++)
    {
        fileName.push_back(filePath[i+lastSlashPos]);// lastSlashPos représente le premier caractère du nom de fichier
    }
    return fileName;
}

void pbBarColor(HWND hwndParent)
{
    /*  r = 255 pour amorcer/commencer   */
    uint8_t r(255), g(0), b(0);

    bool increaseR;
    bool increaseG;
    bool increaseB;

    while(1)
    {

        WINDOWPLACEMENT wndPcmt;
        wndPcmt.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(hwndParent, &wndPcmt);
        if(wndPcmt.showCmd == SW_SHOWMINIMIZED){//   Si le fenetre n'est pas affichée on saute tout ce qui gère la couleur de la PBAR
            Sleep(SLEEP_TIME_60FPS*4);//    On attend 4 fois plus pour économiser le processeur
            continue;
        }
        else
            Sleep(SLEEP_TIME_60FPS);

        /// Rouge   :   -\_/    -\_/-\_/-\_/-\_/-\_/-\_/-\_/-\_/-\_/...
        /// Vert    :   _/\_    _/\__/\__/\__/\__/\__/\__/\__/\__/\_...
        /// Bleu    :   __/\    __/\__/\__/\__/\__/\__/\__/\__/\__/\...

        if(r <= 0) {
            increaseG = false;
            increaseB = true;
        }
        if(g <= 0) {
            increaseR = true;
            increaseB = false;
        }
        if(b <= 0) {
            increaseR = false;
            increaseG = true;
        }

        /**if(r >= 255) {
            increaseR = false;
            increaseG = true;
        }
        if(g >= 255) {
             increaseG = false;     OPTIMISATION
             increaseB = true;
        }
        if(b >= 255) {
            increaseR = true;
            increaseB = false;
        }**/

    /** AFFECTATION DES INCREMENTATIONS **/
       if(increaseR == true && r < 255)
            r++;
        if(increaseR == false && r > 0)
            r--;

        if(increaseG == true && g < 255)
            g++;
        if(increaseG == false && g > 0)
            g--;

        if(increaseB == true && b < 255)
            b++;
        if(increaseB == false && b > 0)
            b--;

        /*std::cout << "R : " << (int)r << std::endl;
        std::cout << "G : " << (int)g << std::endl;
        std::cout << "B : " << (int)b << std::endl;
        system("cls");*/

        SendMessage(hPBarSequence, PBM_SETBKCOLOR , 0, (LPARAM) RGB (r, g, b));
        SendMessage(hPBarSequence, PBM_SETBARCOLOR , 0, (LPARAM) RGB (255-r, 255-g, 255-b));
    }
}

bool CreateWritableDirectory(const char* directory)
{
    /*SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
    PSID everyone_sid = NULL;
    AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &everyone_sid);

    EXPLICIT_ACCESS ea;
    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
    ea.grfAccessPermissions = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    ea.grfAccessMode = SET_ACCESS;
    ea.grfInheritance = NO_INHERITANCE;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea.Trustee.ptstrName  = (LPSTR)everyone_sid;

    PACL acl = NULL;
    SetEntriesInAcl(1, &ea, NULL, &acl);


    SECURITY_DESCRIPTOR sd;
    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, true, acl, false);


    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = false;
    sa.lpSecurityDescriptor = &sd;*/


    // Use the security attributes to set the security descriptor
    // when you create a key.

    /*SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = false;
    sa.lpSecurityDescriptor = ConvertStringSecurityDescriptorToSecurityDescriptorA("(D;OICI;GRGWGX;;;AU)", SDDL_REVISION_1, &sa.lpSecurityDescriptor, 0);   //"(D;OICI;GRGWGX;;;AU)"
*/
    //return CreateDirectory(directory, &sa);
    return CreateDirectory(directory, NULL);
}

void readConfig(HWND hwnd)
{
    if(CreateWritableDirectory(SOURCE_FOLDER_NAME) == ERROR_PATH_NOT_FOUND)
        std::cout << "ERROR_PATH_NOT_FOUND" << std::endl;

    std::string configFilePath;
    configFilePath += SOURCE_FOLDER_NAME;
    configFilePath += CONFIG_FILE_NAME;

    std::ifstream fluxIn(configFilePath.c_str());
    if(fluxIn.good())
    {
        UINT menuCurPosStat;
        UINT menuBringBackCursor;
        UINT menuDefaultFontStat;
        UINT menuPBarVisualStyleStat;
        UINT menuHotKeyShowWndStat;
        UINT menuStartSeqStat;
        UINT menuCreateSeqStat;
        std::string seqName;

        //  On récupère l'état des menus
        fluxIn >> menuCurPosStat;
        fluxIn >> menuBringBackCursor;
        fluxIn >> menuDefaultFontStat;
        fluxIn >> menuPBarVisualStyleStat;
        fluxIn >> menuHotKeyShowWndStat;
        fluxIn >> menuStartSeqStat;
        fluxIn >> menuCreateSeqStat;

        fluxIn.seekg(1, fluxIn.cur);//    Pour sauter le retour à la ligne
        std::getline(fluxIn, seqName);//   On récupère la ligne qui est le nom de la séquence à charger

        //  On affecte l'état des menu
        if(menuCurPosStat == MF_CHECKED)
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_MENU_CUR_POS, 0), 0);// 'Simule' un clic sur les menus
        if(menuBringBackCursor == MF_CHECKED)
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_MENU_BRING_BACK_CURSOR, 0), 0);// 'Simule' un clic sur les menus
        if(menuDefaultFontStat == MF_CHECKED)
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_MENU_DEFAULT_FONT, 0), 0);
        if(menuPBarVisualStyleStat == MF_CHECKED)
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_MENU_PBAR_VISUAL_STYLE, 0), 0);
        if(menuHotKeyShowWndStat == MF_CHECKED)
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_MENU_HOT_KEY_SHOW_WINDOW, 0), 0);
        if(menuStartSeqStat == MF_CHECKED)
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_MENU_HOT_KEY_START_SEQ, 0), 0);
        if(menuCreateSeqStat == MF_CHECKED)
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_MENU_HOT_KEY_CREATE_SEQ, 0), 0);

        if(seqName.size() > 0)//  Si c'est un nom correcte
        {
            std::string* seqPath = new std::string;
            *seqPath += SEQUENCES_FOLDER_NAME;
            *seqPath += seqName;
            *seqPath += FILE_EXTENSION;

            ///     Lancement du thread de chargement de la séquence
            hLoadSeqThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&loadSeqFromFile, (PVOID)seqPath, 0, 0);
            if(hLoadSeqThread == NULL)
            {
                delete(seqPath);//   On libère le string car il ne sera pas libéré dans le thread

                std::ostringstream codeError;
                codeError << "Impossible de lancer le thread de chargement de séquence\nCode d'erreur : ";
                codeError << GetLastError();
                MessageBox(0, codeError.str().c_str(), "Error", MB_OK);
            }
            else
            {
                #ifdef DEBUG
                    std::cout << "loadSeqFromFile thread created" << std::endl;
                #endif // DEBUG
            }
        }
        else
        {
            #ifdef DEBUG
                std::cout << "No sequence loaded before" << std::endl;
            #endif // DEBUG
        }

        #ifdef DEBUG
            std::cout << "Config Read" << std::endl;
        #endif // DEBUG
    }
    else
    {
        #ifdef DEBUG
            std::cout << "Can't open config file" << std::endl;
        #endif // DEBUG
    }
}

void writeConfig()
{
    CreateWritableDirectory(SOURCE_FOLDER_NAME);

    std::string configFilePath;
    configFilePath += SOURCE_FOLDER_NAME;
    configFilePath += CONFIG_FILE_NAME;

    std::ofstream fluxOut(configFilePath.c_str());
    if(fluxOut.good())
    {
        ///     état des menus
        fluxOut << GetMenuState(hMenuBar, ID_MENU_CUR_POS, MF_BYCOMMAND) << std::endl;
        fluxOut << GetMenuState(hMenuBar, ID_MENU_BRING_BACK_CURSOR, MF_BYCOMMAND) << std::endl;
        fluxOut << GetMenuState(hMenuBar, ID_MENU_DEFAULT_FONT, MF_BYCOMMAND) << std::endl;
        fluxOut << GetMenuState(hMenuBar, ID_MENU_PBAR_VISUAL_STYLE, MF_BYCOMMAND) << std::endl;
        fluxOut << GetMenuState(hMenuBar, ID_MENU_HOT_KEY_SHOW_WINDOW, MF_BYCOMMAND) << std::endl;
        fluxOut << GetMenuState(hMenuBar, ID_MENU_HOT_KEY_START_SEQ, MF_BYCOMMAND) << std::endl;
        fluxOut << GetMenuState(hMenuBar, ID_MENU_HOT_KEY_CREATE_SEQ, MF_BYCOMMAND) << std::endl;
        ///     On écrit le nom de la séquence chargée
        std::string seqName;
        seqName = currSeq.getName();//  On récupère le nom de la séquence
        if(seqName != DEFAULT_SEQ_NAME && seqName.size() > 0)//   uniquement si une séquence est chargée on l'écrit
            fluxOut << seqName;

        #ifdef DEBUG
            std::cout << "Config Written" << std::endl;
        #endif // DEBUG
    }
    else
    {
        #ifdef DEBUG
            std::cout << "Can't open Config file" << std::endl;
        #endif // DEBUG
    }
}

int getTimeEdit()
{
    int textLength = GetWindowTextLength(hActionTime);
    if(textLength < 1)//    si il n'y a pas de texte dans l'edit
        return 0;
    char text[textLength+1];
    GetWindowText(hActionTime, text, textLength+1);
    return atoi(text);
}

void setTimeEdit(uint32_t time)
{
    std::ostringstream text;
    text << time;
    SetWindowText(hActionTime, text.str().c_str());
}

void createMenu(HWND hwnd)
{
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hOptionsMenu, "Options");//   Creation du menu 'Options' dans la barre de menu

    AppendMenu(hOptionsMenu, MF_POPUP, ID_MENU_SET_SAME_TIME, "");//    Le texte sera géré par WM_ENTERMENULOOP
    AppendMenu(hOptionsMenu, MF_POPUP, ID_MENU_BRING_BACK_CURSOR, TXT_MENU_BRING_BACK_CURSOR);
    AppendMenu(hOptionsMenu, MF_POPUP, ID_MENU_CUR_POS, TXT_MENU_CURPOS);//   Creation du menu 'CurPos'
    AppendMenu(hOptionsMenu, MF_POPUP, ID_MENU_DEFAULT_FONT, TXT_MENU_DEFAULT_FONT);
    AppendMenu(hOptionsMenu, MF_POPUP, ID_MENU_PBAR_VISUAL_STYLE, TXT_MENU_PBAR_VISUAL_STYLE);
    AppendMenu(hOptionsMenu, MF_POPUP, ID_MENU_HOT_KEY_SHOW_WINDOW, TXT_MENU_HOT_KEY_SHOW_WINDOW);
    AppendMenu(hOptionsMenu, MF_POPUP, ID_MENU_HOT_KEY_START_SEQ, TXT_MENU_HOT_KEY_LUNCH_SEQ);
    AppendMenu(hOptionsMenu, MF_POPUP, ID_MENU_HOT_KEY_CREATE_SEQ, TXT_MENU_HOT_KEY_CREATE_SEQ);

    SetMenu(hwnd, hMenuBar);
}

void createTrayIcon(HWND hwnd)
{
    NOTIFYICONDATA trayIcon;
    ZeroMemory(&trayIcon, sizeof(NOTIFYICONDATA));
    trayIcon.cbSize = sizeof(NOTIFYICONDATA);
    trayIcon.hWnd = hwnd;
    trayIcon.uID = 0;// On met 0 car on aura qu'un seul trayicon pour cette hwnd et pour toute l'appli
    trayIcon.hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON_TRAY_NOTIF), IMAGE_ICON, 0, 0, 0);
    trayIcon.uCallbackMessage = ID_CALLBACK_TRAY_ICON;
    trayIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    strcpy(trayIcon.szTip, TXT_TRAY_MENU_TIP);  /*  bulle quand la souris reste sur la tray icon    */

    Shell_NotifyIcon(NIM_ADD, &trayIcon);//  On crée la trayIcon
}

void trayIconProc(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    ///     CLIC GAUGHE
    if(lParam == WM_LBUTTONUP)
    {
        ShowWindow(hwnd, SW_RESTORE);
        SetForegroundWindow(hwnd);
        FlashWindow(hwnd, true);
    }
    ///     CLIC DROIT
    if(lParam == WM_RBUTTONUP)
    {
        HMENU trayMenu = CreatePopupMenu();//   Création du menu
        InsertMenu(trayMenu, 0, MF_BYPOSITION | MF_STRING, ID_BTN_START_SEQ, TXT_MENU_TRAY_MENU_START_SEQ);
        InsertMenu(trayMenu, 1, MF_BYPOSITION | MF_STRING, ID_BTN_SIMULATE_SEQ , TXT_MENU_TRAY_MENU_SIMULATE_SEQ);

        SetForegroundWindow(hwnd);//    Sinon menu ne disparait pas
        SendMessage(hwnd, WM_INITMENUPOPUP, (WPARAM)trayMenu, 0 );

        //SHOW POPUP MENU.-----------------------------------------------------------------
        //GET CURSOR POSITION TO CREATE POPUP THERE.-------------------------------------
        POINT curPos;
        GetCursorPos(&curPos);

        //DISPLAT MENU AND WAIT FOR SELECTION.-----------------------------------------
        WORD cmd = TrackPopupMenu(trayMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_BOTTOMALIGN, curPos.x, curPos.y, 0, hwnd, NULL);
        //SEND MESSAGE MAPPED TO SELECTED ITEM.----------------------------------------
        SendMessage(hwnd, WM_COMMAND, cmd, 0);
        DestroyMenu(trayMenu);
    }
}

HBRUSH CreateGradientBrush(COLORREF top, COLORREF bottom, LPDRAWITEMSTRUCT item)
{
    //  On récupère le rectangle de la client area
    RECT rcClient;
    GetClientRect(item->hwndItem, &rcClient);

    HBRUSH Brush = NULL;
    HDC hdcmem = CreateCompatibleDC(item->hDC);
    HBITMAP hbitmap = CreateCompatibleBitmap(item->hDC, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top);
    SelectObject(hdcmem, hbitmap);

    int r1 = GetRValue(top), r2 = GetRValue(bottom), g1 = GetGValue(top), g2 = GetGValue(bottom), b1 = GetBValue(top), b2 = GetBValue(bottom);
    for(int i = 0; i < rcClient.bottom-rcClient.top; i++)
    {
        RECT temp;
        int r,g,b;
        r = int(r1 + double(i * (r2-r1) / rcClient.bottom-rcClient.top));
        g = int(g1 + double(i * (g2-g1) / rcClient.bottom-rcClient.top));
        b = int(b1 + double(i * (b2-b1) / rcClient.bottom-rcClient.top));
        Brush = CreateSolidBrush(RGB(r, g, b));
        temp.left = 0;
        temp.top = i;
        temp.right = rcClient.right-rcClient.left;
        temp.bottom = i + 1;
        FillRect(hdcmem, &temp, Brush);
        DeleteObject(Brush);
    }
    HBRUSH pattern = CreatePatternBrush(hbitmap);

    DeleteDC(hdcmem);
    DeleteObject(Brush);
    DeleteObject(hbitmap);

    return pattern;
}

void cursorListener()
{
    /*  Si on affiche pas déjà la position du curseur dans la status bar et dans les spins boxes */
    if(!displayCurPosStatusBar || !displayCurPosSpinBox)
    {
        /*  On indique que le thread curPos tourne  */
        curPosThreadRunning = true;

        int16_t posX(0);
        int16_t posY(0);

        int16_t oldPosX(0);
        int16_t oldPosY(0);
        while(displayCurPosStatusBar || displayCurPosSpinBox)
        {
            /*  On récupère la position du curseur  */
            posX = cmd.getCurPosX();
            posY = cmd.getCurPosY();

            if(displayCurPosStatusBar)
            {
                std::ostringstream curPos;
                curPos << posX;
                curPos << ", ";
                curPos << posY;

                /*  Si la position du curseur a changé on l'afiche sinon non    */
                if(posX != oldPosX || posY != oldPosY)
                    botClic.setStatusBarText(STATUS_BAR_CUR_POS_PART, (LPSTR)curPos.str().c_str());
            }
            if(displayCurPosSpinBox)
            {
                if(posX != oldPosX || posY != oldPosY)
                {
                    sbActionPosX.setPos(posX);
                    sbActionPosY.setPos(posY);
                }
            }

            oldPosX = posX;
            oldPosY = posY;

            Sleep(SLEEP_TIME_60FPS);  /*  60FPS -> 1000/60 -> 17  */
        }
    }
    curPosThreadRunning = false;
    #ifdef DEBUG
        std::cout << "CurPos thread finished" << std::endl;
    #endif // DEBUG
}

void keyboardListener()
{
    listeningKeyboard = true;// On indique que le thread tourne, qu'on écoute le clavier
    btnCreateSeq.enable(false);

    bool listeningKeysStat[NBR_COMMAND+1];//    tab de l'état des touches écoutés
    bool oldListeningKeysStat[NBR_COMMAND+1];// tab de l'ancien état
    int listeningKeyCode[NBR_COMMAND+1];//      tab des key code

    //  On laisse la place pour la dernière touche 'f'
    for(int i(0); i < NBR_COMMAND; i++)
        listeningKeyCode[i] = cmd.getKeyByCommand(i+1);

    listeningKeyCode[NBR_COMMAND] = VK_END_CREATING_SEQ;    //  on ajoute le caractère de fin ("F" par défaut)

    while(listeningKeyboard)
    {
        for(int i(0); i < NBR_COMMAND+1; i++)
        {
            /*  On prend l'état des touche qu'on a prédéfinie dans listeningKeyCode  */
            listeningKeysStat[i] = GetKeyState(listeningKeyCode[i]) & MASK;

            /*  Si l'état de la touche a changé et que son état est appuyé  */
            if(listeningKeysStat[i] != oldListeningKeysStat[i] && listeningKeysStat[i])
            {
                //  Si on écoute la dernière touche, c'est 'f' pour arreter le thread
                if(listeningKeyCode[i] == listeningKeyCode[NBR_COMMAND])
                {
                    listeningKeyboard = false;
                    #ifdef DEBUG
                        std::cout << "Arret du thread keyboardListener" << std::endl;
                    #endif // DEBUG
                }
                else
                {
                    /*  i est la commande correspondante grace a l'ordre de listeningKeyCode */
                    int time = getTimeEdit();// On récupère le temps sélectionné dans la spin box
                    addAction(cmd.getCurPosX(), cmd.getCurPosY(), i+1, time, APPEND_ACTION);//
                    #ifdef DEBUG
                        std::cout << "Clee appuye : " << cmd.getDescByCommand(i+1)<< std::endl;
                    #endif // DEBUG
                }
            }

            /*  On met a jour oldListeningKeysStat  */
            oldListeningKeysStat[i] = listeningKeysStat[i];
        }
        Sleep(50);
    }

    listeningKeyboard = false;//  Au cas ou un probleme se serais passé
    btnCreateSeq.enable(true);
}

void newActionBtnThreadProc()
{
    /*  On désactive le bouton new action, on change le texte et on dit que le thread tourne    */
    newActionBtnThreadRunning = true;
    btnNewAction.enable(false);
    btnNewAction.setText(TXT_BTN_NEW_ACTION_CONFIRM);
    ResetEvent(hEditActionMutex);// état non signalé -> pr ne pas prendre en compte messages des edit listbox Action
    displayCurPosSpinBox = true;

    /*  Si le thread pour afficher la souris ne tourne pas déjà on le lance */
    if(!curPosThreadRunning)
    {
        DWORD curPosThreadID;
        hCurPosThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&cursorListener, 0, 0, &curPosThreadID);
        if(hCurPosThread == NULL)
        {
            #ifdef DEBUG
                std::cout << "Impossible de lancer le thread d'écoute de la souris\nCode d'erreur : " << GetLastError() << std::endl;
            #endif // DEBUG
            displayCurPosSpinBox = false;
        }
        else
        {
            #ifdef DEBUG
                std::cout << "Cur pos thread created" << std::endl;
            #endif // DEBUG
        }
    }

    bool returnStat(false);
    /*  tant qu'on a pas appuyé sur entrée  */
    while(!returnStat)
    {
        /*  on actualise l'état de la touche entrée */
        returnStat = GetKeyState(0x0D) & MASK;
        if(GetKeyState(0x1B) & MASK)//  Si appuie sur échap on fini le thread
            break;

        Sleep(SLEEP_TIME_60FPS);  /*  60FPS -> 1000/60 -> 17  */
    }
    displayCurPosSpinBox = false;
    SetEvent(hEditActionMutex);//   etat signalé -> on peut prendre en compte les messages des edit dans listbox Action

    /*  On récupère l'index de la sélection dans la listBox  si appuie sur entré */
    if(returnStat)
    {
        int selItem = SendMessage(hListBoxActions, LB_GETCURSEL, 0, 0);
        addAction(sbActionPosX.getPos(), sbActionPosY.getPos(), cbCommand.getPosition()+1, getTimeEdit(), selItem+1);
        //      +1 pour la commande -> pour compenser le décalage entre les valeurs des action et la position du comboBox
    }

    /*  On active le bouton new action, on remet le texte d'origine et on dit que le thread ne tourne plus   */
    btnNewAction.enable(true);
    btnNewAction.setText(TXT_BTN_NEW_ACTION_DEFAULT);
    newActionBtnThreadRunning = false;
}

void editKeyProc()
{
    editKeyThreadRunning = true;
    newKeyCode = 0; //  newKeyCode -> code de la touche appuyée, modifiée dans WM_KEYDOWN
    btnEditKey.enable(false);
    btnEditKey.setText(TXT_BTN_EDIT_KEY_CHOOSE);

    DWORD rtVal;
    //  On attend que la touche soit reçus dans WM_KEYDOWN
    #ifdef DEBUG
        std::cout << "Waiting..." << std::endl;
    #endif // DEBUG
    rtVal = WaitForSingleObject(hNewKeyCodeAvailable, EDIT_KEY_TIMEOUT);

    switch(rtVal)
    {
        //  l'objet est signalé donc une touche a été appuyée : on assigne la nouvelle touche à la commande
        case WAIT_OBJECT_0:/// Touche appuyée: on assigne
        {
            int command = cbCommandKey.getPosition()+1; //  +1 pour que ça corresponde aux valeurs des commandes

            //  On vérifie que la touche ne soit pas déjà utilisée
            for(int i(1); i < NBR_COMMAND+1; i++)// +1 pour compenser le décalage des commandes
            {
                //  Si elle est utilisé on avertit l'utilisateur et on ne sauvegarde pas
                if(cmd.getKeyByCommand(i) == newKeyCode || newKeyCode == VK_END_CREATING_SEQ)
                {
                    #ifdef DEBUG
                        std::cout << "editKeyProc() : Key already used by command " << i+1 << std::endl;
                    #endif // DEBUG

                    std::ostringstream oss;
                    if(newKeyCode == VK_END_CREATING_SEQ)//   Si c'est la touche de fin de creation de seq
                        oss << "Cette touche est utilisée pour mettre fin à la creation de séquence";
                    else
                    {
                        oss << "Cette touche est déjà utilisée par la commande\n";
                        oss << cmd.getDescByCommand(i);
                    }

                    MessageBox(NULL, oss.str().c_str(), "Assignement impossible", MB_OK);
                    //  On remet tout comme avant et on arrete la fonc
                    ResetEvent(hNewKeyCodeAvailable);// On met a l'état non signalé
                    btnEditKey.enable(true);
                    btnEditKey.setText(TXT_BTN_EDIT_KEY_DEFAULT);
                    editKeyThreadRunning = false;
                    return;
                }
            }
            //  Si on arrive ici, la touche n'est pas utilisée
            #ifdef DEBUG
                std::cout << "Key " << (char)newKeyCode << " code " << (int)newKeyCode << " assigned to command " << command << std::endl;
            #endif // DEBUG

            cmd.editKey(command, newKeyCode, isLaunchedFromExe);   //  On modifie la touche et on écrit ds fichier si lancé depuis exe
            fillKeyComboBox(cbCommandKey.getPosition()); //  On actualise le contenue du combobox des touches
            fillKeyListBox();// On actualise la list box dans l'onglet créer
            break;
        }
        //  Le temps s'est écoulé, on ne modifie pas la touche
        case WAIT_TIMEOUT:
        {
            #ifdef DEBUG
                std::cout << "editKeyProc() : Time out" << std::endl;
            #endif // DEBUG
            break;
        }
        //  Default est soit WAIT_FAILED soit WAIT_ABANDONED, on affiche une erreur
        default:
        {
            #ifdef DEBUG
                std::cout << "editKeyProc() : Couldn't wait for newKeyCodeAvailable to be signaled. Error code : " << GetLastError() << std::endl;
            #endif // DEBUG
            MessageBox(NULL, "Une erreur est survenue lors de l'attente d'une nouvelle touche.", "Erreur", MB_OK);
        }
    }
    //  On remet tout comme avant
    ResetEvent(hNewKeyCodeAvailable);// On met a l'état non signalé
    btnEditKey.enable(true);
    btnEditKey.setText(TXT_BTN_EDIT_KEY_DEFAULT);
    editKeyThreadRunning = false;
}

void addAction(int xCursor, int yCursor, unsigned int command, unsigned int time, int index)
{
    /*  Si la commande passée existe c'est qu'elle est entre NBR_COMMAND et 0   */
    if(command <= NBR_COMMAND && command > 0)
    {
        std::ostringstream actionText;
        actionText << xCursor << ", " << yCursor << ", " << cmd.getDescByCommand(command) << ", " << time << " ms";

        /*  Si on veut rajouter l'action à la fin   */
        if(index == APPEND_ACTION)
        {
            SendMessage(hListBoxActions, LB_ADDSTRING, 0, (LPARAM)actionText.str().c_str());
            currSeq.addAction(xCursor, yCursor, command, time);

            /*  On recupère le nombre d'item dans la listbox pour séléctionner le dernier item */
            int nbrItem(SendMessage(hListBoxActions, LB_GETCOUNT , 0, 0));
            SendMessage(hListBoxActions, LB_SETCURSEL, nbrItem-1, 0);// On séléctionne le dernier item */
        }
        else
        {
            SendMessage(hListBoxActions, LB_INSERTSTRING, index, (LPARAM)actionText.str().c_str());
            currSeq.addAction(xCursor, yCursor, command, time, index);

            /*  On séléctionne l'item qu'on vient d'ajouter  */
            SendMessage(hListBoxActions, LB_SETCURSEL, index, 0);
        }

        #ifdef DEBUG
            std::cout << "New action added: " << actionText.str() << std::endl;
        #endif // DEBUG
    }
    else
    {
        #ifdef DEBUG
            std::cout << "addAction () : Unknown command " << command << std::endl;
        #endif // DEBUG
    }
}

void deleteAction(int index)
{
    /*  Si l'index est compris entre la taille de la séquence et zéro : il est valide  */
    if(index < currSeq.size() && index >= 0)
    {
        /*  On supprime l'action dans la listbox et dans la séquance    */
        SendMessage(hListBoxActions, LB_DELETESTRING, index, 0);
        currSeq.deleteAction(index);
        #ifdef DEBUG
            std::cout << "Action deleted : " << index << std::endl;
        #endif // DEBUG

        /**             On séléctionne une nouvelle action      **/
        /*  Si le nombre d'actions dans la listbox est superieur a l'index de l'action qu'on vient de supprimer
            on séléctionne l'item suivant donc celui qui a le même index, sinon l'item précédent   */
        if(SendMessage(hListBoxActions, LB_GETCOUNT, index, 0) > index)
            SendMessage(hListBoxActions, LB_SETCURSEL, index, 0);
        else
            SendMessage(hListBoxActions, LB_SETCURSEL, index-1, 0);
    }
    else
    {
        #ifdef DEBUG
            std::cout << "Invalid index : " << index << " size of the sequence : " << currSeq.size() << std::endl;
        #endif // DEBUG
    }
}

void clearSeq()
{
    //  On supprime toutes la séquence
    currSeq.clear();
    SendMessage(hListBoxActions, LB_RESETCONTENT, 0, 0);

    #ifdef DEBUG
        std::cout << "Sequence clear" << std::endl;
    #endif // DEBUG
}

/*  On charge les séquences dans le dossier où elles sont enregistrées : Sequences\\  */
void loadSeqFromFolder()
{
    //  On crée le dossier, si celui ci existe il ne sera pas modifié
    //CreateDirectory(SEQUENCES_FOLDER_NAME, NULL);
    CreateWritableDirectory(SEQUENCES_FOLDER_NAME);

    HANDLE hFile;
    WIN32_FIND_DATA findFileData;
    std::string searchDirectory = SEQUENCES_FOLDER_NAME;
    searchDirectory += "*";//   Tout les fichiers
    hFile = FindFirstFile(searchDirectory.c_str(), &findFileData);//  On trouve le premier fichier du dossier

    while(FindNextFile(hFile, &findFileData))
    {
        /*  Si c'est bien un fichier .BCSeq et que ce n'est pas un dossier on le lit*/
        if(isBCSeq(findFileData.cFileName) && findFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
        {
            std::string fileName = findFileData.cFileName;

            /**     On enlève l'extention   **/
            int count(0);//     Sécurité : limiter la boucle a 100 essaies
            // fileName[fileName.size()-1] est le dernier caractère
            while(fileName[fileName.size()-1] != '.' && count < 100)//  Tant qu'on  apas trouvé le point et au bout de 100 essaies on arrête
            {
                fileName.pop_back();
                count++;
            }
            /*  Dernier pop back pour enlever le '.' de l'extention */
            fileName.pop_back();

            //  On l'ajoute a la listBox
            int totalItem = SendMessage(hListBoxFiles, LB_GETCOUNT, 0, 0);
            SendMessage(hListBoxFiles, LB_INSERTSTRING, totalItem, (LPARAM)fileName.c_str());
        }
    }

    #ifdef DEBUG
        int totalFilesAdded = SendMessage(hListBoxFiles, LB_GETCOUNT, 0, 0);
        std::cout << "Total file added : " << totalFilesAdded << std::endl;
    #endif // DEBUG
}

/*  Charger une séquence depuis un fichier dans la séquence du programme    */
void loadSeqFromFile(std::string* ptrSeqPath)
{
    SetLastError(0);//  Pour éviter les "perturbatiohs" des erreur précédentes
    //  On reconstitue le chemin du fichier
    std::string seqName = fileNameByPath(ptrSeqPath->c_str());
    for(int i(0);  i < strlen(FILE_EXTENSION); i++)// pour enlever l'extension
        seqName.pop_back();

    #ifdef DEBUG
        std::cout << "Loading file : " << *ptrSeqPath << std::endl;
    #endif // DEBUG

    File loadFile;
    /*  On charge la séquence depuis le fichier */
    if(loadFile.open(ptrSeqPath->c_str()))
    {
        Sequence tempSeq;   //  On crée une séquance temporaire pour transferer dans celle utilisée avec addAction
        loadFile.read(&tempSeq);//  Car addAction modifie currSeq, donc inutilisable
        loadFile.close();

        //  On remplie la listBox avec les actions qu'on vient de charger
        for(int i(0); i < tempSeq.size(); i++)
            addAction(tempSeq.getX(i), tempSeq.getY(i), tempSeq.getCommand(i), tempSeq.getTime(i), APPEND_ACTION);

        #ifdef DEBUG
            std::cout << "Seq size : " << currSeq.size() << std::endl;
        #endif // DEBUG

        //  On modifie le nom de la séquence dans la status bar et dans currSeq
        currSeq.setName(seqName.c_str());
        botClic.setStatusBarText(STATUS_BAR_SEQ_NAME_PART, seqName.c_str());
    }
    else//  On a pas pu ouvrir le fichier
    {
        std::ostringstream errorText;
        errorText << "Impossible de charger le fichier ";
        errorText << *ptrSeqPath;
        errorText << "\nErreur : ";
        if(GetLastError() == ERROR_FILE_NOT_FOUND)
            errorText << "ERROR_FILE_NOT_FOUND";
        else if(GetLastError() == ERROR_PATH_NOT_FOUND)
            errorText << "ERROR_PATH_NOT_FOUND";
        else if(GetLastError() == ERROR_ACCESS_DENIED)
            errorText << "ERROR_ACCESS_DENIED";
        else if(GetLastError() == ERROR_INVALID_NAME)
            errorText << "ERROR_INVALID_NAME";
        else//  Autre erreur
            errorText << GetLastError();

        MessageBox(NULL, errorText.str().c_str(), "Erreur de fichier", MB_OK);
    }
    delete(ptrSeqPath);//   on libère l'espace car on en a plus besoin
}

void saveSeq(std::string* seqFilePath)
{
    File saveFile;
    //  Si le fichier n'existe pas, on le crée
    if(!saveFile.open(seqFilePath->c_str(), std::ios::trunc | std::ios::in | std::ios::out))// On ouvre le fichier de sorte à éffacer son contenu
    {
        #ifdef DEBUG
            std::cout << "Creating file" << std::endl;
        #endif // DEBUG

        HANDLE hFile = INVALID_HANDLE_VALUE;
        //  FILE_SHARE_READ | FILE_SHARE_WRITE -> On autorise l'écriture et la lecture du fichier en même temps que nous
        hFile = CreateFile((LPCTSTR)seqFilePath->c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL /*| SECURITY_ANONYMOUS*/, NULL);
        //  On gère les erreur de creation du fichier
        if(hFile == INVALID_HANDLE_VALUE)
        {
            std::ostringstream errorText;
            errorText << "Impossible de créer le fichier ";
            errorText << *seqFilePath;
            errorText << "\nErreur : ";

            if(GetLastError() == ERROR_ACCESS_DENIED)
                errorText << "ERROR_ACCESS_DENIED";
            else if(GetLastError() == ERROR_INVALID_NAME)
                errorText << "ERROR_INVALID_NAME";
            else//  Autre erreur
                errorText << GetLastError();
            MessageBox(NULL, errorText.str().c_str(), "Erreur de fichier", MB_OK);
            #ifdef DEBUG
                std::cout << "Creating file error : "  << GetLastError() << " with file path: " << *seqFilePath << std::endl;
            #endif // DEBUG
        }
        else
        {
            #ifdef DEBUG
                std::cout << "File " <<  *seqFilePath << " created" << std::endl;
            #endif // DEBUG
        }
        CloseHandle(hFile);//   On ferme le fichier

        saveFile.open(seqFilePath->c_str());//  On le réouvre pour y enregistrer la séquence
    }
    else
    {
        #ifdef DEBUG
            std::cout << "File " <<  *seqFilePath << " opened, already created" << std::endl;
        #endif // DEBUG
    }

    saveFile.write(&currSeq);//  On écrit la séquence dans le fichier et on le ferme
    saveFile.close();
    //  Nouveau thread car manipule des fichiers
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&fillFileListBox, 0, 0, 0);//  On actualise la listBox des fichiers

    ///  On récupère le nom de la séquence
    std::string seqName;
    seqName = fileNameByPath(seqFilePath->c_str());//   On enlève le chemin
    //  On supprime un caractère en plus : le '.' de l'extension
    for(int i(0); i < strlen(FILE_EXTENSION); i++)//  On supprime l'extention du fichier pour avoir uniquement le nom du fichier
        seqName.pop_back();

    /// on affiche le nom de la séquence
    currSeq.setName(seqName.c_str());
    botClic.setStatusBarText(STATUS_BAR_SEQ_NAME_PART, (LPSTR)seqName.c_str());// On affiche le nom de la séquence dans la status bar
    delete(seqFilePath);
}

void deleteFile(std::string* filePath)
{
    if(DeleteFile((LPCTSTR)filePath->c_str()) == 0)
    {
        //  Erreur de la suppression
        #ifdef DEBUG
            std::cout << "DeleteFile() returned error : " << GetLastError() << " on file path : " << *filePath << std::endl;
        #endif // DEBUG
        std::ostringstream errorTxt;
        errorTxt << "Erreur de la suppression du fichier ";
        errorTxt << *filePath;
        errorTxt << "\nErreur : ";
        errorTxt << GetLastError();

        MessageBox(NULL, errorTxt.str().c_str(), "Erreur de suppression", MB_OK);
    }
    else
    {
        #ifdef DEBUG
            std::cout << "File " << *filePath << " deleted " << std::endl;
        #endif // DEBUG
    }

    //  Dans un nouveau thread car manipule des fichiers
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&fillFileListBox, 0, 0, 0);//    On actualise la listBox des fichiers
    delete(filePath); // On libère l'espace car on ne s'en servira plus
}

void runSeq(Sequence* seq)//    hRunSeqThread
{
    SetEvent(hRunSeqThread); //  On signale la HANDLE pour dire que le thread tourne
    SendMessage(hPBarSequence, PBM_SETPOS, 0, 0);//    On met a zéro la pbBar
    SendMessage(hPBarSequence, PBM_SETRANGE, 0, MAKELPARAM(0, PBBAR_WIDTH));//    On met le max à la longueur de la pbar

    POINT prevCurPos;
    if(GetMenuState(hMenuBar, ID_MENU_BRING_BACK_CURSOR, MF_BYCOMMAND) == MF_CHECKED)// On récupère la position du curseur pour le repositionner après
        GetCursorPos(&prevCurPos);
    if(seq->size() > 0)
    {
        int pbStep = PBBAR_WIDTH/seq->size();
        SendMessage(hPBarSequence, PBM_SETSTEP, pbStep, 0);//  On applique le 'palier'

        for(int i(0); i < seq->size(); i++)
        {
            SendMessage(hListBoxActions, LB_SETCURSEL, i, 0);// On sélectionne l'action de la seq qu'on va executer
            SendMessage(GetParent(hListBoxActions), WM_COMMAND, MAKELONG(ID_LIST_BOX_ACTIONS, LBN_SELCHANGE), 0);// On actualise les spin boxes

            uint32_t sleepTime = seq->getTime(i);
            if(sleepTime > 0)
                Sleep(sleepTime);//   On attend le temps voulu

            cmd.setCurPos(seq->getX(i), seq->getY(i));//    On change la position du curseur
            cmd.doCommand(seq->getCommand(i));//    on exécute la commande, +1 pour le décalage avec doCommand()
            SendMessage(hPBarSequence, PBM_STEPIT, 0, 0);//    On avance la pbBar
        }
        SendMessage(hPBarSequence, PBM_SETPOS, PBBAR_WIDTH, 0);//   On la met a la fin bout a cause du truncat de pbStep
        if(GetMenuState(hMenuBar, ID_MENU_BRING_BACK_CURSOR, MF_BYCOMMAND) == MF_CHECKED)// On remet le curseur en fonction du menu
            SetCursorPos(prevCurPos.x, prevCurPos.y);

        #ifdef DEBUG
            std::cout << "Sequence successfully finished" << std::endl;
        #endif // DEBUG
    }
    else
    {
        #ifdef DEBUG
            std::cout << "Sequence finished, empty sequence" << std::endl;
        #endif // DEBUG
    }

    btnSimulateSeq.enable(true);//  On le réactive pour qu'il soit utilisable
    btnStartSeq.setText(TXT_BTN_START_SEQ);//     "Pause"
    ResetEvent(hRunSeqThread);//    On met le handle a l'état non signalé pour dire que le thread ne tourne plus
}

void simulateSeq(Sequence* seq)
{
    SetEvent(hSimulateSeqThread); //  On signale le HANDLE pour dire que le thread tourne

    SendMessage(hPBarSequence, PBM_SETPOS, 0, 0);//    On met a zéro la pbBar
    SendMessage(hPBarSequence, PBM_SETRANGE, 0, MAKELPARAM(0, PBBAR_WIDTH));//    On met le max à la longueur de la pbar

    POINT prevCurPos;
    if(GetMenuState(hMenuBar, ID_MENU_BRING_BACK_CURSOR, MF_BYCOMMAND) == MF_CHECKED)// On récupère la position du curseur pour le repositionner après
        GetCursorPos(&prevCurPos);

    if(seq->size() > 0)//  Pour ne pas diviser par 0 si la séquence est vide
    {
        int pbStep(0);
        pbStep = PBBAR_WIDTH/seq->size();//  On calcule la valeur d'un 'palier' de la pbBar
        SendMessage(hPBarSequence, PBM_SETSTEP, pbStep, 0);//  On applique les 'paliers'

        for(int i(0); i < seq->size(); i++)
        {
            SendMessage(hListBoxActions, LB_SETCURSEL, i, 0);// On sélectionne l'action de la seq qu'on va executer
			SendMessage(GetParent(hListBoxActions), WM_COMMAND, MAKELONG(ID_LIST_BOX_ACTIONS, LBN_SELCHANGE), 0);// On actualise les spin boxes

            cmd.setCurPos(seq->getX(i), seq->getY(i));//    On change la position du curseur
            Sleep(SLEEP_TIME_SIMULATION_SEQ);
            SendMessage(hPBarSequence, PBM_STEPIT, 0, 0);//    On avance la pbBar
        }
        SendMessage(hPBarSequence, PBM_SETPOS, PBBAR_WIDTH, 0);//   On la met a la fin bout a cause du truncat de pbStep
        if(GetMenuState(hMenuBar, ID_MENU_BRING_BACK_CURSOR, MF_BYCOMMAND) == MF_CHECKED)// On remet le curseur en fonction du menu
            SetCursorPos(prevCurPos.x, prevCurPos.y);
    }

    btnSimulateSeq.setText(TXT_BTN_SIMULATE_SEQ);//   On remet le texte par défaut
    btnStartSeq.enable(true);// O nautorise l'appuie sur le bouton pour lancer une séquence
    ResetEvent(hSimulateSeqThread);//    On met le handle a l'état non signalé pour dire que le thread ne tourne plus
    #ifdef DEBUG
        std::cout << "Simulation finished" << std::endl;
    #endif // DEBUG
}

void fillKeyComboBox(int pos)//    Remplire la combobox des touches
{
    cbCommandKey.clear();   //  On vide le contenus
    for(int i(0); i < NBR_COMMAND; i++)
    {
        //  On crée le texte de la ligne
        //ToAscii
        std::ostringstream ossLine;
        ossLine << cmd.getDescByCommand(i+1);   //  i+1 pour correspondre aux commandes
        ossLine << " : ";
        ossLine << (char)cmd.getKeyByCommand(i+1);

        cbCommandKey.addLine(ossLine.str().c_str());
    }
    cbCommandKey.setPosition(pos);
}

void fillKeyListBox()
{
    SendMessage(hListBoxKey, LB_RESETCONTENT, 0, 0);//  On efface tout
    for(int i(0); i < NBR_COMMAND; i++)
    {
        std::ostringstream text;//  On crée le texte à mettre dans la listbox
        text << cmd.getDescByCommand(i+1) << " : " << (char)cmd.getKeyByCommand(i+1);
        SendMessage(hListBoxKey, LB_ADDSTRING, 0, (LPARAM)text.str().c_str());//  On ajoute le texte
    }
}

void fillFileListBox()
{
    int selIndex = SendMessage(hListBoxFiles, LB_GETCURSEL, 0, 0);//    On récupère l'index séléctionné
    SendMessage(hListBoxFiles, LB_RESETCONTENT, 0, 0);//  On efface tout
    loadSeqFromFolder();

    //  Si il y a plus d'item que l'index, on séléctionne l'item précédent
    (SendMessage(hListBoxFiles, LB_GETCOUNT, 0, 0) > selIndex) ? SendMessage(hListBoxFiles, LB_SETCURSEL, selIndex, 0) : SendMessage(hListBoxFiles, LB_SETCURSEL, selIndex-1, 0);
}

void setWindowFont(bool setFont)
{
    HFONT hBtnStartSeqFont;
    HFONT hBtnsFont;
    HFONT hLittleBtnsFont;
    HFONT hListsFont;
    HFONT hSBsFont;
    //  On fait une copie des font déjà crée
    if(setFont)
    {
        //Gisha /  Kalinga / Neo Sans
        hBtnStartSeqFont = CreateFont(24, 12, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Gisha");
        hBtnsFont = CreateFont(24, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Calibri");
        hLittleBtnsFont = CreateFont(22, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Calibri");
        hListsFont = CreateFont(18, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Calibri");
        hSBsFont = CreateFont(20, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Consolas");
    }
    //  On mes les font a NULL pour que les font par défaut soient utilisées
    else
    {
        hBtnStartSeqFont = NULL;
        hBtnsFont = NULL;
        hLittleBtnsFont = NULL;
        hListsFont = NULL;
        hSBsFont = NULL;
    }

    ///     Attribution des polices
    btnStartSeq.setFont(hBtnStartSeqFont);
    //InvalidateRect(hBtnStartSeq, NULL, true);
    /*  Onglet Créer    */
    btnCreateSeq.setFont(hBtnsFont);

    /*  Onglet Charger */
    btnLoadFile.setFont(hBtnsFont);
    btnSaveFile.setFont(hBtnsFont);
    btnDeleteFile.setFont(hLittleBtnsFont);

    /*  Onglet Modifier */
    btnNewAction.setFont(hLittleBtnsFont);
    sbActionPosX.setFont(hSBsFont);
    sbActionPosY.setFont(hSBsFont);
    SendMessage(hActionTime, WM_SETFONT, (WPARAM)hSBsFont, MAKELPARAM(true, 0));
    btnSimulateSeq.setFont(hLittleBtnsFont);
    cbCommand.setFont(hListsFont);

    /*  Onglet Options  */
    cbCommandKey.setFont(hListsFont);
    btnEditKey.setFont(hBtnsFont);

    #ifdef DEBUG
        std::cout << "New font set : " << setFont << std::endl;
    #endif // DEBUG
}

LRESULT CALLBACK spinBoxesProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
		case WM_CHAR:
        {
            std::cout << "WM_CHAR : " << (char)wParam << ", lParam : " << lParam << std::endl;
            if(tolower(wParam) == 'x' || tolower(wParam) == 'y')
            {

            }
			break;
        }


        default:
            //std::cout << "msg : " << msg << std::endl;
			CallWindowProc(oldSpinBoxesProc, hwnd, msg, wParam, lParam);
    }
}

void listBoxesControl(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
    {
        case ID_LIST_BOX_ACTIONS:
        {
            switch(HIWORD(wParam))
            {
                ///     Changement de sélection : on change les valeurs de X Y Time et Command
                case LBN_SELCHANGE:
                {
					/*  La séléction dans la listBox a changée, on modifie les valeurs des spin box X, Y, Time et command */
                    if(WaitForSingleObject(hNewActionBtnThread, 0) == WAIT_OBJECT_0)//  Si le thread de nouvelle action ne tourne pas
                    {
                        ResetEvent(hEditActionMutex);// On met a l'état non signalé pour ne pas prendre en compte les messages des edit

                        int selectedItem = SendMessage(hListBoxActions, LB_GETCURSEL , 0, 0);
                        sbActionPosX.setPos(currSeq.getX(selectedItem));
                        sbActionPosY.setPos(currSeq.getY(selectedItem));
                        setTimeEdit(currSeq.getTime(selectedItem));
                        cbCommand.setPosition(currSeq.getCommand(selectedItem)-1);//    -1 pour supprimer le décalage des valeurs de commandes

                        SetEvent(hEditActionMutex);//   On signale pour indiquer qu'on peut prendre en compte les messages des edit
                    }

                    break;
                }
                ///     Double clic : on change la position de la souris à celle de l'action
                case LBN_DBLCLK:
                {
                    int selIdex = SendMessage(hListBoxActions, LB_GETCURSEL , 0, 0);//  On récupère l'index de l'action séléctionné
                    int xCurPox = currSeq.getX(selIdex);
                    int yCurPox = currSeq.getY(selIdex);//  On récupère les coordonnées dansla séquence

                    cmd.setCurPos(xCurPox, yCurPox);//  On change la positionde de la souris

                    break;
                }
            }
            break;
        }

        case ID_LIST_BOX_FILES:
        {
            switch(HIWORD(wParam))
            {
                ///     On charge le fichier
                case LBN_DBLCLK:
                {
                    btnLoadFile.clic();//   On simule l'appuie du bouton
                    break;
                }
                ///    On active ou désactive le bouton de chargement
                case LBN_SELCHANGE:
                {
                     //  Si un item est séléctionnée dans la listbox des fichiers on active le bouton pour charger les fichiers
                    (SendMessage(hwnd, LB_GETCURSEL, 0, 0) != LB_ERR) ? btnLoadFile.enable(true) : btnLoadFile.enable(false);
                    break;
                }
            }

            break;
        }

        case ID_HWND_EDIT_ACTION:
        {
            if(HIWORD(wParam) == EN_CHANGE || HIWORD(wParam) == CBN_SELCHANGE)
            {
                if(WaitForSingleObject(hEditActionMutex, 0) == WAIT_OBJECT_0)// si on est autorisé à modifier la veleur de l'acion dans la listBox Action
                {
                    int selIndex = SendMessage(hListBoxActions, LB_GETCURSEL , 0, 0);
                    deleteAction(selIndex);//   On supprime l'action pour ensuite l'ajouter avec les bonnes valeurs
                    addAction(sbActionPosX.getPos(), sbActionPosY.getPos(), cbCommand.getPosition()+1, getTimeEdit(), selIndex);

                    #ifdef DEBUG
                        std::cout << "Action successfully edited" << std::endl;
                    #endif // DEBUG
                }
            }
            break;
        }
    }
}

void menusControl(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
        case ID_MENU_BRING_BACK_CURSOR:
        {
            //  inversion de l'état du menu
            (GetMenuState(hMenuBar, ID_MENU_BRING_BACK_CURSOR, MF_BYCOMMAND) == MF_CHECKED) ? CheckMenuItem(hMenuBar, ID_MENU_BRING_BACK_CURSOR, MF_UNCHECKED) : CheckMenuItem(hMenuBar, ID_MENU_BRING_BACK_CURSOR, MF_CHECKED);;
            break;
        }

        case ID_MENU_CUR_POS:
        {
            /*  Si le thread ne tourne pas et qu'on affiche pas, on lance le thread et on affiche  */
            if(!curPosThreadRunning && !displayCurPosStatusBar)
            {
                displayCurPosStatusBar = true;

                DWORD curPosThreadID;
                hCurPosThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&cursorListener, 0, 0, &curPosThreadID);

                if(hCurPosThread == NULL)
                {
                    std::ostringstream codeError;
                    codeError << "Impossible de lancer le thread d'écoute de la souris\nCode d'erreur : ";
                    codeError << GetLastError();

                    SetForegroundWindow(hwnd);
                    MessageBox(hwnd, codeError.str().c_str(), "Error", MB_OK);
                }
                else
                {
                    CheckMenuItem(hMenuBar, ID_MENU_CUR_POS, MF_CHECKED);//   On coche le menu
                    #ifdef DEBUG
                        std::cout << "CurPos thread created" << std::endl;
                    #endif // DEBUG
                }
            }
            /*  Si le thread est lancé mais qu'on affiche pas curPos dans la statusBar, on affiche  */
            else if(curPosThreadRunning && !displayCurPosStatusBar)
            {
                displayCurPosStatusBar = true;
                CheckMenuItem(hMenuBar, ID_MENU_CUR_POS, MF_CHECKED);//   On coche le menu
            }
            /*  Sinon on arrête d'afficher curPos et on met le texte par déffaut   */
            else
            {
                displayCurPosStatusBar = false;
                /*  On affiche le texte par deffaut dans la status bar "---, ---"   */
                botClic.setStatusBarText(STATUS_BAR_CUR_POS_PART, (LPSTR)TEXT_CUR_POS_DEFAULT);
                CheckMenuItem(hMenuBar, ID_MENU_CUR_POS, MF_UNCHECKED);//   On decoche la case du menu
            }
            break;
        }

        case ID_MENU_DEFAULT_FONT:
        {
            if(GetMenuState(hMenuBar, ID_MENU_DEFAULT_FONT, MF_BYCOMMAND) == MF_CHECKED)
            {
                //  On inverse l'état du menu et on désactive la police par défaut
                setWindowFont(true);
                CheckMenuItem(hMenuBar, ID_MENU_DEFAULT_FONT, MF_UNCHECKED);
            }
            else//  On met la police par défaut
            {
                setWindowFont(false);
                CheckMenuItem(hMenuBar, ID_MENU_DEFAULT_FONT, MF_CHECKED);
            }

            break;
        }

        case ID_MENU_PBAR_VISUAL_STYLE:
        {
            ///     Si l'utilisateur a clické sur le menu et qu'il est décoché : on active les styles visuels
            if(GetMenuState(hMenuBar, ID_MENU_PBAR_VISUAL_STYLE, MF_BYCOMMAND) == MF_UNCHECKED)//    Menu décoché lorsque l'utilisateur a clické
            {
                ///     Activation styles visuels
                SetWindowTheme(hPBarSequence, L"Explorer", NULL);///    On effecte les styles de base de windows à la pbBar
                //    On arrète le thread qui change la couleur de pa pbBar
                TerminateThread(hPbBarColorThread, 0);
                CheckMenuItem(hMenuBar, ID_MENU_PBAR_VISUAL_STYLE, MF_CHECKED);
            }
            else
            {
                ///     Desactivation styles visuels
                SetWindowTheme(hPBarSequence, L"", L"");/// On enlève les style de la pbBar
                //  On lance le thread de changement des couleurs de la PbBar
                hPbBarColorThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&pbBarColor, (LPVOID)hwnd, 0, NULL);
                if(hPbBarColorThread == NULL){
                    std::ostringstream codeError;
                    codeError << "Impossible de lancer le thread de la bar de progression\nCode d'erreur : ";
                    codeError << GetLastError();

                    SetForegroundWindow(hwnd);
                    MessageBox(hwnd, codeError.str().c_str(), "Error", MB_OK);
                }
                else{
                    #ifdef DEBUG
                        std::cout << "pbBarColor thread created" << std::endl;
                    #endif // DEBUG
                }

                CheckMenuItem(hMenuBar, ID_MENU_PBAR_VISUAL_STYLE, MF_UNCHECKED);//    On coche le menu
            }
            break;
        }

        case ID_MENU_HOT_KEY_SHOW_WINDOW:
        {
            if(GetMenuState(hMenuBar, ID_MENU_HOT_KEY_SHOW_WINDOW, MF_BYCOMMAND) == MF_CHECKED)
            {
                //  Désactivement du menu : on enlève les raccourcis
                if(UnregisterHotKey(hwnd, ID_HOT_KEY_SHOW_WINDOW))
                    CheckMenuItem(hMenuBar, ID_MENU_HOT_KEY_SHOW_WINDOW, MF_UNCHECKED);
            }
            else
            {
                //  Activation du menu : on enregistre les raccourcis
                if(RegisterHotKey(hwnd, ID_HOT_KEY_SHOW_WINDOW, MOD_CONTROL | MOD_SHIFT, VK_HOT_KEY_SHOW_WINDOW)) //  Ctrl+Maj+B
                    CheckMenuItem(hMenuBar, ID_MENU_HOT_KEY_SHOW_WINDOW, MF_CHECKED);
            }
            break;
        }

        case ID_MENU_HOT_KEY_START_SEQ:
        {
            if(GetMenuState(hMenuBar, ID_MENU_HOT_KEY_START_SEQ, MF_BYCOMMAND) == MF_CHECKED)
            {
                //  Désactivement du menu : on enlève les raccourcis
                if(UnregisterHotKey(hwnd, ID_HOT_KEY_START_SEQ))
                    CheckMenuItem(hMenuBar, ID_MENU_HOT_KEY_START_SEQ, MF_UNCHECKED);
            }
            else
            {
                //  Activation du menu : on enregistre les raccourcis
                if(RegisterHotKey(hwnd, ID_HOT_KEY_START_SEQ, MOD_CONTROL | MOD_SHIFT, VK_HOT_KEY_START_SEQ)) //  Ctrl+Maj+L
                    CheckMenuItem(hMenuBar, ID_MENU_HOT_KEY_START_SEQ, MF_CHECKED);
            }
            break;
        }

        case ID_MENU_HOT_KEY_CREATE_SEQ:
        {
            if(GetMenuState(hMenuBar, ID_MENU_HOT_KEY_CREATE_SEQ, MF_BYCOMMAND) == MF_CHECKED)
            {
                //  Désactivement du menu : on enlève les raccourcis
                if(UnregisterHotKey(hwnd, ID_HOT_KEY_CREATE_SEQ))
                    CheckMenuItem(hMenuBar, ID_MENU_HOT_KEY_CREATE_SEQ, MF_UNCHECKED);
            }
            else
            {
                //  Activation du menu : on enregistre les raccourcis
                if(RegisterHotKey(hwnd, ID_HOT_KEY_CREATE_SEQ, MOD_CONTROL | MOD_ALT, VK_HOT_KEY_CREATE_SEQ)) //  Ctrl+Maj+S
                    CheckMenuItem(hMenuBar, ID_MENU_HOT_KEY_CREATE_SEQ, MF_CHECKED);
            }
            break;
        }

        case ID_MENU_SET_SAME_TIME:
        {
            uint32_t time = getTimeEdit();
            for(int i(0); i < currSeq.size(); i++)//    Pour chaque action, on la supprime puis la crée avec le temps de le'edit
            {
                int16_t posX = currSeq.getX(i);
                int16_t posY = currSeq.getY(i);
                uint16_t command = currSeq.getCommand(i);
                deleteAction(i);
                addAction(posX, posY, command, time, i);
            }

            break;
        }
    }
}

void buttonsControl(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
        case ID_BTN_START_SEQ:
        {
            btnSimulateSeq.enable(false);// On indique qu'on ne peut pas lancer la simulation et la séqeunce en même temps

            HANDLE hWaitThread[2];
            hWaitThread[0] = hRunSeqThread;
            hWaitThread[1] = hSimulateSeqThread;

            switch(WaitForMultipleObjects(2, hWaitThread, true, 0))
            {
                case WAIT_OBJECT_0:///   les handles sont signalés donc les threads ne tournent pas
                {
                    btnStartSeq.setText(TXT_BTN_START_SEQ_PAUSE);//     "Pause"
                    ///     On lance le thread d'éxecution de séquence avec en param la séquence qu'on veut lancer
                    DWORD runSeqThreadID;
                    hRunSeqThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&runSeq, &currSeq, 0, &runSeqThreadID);
                    if(hRunSeqThread == NULL)
                    {
                        std::ostringstream codeError;
                        codeError << "Impossible de lancer le thread d'éxecution de séquence\nCode d'erreur : ";
                        codeError << GetLastError();

                        SetForegroundWindow(hwnd);
                        MessageBox(hwnd, codeError.str().c_str(), "Error", MB_OK);
                    }
                    else
                    {
                        #ifdef DEBUG
                            std::cout << "RunSeq thread created" << std::endl;
                        #endif // DEBUG
                    }

                    break;
                }

                case WAIT_TIMEOUT:///    L'état du handle est non signalé donc il tourne déjà
                {
                    //  On regarde si c'est runSeq thread qui tourne, dans ce cas on met le thread en pause
                    if(WaitForSingleObject(hRunSeqThread, 0) == WAIT_TIMEOUT)
                    {
                        #ifdef DEBUG
                            std::cout << "RunSeq thread already running" << std::endl;
                        #endif // DEBUG

                        if(seqThreadPaused)
                        {
                            ///     Reprise du thread
                            ResumeThread(hRunSeqThread);
                            btnStartSeq.setText(TXT_BTN_START_SEQ_PAUSE);//     "Pause"

                            seqThreadPaused = false;//   On indique que le thread n'est pas en pause
                            #ifdef DEBUG
                                std::cout << "Sequence resumed" << std::endl;
                            #endif // DEBUG
                        }
                        else
                        {
                            ///     Mise en pause
                            SuspendThread(hRunSeqThread);
                            btnStartSeq.setText(TXT_BTN_START_SEQ_RESUME);//    "Reprendre"

                            seqThreadPaused = true;//   On indique que le thread est en pause
                            #ifdef DEBUG
                                std::cout << "Sequence paused" << std::endl;
                            #endif // DEBUG
                        }
                    }
                    else//  Sinon c'est simulate thread qui tourne
                    {
                        #ifdef DEBUG
                            std::cout << "simulateSeq thread already running, Or WaitForSingleObject failed" << std::endl;
                        #endif // DEBUG
                    }

                    break;
                }
            }

            break;
        }

        case ID_BTN_SIMULATE_SEQ:
        {
            btnStartSeq.enable(false);//    On désactive le bouton pour indiquer qu'on ne peut pas lancer les deux en même temps

            HANDLE hWaitThread[2];
            hWaitThread[0] = hRunSeqThread;
            hWaitThread[1] = hSimulateSeqThread;
            ///   On regarde qu'aucun thread runSeq ou simulateSeq ne tourne
            switch(WaitForMultipleObjects(2, hWaitThread, true, 0))
            {
                case WAIT_OBJECT_0:///   les handles sont signalés donc les threads ne tournent pas
                {
                    btnSimulateSeq.setText(TXT_BTN_SIMULATE_SEQ_STOP);//   On remet le texte par défaut
                    //  On lance le thread de simulation
                    hSimulateSeqThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&simulateSeq, &currSeq, 0, NULL);
                    if(hSimulateSeqThread == NULL)
                    {
                        std::ostringstream codeError;
                        codeError << "Impossible de lancer le thread de simulation\nCode d'erreur : ";
                        codeError << GetLastError();

                        SetForegroundWindow(hwnd);
                        MessageBox(hwnd, codeError.str().c_str(), "Error", MB_OK);
                    }
                    else
                    {
                        #ifdef DEBUG
                            std::cout << "SimulateSeq thread created" << std::endl;
                        #endif // DEBUG
                    }
                    break;
                }

                case WAIT_TIMEOUT:///    Un des deux thread tourne
                {
                    if(WaitForSingleObject(hSimulateSeqThread, 0) == WAIT_TIMEOUT)//    On regarde si c'est simulateSeq thread qui tourne
                    {
                        ///     Arret du thread
                        TerminateThread(hSimulateSeqThread, 0);//   On arrête le thread
                        btnSimulateSeq.setText(TXT_BTN_SIMULATE_SEQ);//   On remet le texte par défaut
                        btnStartSeq.enable(true);// O nautorise l'appuie sur le bouton pour lancer une séquence
                        SendMessage(hPBarSequence, PBM_SETPOS, 0, 0);//    On met a zéro la pbBar

                        #ifdef DEBUG
                            std::cout << "Simulation stopped" << std::endl;
                        #endif // DEBUG
                    }
                    else//  Sinon c'es runSeq thread qui trourne
                    {
                        #ifdef DEBUG
                            std::cout << "RunSeq thread running" << std::endl;
                        #endif // DEBUG
                    }
                    break;
                }
            }

            break;
        }

        case ID_BTN_NEW_SEQ:
        {
            /*  On vide la séquence  */
            clearSeq();

            currSeq.setName(DEFAULT_SEQ_NAME);// On met le nom par défaut pour séquence
            botClic.setStatusBarText(STATUS_BAR_SEQ_NAME_PART, (LPSTR)DEFAULT_SEQ_NAME);

            /**     On lance le thread d'écoute du clavier  **/
            /*  Si un thread d'écoute des touches n'est pas déjà lancé  */
            if(!listeningKeyboard)
            {
                /*  Ce thread appel addAction() à chaque touche appuyée étant écoutés */
                DWORD threadKeyboardListenerID;
                hKeyboardListenerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&keyboardListener, 0, 0, &threadKeyboardListenerID);
                if(hKeyboardListenerThread == NULL)
                {
                    std::ostringstream codeError;
                    codeError << "Impossible de lancer le thread d'écoute du clavier\nCode d'erreur : ";
                    codeError << GetLastError();

                    SetForegroundWindow(hwnd);
                    MessageBox(hwnd, codeError.str().c_str(), "Erreur", MB_OK);
                }
                else
                {
                    #ifdef DEBUG
                        std::cout << "Key listening thread created" << std::endl;
                    #endif // DEBUG
                }
            }

            break;
        }

        case ID_BTN_NEW_ACTION:
        {
            /*  Si le thread n'est pas déjà lancé   */
            if(!newActionBtnThreadRunning)
            {
                DWORD newActionBtnThreadID;
                hNewActionBtnThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&newActionBtnThreadProc, 0, 0, &newActionBtnThreadID);

                if(hNewActionBtnThread == NULL)
                {
                    std::ostringstream codeError;
                    codeError << "Impossible de lancer le thread de creation d'une nouvelle action\nCode d'erreur : ";
                    codeError << GetLastError();

                    SetForegroundWindow(hwnd);
                    MessageBox(hwnd, codeError.str().c_str(), "Erreur", MB_OK);
                }
                else
                {
                    #ifdef DEBUG
                        std::cout << "New action thread created" << std::endl;
                    #endif // DEBUG
                }
            }

            break;
        }

        case ID_BTN_DELETE_ACTION:
        {
            /*  On récupère l'index de l'action qu'on veut supprimer    */
            int deleteItemIndex = SendMessage(hListBoxActions, LB_GETCURSEL , 0, 0);
            deleteAction(deleteItemIndex);

            break;
        }

        case ID_BTN_LOAD_FILE:
        {
            if(SendMessage(hListBoxActions, LB_GETCOUNT, 0, 0) > 0)//   Si il y a des action dans le listbox on prévient
            {
                if(MessageBox(hwnd, "En chargant nouvelle une séquence vous perdrez tout travail non enregistré\nCharger quand même ?", "Charger ?", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES)
                    break;// L'utilisateur ne souhaite pas enregistrer
            }

            HANDLE hWaitThread[2];
            hWaitThread[0] = hRunSeqThread;
            hWaitThread[1] = hSimulateSeqThread;
            if(WaitForMultipleObjects(2, hWaitThread, true, 0) == WAIT_TIMEOUT)//   simulate thread ou seq thread tournent : on les arrêtes
            {
                ///     On remet les boutons à leur état normal
                TerminateThread(hRunSeqThread, 0);
                ResetEvent(hRunSeqThread);
                btnStartSeq.setText(TXT_BTN_START_SEQ);
                seqThreadPaused = false;
                btnStartSeq.enable(true);

                TerminateThread(hSimulateSeqThread, 0);
                ResetEvent(hSimulateSeqThread);
                btnSimulateSeq.setText(TXT_BTN_SIMULATE_SEQ);
                btnSimulateSeq.enable(true);
            }
            /*  On vide la séquence  */
            clearSeq();

            int selIndex = SendMessage(hListBoxFiles, LB_GETCURSEL, 0, 0);
            int16_t selFileNameLen = SendMessage(hListBoxFiles, LB_GETTEXTLEN, selIndex, 0);// On récupère la longeur du nom du fichier
            char selSeqName[selFileNameLen+1];
            SendMessage(hListBoxFiles, LB_GETTEXT, selIndex, (LPARAM)selSeqName);//   On récupère le nom de la séquence
            std::string* strSeqPath = new std::string;//    on l'alloue dynamiquement car le thread en aura besoin après la fin de la fonction
            *strSeqPath += SEQUENCES_FOLDER_NAME;
            *strSeqPath += selSeqName;
            *strSeqPath += FILE_EXTENSION;

            DWORD loadSeqThreadID;//                                On laisse le thread gerer la libération de l'espace du std::string*
            hLoadSeqThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&loadSeqFromFile, (PVOID)strSeqPath, 0, &loadSeqThreadID);

            if(hLoadSeqThread == NULL)
            {
                delete(strSeqPath);//   On libère le string car il ne sera pas libéré dans le thread

                std::ostringstream codeError;
                codeError << "Impossible de lancer le thread de chargement de séquence\nCode d'erreur : ";
                codeError << GetLastError();

                SetForegroundWindow(hwnd);
                MessageBox(hwnd, codeError.str().c_str(), "Error", MB_OK);
            }
            else
            {
                #ifdef DEBUG
                    std::cout << "loadSeqFromFile thread created" << std::endl;
                #endif // DEBUG
            }
            break;
        }

        case ID_BTN_SAVE_FILE:
        {
            int selItem = SendMessage(hListBoxFiles, LB_GETCURSEL, 0, 0);// On récupère l'index de l'item sélectionné
            /**  On récupère le texte de la ligne est sélectionné dans la listBox**/
            std::string seqName;
            if(selItem != LB_ERR)// On initialise seqName avec le nom selectionné
            {
                int selItemTextLen = SendMessage(hListBoxFiles, LB_GETTEXTLEN, selItem, 0);// On récupère la longueur du texte
                char selSeqName[selItemTextLen+1];
                SendMessage(hListBoxFiles, LB_GETTEXT, selItem, (LPARAM)selSeqName);//  On récupère le nom de la séquence
                seqName = selSeqName;//  On passe en param du Dld le nom pour qu'il soit dans l'edit
            }

            /**     Si un nom de séquence est entré     **///   On passe en param au dlg le nom de la seq qu'il affichera
            if(DialogBoxParam(instance, MAKEINTRESOURCE(DLG_SEQ_NAME_MAIN), NULL, (DLGPROC)DlgSeqName, (LPARAM)&seqName))
            {
                //  On crée le chemin du fichier
                std::string* filePath = new std::string;//    on l'alloue dynamiquement car le thread en aura besoin après la fin de la fonction
                *filePath += SEQUENCES_FOLDER_NAME;
                *filePath += seqName;
                *filePath += FILE_EXTENSION; //  .BCSeq

                ///  On lance le thread pour enregistrer la séquence
                hSaveSeqThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&saveSeq, (PVOID)filePath, 0, NULL);

                if(hSaveSeqThread == NULL)
                {
                    delete(filePath);//   On libère le string car il ne sera pas libéré dans le thread

                    std::ostringstream codeError;
                    codeError << "Impossible de lancer le thread d'enregistrement de séquence\nCode d'erreur : ";
                    codeError << GetLastError();

                    SetForegroundWindow(hwnd);
                    MessageBox(hwnd, codeError.str().c_str(), "Error", MB_OK);
                }
                else
                {
                    #ifdef DEBUG
                        std::cout << "SaveSeq thread created" << std::endl;
                    #endif // DEBUG
                }
            }
            else
            {
                #ifdef DEBUG
                    std::cout << "Getting seq name aborted " << std::endl;
                #endif // DEBUG
            }

            break;
        }

        case ID_BTN_DELETE_FILE:
        {
            int selItem = SendMessage(hListBoxFiles, LB_GETCURSEL, 0, 0);// On récupère l'index de l'item sélectionné
            if(selItem != LB_ERR)
            {
                int selItemTextLen = SendMessage(hListBoxFiles, LB_GETTEXTLEN, selItem, 0);// On récupère la longueur du texte
                char selSeqName[selItemTextLen+1];
                SendMessage(hListBoxFiles, LB_GETTEXT, selItem, (LPARAM)selSeqName);//  On récupère le nom de la séquence

                std::string* filePath = new std::string;
                *filePath += SEQUENCES_FOLDER_NAME;
                *filePath += selSeqName;
                *filePath += FILE_EXTENSION;

                hDeleteFileThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&deleteFile, filePath, 0, NULL);
                if(hDeleteFileThread == NULL)
                {
                    delete(filePath);//   On libère le string car il ne sera pas libéré dans le thread

                    std::ostringstream codeError;
                    codeError << "Impossible de lancer le thread de suppression de fichier\nCode d'erreur : ";
                    codeError << GetLastError();

                    SetForegroundWindow(hwnd);
                    MessageBox(hwnd, codeError.str().c_str(), "Error", MB_OK);
                }
                else
                {
                    #ifdef DEBUG
                        std::cout << "deleteFile thread created" << std::endl;
                    #endif // DEBUG
                }

            }

            break;
        }

        case ID_BTN_EDIT_KEY:
        {
            //  On lance le thread d'écoute pour une nouvelle touche si il ne tourne pas déjà
            if(!editKeyThreadRunning)
            {
                DWORD editKeyThreadID;
                hEditKeyThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&editKeyProc, 0, 0, &editKeyThreadID);

                if(hEditKeyThread == NULL)
                {
                    std::ostringstream codeError;
                    codeError << "Impossible de lancer le thread d'assignement de touche\nCode d'erreur : ";
                    codeError << GetLastError();

                    SetForegroundWindow(hwnd);
                    MessageBox(hwnd, codeError.str().c_str(), "Error", MB_OK);
                }
                else
                {
                    #ifdef DEBUG
                        std::cout << "editKeyProc thread created" << std::endl;
                    #endif // DEBUG
                }
            }
            SetFocus(hwnd); //  On perd le focus pour que la fenetre principale reçois WM_KEYDOWN

            break;
        }
    }
}

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case ID_CALLBACK_TRAY_ICON:
        {
            trayIconProc(hwnd, wParam, lParam);
            break;
        }

        case WM_CREATE:
        {
            #ifdef DEBUG
                std::cout << "Command line : " << GetCommandLine() << std::endl;
            #endif // DEBUG

            /**     DWM     **/
            /*MARGINS marg;
            marg.cxLeftWidth = -1;
            marg.cxRightWidth = -1;
            marg.cyTopHeight = -1;
            marg.cyBottomHeight = -1;
            DwmExtendFrameIntoClientArea(hwnd, &marg);*/
            DragAcceptFiles(hwnd, true);   //  La fenetre acceptera le drag and drop

            HFONT hListsFont = CreateFont(18, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Calibri");

            /**     Creation du tableau global    **/
            hMainTab = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
                                TAB_POSX, TAB_POSY, TAB_WIDTH, TAB_HEIGHT, hwnd, (HMENU)ID_MAIN_TAB, instance, NULL);
            tie.mask = TCIF_TEXT;

            tie.pszText = LPSTR(TXT_TAB_1);
            TabCtrl_InsertItem(hMainTab, 1, &tie);

            tie.pszText = LPSTR(TXT_TAB_2);
            TabCtrl_InsertItem(hMainTab, 2, &tie);

            tie.pszText = LPSTR(TXT_TAB_3);
            TabCtrl_InsertItem(hMainTab, 3, &tie);

            tie.pszText = LPSTR(TXT_TAB_4);
            TabCtrl_InsertItem(hMainTab, 4, &tie);
            /*  Modifie la hauteur d'un onglet a 30px   */
            SendMessage(hMainTab, TCM_SETITEMSIZE, 0, MAKELPARAM(10, 30));
            HFONT hMainTabFont = CreateFont(22, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Calibri");
            SendMessage(hMainTab, WM_SETFONT,(WPARAM)hMainTabFont,0);

            /**     Creation de la bar de status en bas de la fenetre   **/
            botClic.createStatusBar(hwnd, 4, WINDOW_WIDTH, ID_STATUS_BAR, hListsFont);
            botClic.setStatusBarText(STATUS_BAR_BOT_CLIC_VERSION_PART, BOTCLIC_VERSION);
            botClic.setStatusBarText(STATUS_BAR_CUR_POS_PART, TEXT_CUR_POS_DEFAULT);
            botClic.setStatusBarText(STATUS_BAR_SEQ_NAME_PART, DEFAULT_SEQ_NAME);

            /**     Creation du bouton de simulation        **/
            btnSimulateSeq.create(BS_OWNERDRAW, hwnd, instance, ID_BTN_SIMULATE_SEQ, BTN_SIMULATE_SEQ_POSX, BTN_SIMULATE_SEQ_POSY, BTN_SIMULATE_SEQ_WIDTH, BTN_SIMULATE_SEQ_HEIGHT);
            btnSimulateSeq.setText(TXT_BTN_SIMULATE_SEQ);

            /**     Creation du bouton de lancement de la séquance chargé   **/
            btnStartSeq.create(BS_OWNERDRAW, hwnd, instance, ID_BTN_START_SEQ, BTN_START_SEQ_POSX, BTN_START_SEQ_POSY, BTN_START_SEQ_WIDTH, BTN_START_SEQ_HEIGHT);
            btnStartSeq.setText(TXT_BTN_START_SEQ);

            /**     Creation de la bar de progression indiquant l'avancement d'un seqeuence   **/
            hPBarSequence = CreateWindow(PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                                          PBBAR_POSX, PBBAR_POSY, PBBAR_WIDTH, PBBAR_HEIGHT, hwnd, (HMENU)ID_PROGRESS_BAR, instance, NULL);

            /**     Creation du thread PBar     **/
            SetWindowTheme(hPBarSequence, L"", L"");

            hPbBarColorThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&pbBarColor, (LPVOID)hwnd, 0, NULL);
            if(hPbBarColorThread == NULL)
            {
                std::ostringstream codeError;
                codeError << "Impossible de lancer le thread de la bar de progression\nCode d'erreur : ";
                codeError << GetLastError();

                SetForegroundWindow(hwnd);
                MessageBox(hwnd, codeError.str().c_str(), "Error", MB_OK);
            }
            else
            {
                #ifdef DEBUG
                    std::cout << "pbBarColor thread created" << std::endl;
                #endif // DEBUG
            }

        /********************************************************************/
        /**                         ONGLET CREER                           **/
        /********************************************************************/
            /**     Creation du bouton pour créer une sequence  **/
            btnCreateSeq.create(BS_OWNERDRAW, hwnd, instance, ID_BTN_NEW_SEQ, BTN_CREATE_SEQ_POSX, BTN_CREATE_SEQ_POSY, BTN_CREATE_SEQ_WIDTH, BTN_CREATE_SEQ_HEIGHT);
            btnCreateSeq.setText(TXT_BTN_CREATE_SEQ);
            btnCreateSeq.show();

            /**     Creation du texte d'explication pour créer une sequence  **/
            hTextCreationSeq = CreateWindowEx(WS_EX_TRANSPARENT, "STATIC", TXT_TEXT_CREATION_SEQ, WS_CHILD | WS_VISIBLE,
                                                TEXT_CREATION_SEQ_POSX, TEXT_CREATION_SEQ_POSY, TEXT_CREATION_SEQ_WIDTH, TEXT_CREATION_SEQ_HEIGHT,
                                                hwnd, (HMENU)ID_TEXT_CREATION_SEQ, instance, NULL);

            SendMessage(hTextCreationSeq, WM_SETFONT,(WPARAM)hListsFont,0);
            ShowWindow(hTextCreationSeq, SW_SHOW);

            /**     Creation de la listbox des touches pour chaques actions  **/
            hListBoxKey = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTBOX, "", LBS_NOTIFY | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL,
                                            LISTBOX_KEY_POSX, LISTBOX_KEY_POSY, LISTBOX_KEY_WIDTH, LISTBOX_KEY_HEIGHT, hwnd,
                                                (HMENU)ID_LIST_BOX_KEY, instance, NULL);
            SendMessage(hListBoxKey, WM_SETFONT,(WPARAM)hListsFont, 0);

        /********************************************************************/
        /**                         ONGLET CHARGER                         **/
        /********************************************************************/
            /**     Creation du bouton pour charger un fichier  **/
            btnLoadFile.create(BS_OWNERDRAW, hwnd, instance, ID_BTN_LOAD_FILE, BTN_LOAD_FILE_POSX, BTN_LOAD_FILE_POSY, BTN_LOAD_FILE_WIDTH, BTN_LOAD_FILE_HEIGHT);
            btnLoadFile.setText(TXT_BTN_LOAD_FILE);
            /*  On le disable car aucun item ne sera séléctionné dans la listbow des fichiers   */
            btnLoadFile.enable(false);
            btnLoadFile.hide();

            /**     Creation du bouton pour sauvegarder une séquence    **/
            btnSaveFile.create(BS_OWNERDRAW, hwnd, instance, ID_BTN_SAVE_FILE, BTN_SAVE_FILE_POSX, BTN_SAVE_FILE_POSY, BTN_SAVE_FILE_WIDTH, BTN_SAVE_FILE_HEIGHT);
            btnSaveFile.setText(TXT_BTN_SAVE_FILE);
            btnSaveFile.hide();

            /**     Creation du bouton pour sauvegarder une séquence    **/
            btnDeleteFile.create(BS_OWNERDRAW, hwnd, instance, ID_BTN_DELETE_FILE, BTN_DELETE_FILE_POSX, BTN_DELETE_FILE_POSY, BTN_DELETE_FILE_WIDTH, BTN_DELETE_FILE_HEIGHT);
            btnDeleteFile.setText(TXT_BTN_DELETE_FILE);
            btnDeleteFile.hide();

            /**     Creation de la list box contenant les fichiers   **/
            hListBoxFiles = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTBOX, "", LBS_NOTIFY | LBS_SORT | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL,
                                            LISTBOX_FILES_POSX, LISTBOX_FILES_POSY, LISTBOX_FILES_WIDTH, LISTBOX_FILES_HEIGHT, hwnd,
                                                (HMENU)ID_LIST_BOX_FILES, instance, NULL);
            SendMessage(hListBoxFiles, WM_SETFONT,(WPARAM)hListsFont, 0);
            ShowWindow(hListBoxFiles, SW_HIDE);

        /********************************************************************/
        /**                         ONGLET MODIFIER                        **/
        /********************************************************************/
            /**     Creation de la list box contenant les actions   **/
            hListBoxActions = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTBOX, "", LBS_NOTIFY | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL,
                                            LISTBOX_ACTIONS_POSX, LISTBOX_ACTIONS_POSY, LISTBOX_ACTIONS_WIDTH, LISTBOX_ACTIONS_HEIGHT, hwnd,
                                                (HMENU)ID_LIST_BOX_ACTIONS, instance, NULL);
            SendMessage(hListBoxActions, WM_SETFONT,(WPARAM)hListsFont, 0);
            ShowWindow(hListBoxActions, SW_HIDE);

            /**     Creation du bouton créant et supprimant une action dans la list box  **/
            btnNewAction.create(BS_OWNERDRAW, hwnd, instance, ID_BTN_NEW_ACTION, BTN_NEW_ACTION_POSX, BTN_NEW_ACTION_POSY, BTN_NEW_ACTION_WIDTH, BTN_NEW_ACTION_HEIGHT);
            btnNewAction.setText(TXT_BTN_NEW_ACTION_DEFAULT);
            btnNewAction.hide();    /*  bouton caché a la creation car dans l'onglet "modifier" */

            btnDeleteAction.create(BS_ICON | BS_OWNERDRAW, hwnd, instance, ID_BTN_DELETE_ACTION, BTN_DELETE_ACTION_POSX, BTN_DELETE_ACTION_POSY, BTN_DELETE_ACTION_WIDTH, BTN_DELETE_ACTION_HEIGHT);
            HICON hBtnIco = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON_DELETE_ACTION), IMAGE_ICON, BTN_DELETE_FILE_ICO_WIDTH, BTN_DELETE_FILE_ICO_HEIGHT, 0);
            btnDeleteAction.setIco((LPARAM)hBtnIco);//  setIco pour btn owner draw : Wm_GETICON
            btnDeleteAction.hide();    /*  bouton caché a la creation car dans l'onglet "modifier" */

            /**     Creation des spin box X Y et edit Time   **/
            sbActionPosX.create(5, hwnd, instance, SB_ACTIONX_POSX, SB_ACTIONX_POSY, SB_ACTIONX_WIDTH, SB_ACTIONX_HEIGHT, SB_UPDOWN_WIDTH, SB_UPDOWN_HEIGHT, ID_HWND_EDIT_ACTION);
            sbActionPosX.setRange(UD_MINVAL, UD_MAXVAL);
            sbActionPosX.hide();

            sbActionPosY.create(5, hwnd, instance, SB_ACTIONY_POSX, SB_ACTIONY_POSY, SB_ACTIONY_WIDTH, SB_ACTIONY_HEIGHT, SB_UPDOWN_WIDTH, SB_UPDOWN_HEIGHT, ID_HWND_EDIT_ACTION);
            sbActionPosY.setRange(UD_MINVAL, UD_MAXVAL);
            sbActionPosY.hide();

            hActionTime = CreateWindowEx(WS_EX_TRANSPARENT, "EDIT", "0", WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_BORDER,
                                          EDIT_TIME_POSX, EDIT_TIME_POSY, EDIT_TIME_WIDTH, EDIT_TIME_HEIGHT,
                                          hwnd, (HMENU)ID_HWND_EDIT_ACTION, instance, NULL);
            SendMessage(hActionTime, EM_LIMITTEXT, 10, 0);//    Limite du nombre de chiffre à 4 294 967 296 -> 2^32
            ShowWindow(hActionTime, SW_HIDE);

            /**     Creation et remplissage du combo box de commandes  **/
            cbCommand.create(hwnd, instance, CB_COMMAND_POSX, CB_COMMAND_POSY, CB_COMMAND_WIDTH, CB_COMMAND_HEIGHT, ID_HWND_EDIT_ACTION);

            for(int i(1); i < NBR_COMMAND+1; i++)   //  On décale l'incrém car les commandes sont décalés
                cbCommand.addLine(cmd.getDescByCommand(i));
            cbCommand.setPosition(0);
            cbCommand.hide();

        /********************************************************************/
        /**                         ONGLET PARAMETRES                      **/
        /********************************************************************/
            /**     Creation du texte expliquatif pour assigner une nouvelle touche **/
            hTextEditKey = CreateWindowEx(WS_EX_TRANSPARENT, "STATIC", TEXT_EDIT_KEY, WS_CHILD | WS_VISIBLE,
                                          TEXT_EDIT_KEY_POSX, TEXT_EDIT_KEY_POSY, TEXT_EDIT_KEY_WIDTH, TEXT_EDIT_KEY_HEIGHT,
                                          hwnd, (HMENU)ID_TEXT_EDIT_KEY, instance, NULL);
            ShowWindow(hTextEditKey, SW_HIDE);
            SendMessage(hTextEditKey, WM_SETFONT,(WPARAM)hListsFont,0);// Modifie la police


            /**     Creation du combo box de l'action a laquelle asocier la touche  **/
            cbCommandKey.create(hwnd, instance, CB_COMMAND_KEY_POSX, CB_COMMAND_KEY_POSY, CB_COMMAND_KEY_WIDTH, CB_COMMAND_KEY_HEIGHT, 0);
            cbCommandKey.hide();

            /**     Creation du bouton pour éditer une touche    **/
            btnEditKey.create(BS_OWNERDRAW, hwnd, instance, ID_BTN_EDIT_KEY, BTN_EDIT_KEY_POSX, BTN_EDIT_KEY_POSY, BTN_EDIT_KEY_WIDTH, BTN_EDIT_KEY_HEIGHT);
            btnEditKey.setText(TXT_BTN_EDIT_KEY_DEFAULT);
            btnEditKey.hide();

            /**     Sous classe les spin boxes  **/
            //oldSpinBoxesProc = sbActionPosX.setWndProc((LONG_PTR)spinBoxesProc);

            /**     Initialisation  **/
            fillKeyComboBox(0);//   On remplis la combobox des touches asociés aux actions
            fillKeyListBox();
            setWindowFont(true);
            createTrayIcon(hwnd);
            createMenu(hwnd);
            if(isLaunchedFromExe)// uniquement si bot clic est lancé depuis son exe, non depuis .BCSeq
            {
                //  Nouveau thread car manipule des fichiers
                CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&fillFileListBox, 0, 0, 0);//  On charge toutes les séquence du dossier \\Sequences
                readConfig(hwnd);// On initialise la config depuis le fichier
            }
            else//  Si lancé depuis fichier .BCSeq interdit de toucher aux fichiers
            {
                btnLoadFile.enable(false);
                btnSaveFile.enable(false);
                btnDeleteFile.enable(false);
            }

            return 0;
        }

        case WM_COMMAND:
        {
            listBoxesControl(hwnd, message, wParam, lParam);
            menusControl(hwnd, message, wParam, lParam);
            buttonsControl(hwnd, message, wParam, lParam);

            return 0;
        }

        case WM_ENTERMENULOOP:///    à l'affichage du menu "options" et des sous menu
        {
            std::ostringstream menuTxt;
            menuTxt << "Affecter ";
            menuTxt << getTimeEdit();
            menuTxt << " ms à toutes les actions";
            ModifyMenu(hOptionsMenu, ID_MENU_SET_SAME_TIME, MF_BYCOMMAND | MF_STRING, ID_MENU_SET_SAME_TIME, menuTxt.str().c_str());

            break;
        }

        case WM_KEYDOWN:
        {
            ///     Récupération d'une touche pour assigner à une action
            if(editKeyThreadRunning)//  Si le thread tourne : on doit récuperer la touche appuyée
            {
                newKeyCode = (uint8_t)wParam;    //  On attribue le code de la touche
                SetEvent(hNewKeyCodeAvailable); //  On signale la HANDLE pour dire qu'on a une nouvelle touche
            }
            break;
        }

        case WM_HOTKEY:
        {
            switch(LOWORD(wParam))
            {
                ///     SHOW WINDOW
                case ID_HOT_KEY_SHOW_WINDOW://  Ctrl+Maj+B
                {
                    #ifdef DEBUG
                        std::cout << "Show window hot key" << std::endl;
                    #endif // DEBUG
                    SetForegroundWindow(hwnd);
                    ShowWindow(hwnd, SW_RESTORE);
                    FlashWindow(hwnd, true);
                    //SetFocus(hwnd);
                    break;
                }
                ///     START SEQ
                case ID_HOT_KEY_START_SEQ://  Ctrl+Maj+L
                {
                    #ifdef DEBUG
                        std::cout << "Lunch seq hot key" << std::endl;
                    #endif // DEBUG
                    btnStartSeq.clic();
                    break;
                }
                ///     CREATE SEQ
                case ID_HOT_KEY_CREATE_SEQ://   Ctrl+Alt+S
                {
                    #ifdef DEBUG
                        std::cout << "Create seq hot key" << std::endl;
                    #endif // DEBUG
                    btnCreateSeq.clic();
                    break;
                }
            }
            break;
        }

        case WM_DROPFILES:
        {
            if(!isLaunchedFromExe)//    Si il 'nest pas lancé depuis l'exe on ne fait pas de drag & drop
                break;

            int nbrDropedFile(0);
            // avec 0xFFFFFFFF comme index de fichier, DragQueryFile retourne le nombre de fichier dispo
            nbrDropedFile = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0);

            //  On copie tout les fichiers
            for(int i(0); i < nbrDropedFile; i++)
            {
                //  Si le buffer est NULL, DragQueryFile renvoi
                int filePathSize = DragQueryFile((HDROP)wParam, i, NULL, 0);
                char filePath[filePathSize+1];//  On crée un buffer de la taille exacte du nom du fichier
                // le null-terminated string est rajouté par la fontcion
                DragQueryFile((HDROP)wParam, i, filePath, filePathSize+1);
                //   Si le fichier ç la bonne extension on le copie et on actualise la listbox des fichiers
                int backSlash(0);
                for(int i(0); i < filePathSize; i++)
                {
                    if(filePath[i] == '\\')
                        backSlash++;
                }

                std::string fileName;
                fileName = fileNameByPath(filePath);//  On récupère le nom du fichier
                if(isBCSeq(fileName.c_str()))
                {
                    //  On crée l'emplacement du nouveau fichier
                    std::string newFilePath;
                    newFilePath += SEQUENCES_FOLDER_NAME;
                    newFilePath += fileName;
                    /**     On gère les erreur de copie du fichier  **/
                    if(!CopyFile(filePath, newFilePath.c_str(), true))
                    {
                        //  On gère le cas où le fichier existe déjà
                        if(GetLastError() == ERROR_FILE_EXISTS) //  80
                        {
                            #ifdef DEBUG
                                std::cout << "File existing : " << newFilePath << std::endl;
                            #endif // DEBUG
                            SetForegroundWindow(hwnd);
                            if(MessageBox(hwnd, "Un des fichiers sélectionné existe déjà, voulez vous le remplacer", "Fichier existant", MB_YESNO) == IDYES)
                            {
                                //  On le copie mais cette fois on autorise la réecriture du fichier
                                CopyFile(filePath, newFilePath.c_str(), false);
                                #ifdef DEBUG
                                    std::cout << "File Overwritten" << std::endl;
                                #endif // DEBUG
                            }
                        }
                        else
                        {
                            //  Sinon on affiche l'erreur
                            #ifdef DEBUG
                            std::cout << "Unable to copy files, error : " << GetLastError() << std::endl;
                            #endif // DEBUG

                            std::ostringstream errorCode;
                            errorCode << "Impossible de copier les fichiers\nCode d'erreur : ";
                            errorCode << GetLastError();

                            SetForegroundWindow(hwnd);
                            MessageBox(hwnd, errorCode.str().c_str(), "Erreur de copie", MB_OK);
                        }
                    }
                }
                else
                {
                    #ifdef DEBUG
                        std::cout << "Bad extention" << std::endl;
                    #endif // DEBUG
                    SetForegroundWindow(hwnd);
                    MessageBox(hwnd, "Veuillez sélectionner uniquement des fichiers avec l'extension \".BCSeq\"", "Mauvaise extension", MB_OK);
                }
            }
            fillFileListBox();//    On actualise la list box des fichiers

            DragFinish((HDROP)wParam);  //  On libère la mémoire
            break;
        }

        case WM_DRAWITEM:
        {
            //  S'il s'agit du bouton de lancement de séquence
            if(wParam == ID_BTN_START_SEQ)
            {
                //  On récupère draw item struct permettant de dessiner
                LPDRAWITEMSTRUCT lpdiControl = (LPDRAWITEMSTRUCT)lParam;

                //  On récupère la zone cliente du bouton
                RECT rcClient;
                GetClientRect(lpdiControl->hwndItem, &rcClient);

                //  On crée la région du bouton à partire rcClient
                //HRGN hBtnRgn = CreateEllipticRgnIndirect(&rcClient);
                HRGN hBtnRgn = CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 5, 5);
                //   On remplis le fond du bouton           (1 + COLOR_BTNFACE)
                //FillRect(lpdiControl->hDC, &lpdiControl->rcItem, (HBRUSH)CreateSolidBrush(RGB(100, 0, 255)));

                /// ETAT APPUYÉ DU BOUTON
                if(lpdiControl->itemState & ODS_SELECTED)// Pas de ==
                {
                    //  On crée la brush dégradé pour remplir le bouton         RGB(100, 0, 255)
                    HBRUSH hPuchedBtnBrush = CreateGradientBrush(RGB(0, 10, 80), RGB(125, 0, 70), lpdiControl);
                    //  On remplis la région avec la brush crée
                    FillRgn(lpdiControl->hDC, hBtnRgn, hPuchedBtnBrush);
                    //  On attribut la couleur correspondante à l'état du bouton a utexte
                    SetTextColor(lpdiControl->hDC, COLOR_TXT_BTN_START_SEQ_PUSHED);
                    DeleteObject(hPuchedBtnBrush);//    On libère la mémoire
                }
                /// ETAT DÉSACTIVÉ DU BOUTON
                else if(lpdiControl->itemState & ODS_DISABLED)//    Censé marcher avec ODS_HOTLIGHT
                {
                    HBRUSH hDisabledBtnBrush = CreateGradientBrush(RGB(100, 100, 100), RGB(0, 0, 0), lpdiControl);
                    //  On remplis la région avec la brush crée
                    FillRgn(lpdiControl->hDC, hBtnRgn, hDisabledBtnBrush);
                    //  On attribut la couleur correspondante à l'état du bouton a utexte
                    SetTextColor(lpdiControl->hDC, COLOR_TXT_BTN_START_SEQ_DISABLED);
                    DeleteObject(hDisabledBtnBrush);//    On libère la mémoire
                }
                /// ETAT NORMAL DU BOUTON
                else
                {
                    //  On crée la brush dégradé pour remplir le bouton
                    HBRUSH hNormalBtnBrush = CreateGradientBrush(RGB(125, 0, 70), RGB(0, 10, 80), lpdiControl);
                    //  On remplis la région avec la brush crée
                    FillRgn(lpdiControl->hDC, hBtnRgn, hNormalBtnBrush);
                    //  On attribut la couleur correspondante à l'état du bouton a utexte
                    SetTextColor(lpdiControl->hDC, COLOR_TXT_BTN_START_SEQ_RELEASED);
                    DeleteObject(hNormalBtnBrush);//    On libère la mémoire
                }
                /// Gestion du texte du bouton
                //  On récupère la taille du texte
                int textLen = (GetWindowTextLength(lpdiControl->hwndItem)+1);
                //char szText[textLen];// La place pour \0 est comprise dans textLen
                char* szText = new char[textLen];
                GetWindowText(lpdiControl->hwndItem, szText, textLen);
                SetBkMode(lpdiControl->hDC, TRANSPARENT);
                //SetTextColor(lpdiControl->hDC, COLOR_TXT_BTN_START_SEQ);
                DrawText(lpdiControl->hDC, szText, textLen, &rcClient, (DT_SINGLELINE | DT_CENTER | DT_VCENTER));

                SelectClipRgn(lpdiControl->hDC, NULL);
                //    On libère la mémoire
                DeleteObject(hBtnRgn);
                DeleteObject(lpdiControl);
                delete(szText);
            }

            /**********************     CLASSIC BUTTONS OWNER DRAWN     ********************/
            else if(wParam == ID_BTN_NEW_SEQ || wParam == ID_BTN_LOAD_FILE || wParam == ID_BTN_NEW_ACTION || wParam == ID_BTN_EDIT_KEY || wParam == ID_BTN_SAVE_FILE || wParam == ID_BTN_DELETE_FILE || wParam == ID_BTN_SIMULATE_SEQ || wParam == ID_BTN_DELETE_ACTION)
            {
                LPDRAWITEMSTRUCT lpdiControl = (LPDRAWITEMSTRUCT)lParam;

                RECT rcClient;
                GetClientRect(lpdiControl->hwndItem, &rcClient);

                HRGN hBtnRgn = CreateRectRgnIndirect(&rcClient);
                /// ETAT APPUYÉ DU BOUTON
                if(lpdiControl->itemState & ODS_SELECTED)// Pas de ==
                {
                    HBRUSH hPushedBtnBrush = CreateGradientBrush(COLOR_CLASSIC_PUSHED_BTN_TOP, COLOR_CLASSIC_PUSHED_BTN_BTM, lpdiControl);
                    HBRUSH hPushedBtnFrameBrush = CreateSolidBrush(COLOR_CLASSIC_PUSHED_FRAME_BTN);
                    SetTextColor(lpdiControl->hDC, COLOR_CLASSIC_TXT_PUSHED_BTN);

                    FillRgn(lpdiControl->hDC, hBtnRgn, hPushedBtnBrush);
                    FrameRgn(lpdiControl->hDC, hBtnRgn, hPushedBtnFrameBrush, 2, 2);

                    DeleteObject(hPushedBtnBrush);
                    DeleteObject(hPushedBtnFrameBrush);
                }
                /// ETAT DÉSACTIVÉ DU BOUTON
                else if(lpdiControl->itemState & ODS_DISABLED)//    Censé marcher avec ODS_HOTLIGHT
                {
                    HBRUSH hDisabledBtnBrush = CreateGradientBrush(COLOR_CLASSIC_DISABLED_BTN_TOP, COLOR_CLASSIC_DISABLED_BTN_BTM, lpdiControl);
                    HBRUSH hDisabledBtnFrameBrush = CreateSolidBrush(COLOR_CLASSIC_DISABLED_FRAME_BTN);
                    SetTextColor(lpdiControl->hDC, COLOR_CLASSIC_TXT_DISABLED_BTN);

                    FillRgn(lpdiControl->hDC, hBtnRgn, hDisabledBtnBrush);
                    FrameRgn(lpdiControl->hDC, hBtnRgn, hDisabledBtnFrameBrush, 2, 2);

                    DeleteObject(hDisabledBtnBrush);
                    DeleteObject(hDisabledBtnFrameBrush);
                }
                /// ETAT NORMAL DU BOUTON
                else
                {
                    HBRUSH hBtnBrush = CreateGradientBrush(COLOR_CLASSIC_NORMAL_BTN_TOP, COLOR_CLASSIC_NORMAL_BTN_BTM, lpdiControl);
                    HBRUSH hBtnFrameBrush = CreateSolidBrush(COLOR_CLASSIC_NORMAL_FRAME_BTN);
                    SetTextColor(lpdiControl->hDC, COLOR_CLASSIC_TXT_NORMAL_BTN);

                    FillRgn(lpdiControl->hDC, hBtnRgn, hBtnBrush);
                    FrameRgn(lpdiControl->hDC, hBtnRgn, hBtnFrameBrush, 2, 2);

                    DeleteObject(hBtnBrush);
                    DeleteObject(hBtnFrameBrush);
                }
                /// Affichage de l'icone
                HICON hBtnIcon = (HICON)SendMessage(lpdiControl->hwndItem, WM_GETICON, ICON_BIG, 0);
                if(hBtnIcon != NULL)//  Le bouton à une icone asocié
                    DrawIconEx(lpdiControl->hDC, 7, 2,hBtnIcon, BTN_DELETE_FILE_ICO_WIDTH, BTN_DELETE_FILE_ICO_HEIGHT, 0, NULL, DI_NORMAL);

                /// Affichage du texte
                int textLen = (GetWindowTextLength(lpdiControl->hwndItem)+1);
                //char szText[textLen];// La place pour \0 est comprise dans textLen
                char* szText = new char[textLen];
                GetWindowText(lpdiControl->hwndItem, szText, textLen);
                SetBkMode(lpdiControl->hDC, TRANSPARENT);

                RECT newRect;
                RECT tempRect;//    sert uniquement comme paramètre a DrawText, inutilisé
                newRect.bottom = DrawText(lpdiControl->hDC, szText, textLen, &tempRect, DT_CALCRECT);// Renvoie la hauteur du texte a dessiner

                rcClient.top = rcClient.bottom/2 - newRect.bottom/2;//  On calcule la posisition en X du texte pour qu'il soit centré

                DrawText(lpdiControl->hDC, szText, textLen, &rcClient, DT_CENTER | DT_VCENTER);
                SelectClipRgn(lpdiControl->hDC, NULL);
                //    On libère la mémoire
                DeleteObject(hBtnRgn);
                DeleteObject(lpdiControl);
                delete(szText);
            }

            return true;
        }

        case WM_NOTIFY:
        {
            LPNMHDR pnmhdr = (LPNMHDR)lParam;
            /** ONGLET CREER   **/
            if(pnmhdr->code == TCN_SELCHANGE && TabCtrl_GetCurSel(hMainTab) == 0)
            {
                /*  Onglet Créer    */
                btnCreateSeq.show();
                ShowWindow(hTextCreationSeq, SW_SHOW);
                ShowWindow(hListBoxKey, SW_SHOW);

                /*  Onglet Charger */
                btnLoadFile.hide();
                btnSaveFile.hide();
                btnDeleteFile.hide();
                ShowWindow(hListBoxFiles, SW_HIDE);

                /*  Onglet Modifier */
                btnNewAction.hide();
                btnDeleteAction.hide();
                ShowWindow(hListBoxActions, SW_HIDE);

                sbActionPosX.hide();
                sbActionPosY.hide();
                ShowWindow(hActionTime, SW_HIDE);
                cbCommand.hide();

                /*  Onglet Options  */
                ShowWindow(hTextEditKey, SW_HIDE);
                cbCommandKey.hide();
                btnEditKey.hide();
            }
            /** ONGLET CHARGER **/
            if(pnmhdr->code == TCN_SELCHANGE && TabCtrl_GetCurSel(hMainTab) == 1)
            {
                /*  Onglet Créer    */
                btnCreateSeq.hide();
                ShowWindow(hTextCreationSeq, SW_HIDE);
                ShowWindow(hListBoxKey, SW_HIDE);

                /*  Onglet Charger */
                btnLoadFile.show();
                btnSaveFile.show();
                btnDeleteFile.show();
                ShowWindow(hListBoxFiles, SW_SHOW);

                /*  Onglet Modifier */
                btnNewAction.hide();
                btnDeleteAction.hide();
                ShowWindow(hListBoxActions, SW_HIDE);
                sbActionPosX.hide();
                sbActionPosY.hide();
                ShowWindow(hActionTime, SW_HIDE);
                cbCommand.hide();

                /*  Onglet Options  */
                ShowWindow(hTextEditKey, SW_HIDE);
                cbCommandKey.hide();
                btnEditKey.hide();
            }
            /** ONGLET MODIFIER    **/
            if(pnmhdr->code == TCN_SELCHANGE && TabCtrl_GetCurSel(hMainTab) == 2)
            {
                /*  Onglet Créer    */
                btnCreateSeq.hide();
                ShowWindow(hTextCreationSeq, SW_HIDE);
                ShowWindow(hListBoxKey, SW_HIDE);

                /*  Onglet Charger */
                btnLoadFile.hide();
                btnSaveFile.hide();
                btnDeleteFile.hide();
                ShowWindow(hListBoxFiles, SW_HIDE);

                /*  Onglet Modifier */
                btnNewAction.show();
                btnDeleteAction.show();
                ShowWindow(hListBoxActions, SW_SHOW);
                sbActionPosX.show();
                sbActionPosY.show();
                ShowWindow(hActionTime, SW_SHOW);
                cbCommand.show();

                /*  Onglet Options  */
                ShowWindow(hTextEditKey, SW_HIDE);
                cbCommandKey.hide();
                btnEditKey.hide();
            }
            /** ONGLET OPTIONS **/
            if(pnmhdr->code == TCN_SELCHANGE && TabCtrl_GetCurSel(hMainTab) == 3)
            {
                /*  Onglet Créer    */
                btnCreateSeq.hide();
                ShowWindow(hTextCreationSeq, SW_HIDE);
                ShowWindow(hListBoxKey, SW_HIDE);

                /*  Onglet Charger */
                btnLoadFile.hide();
                btnSaveFile.hide();
                btnDeleteFile.hide();
                ShowWindow(hListBoxFiles, SW_HIDE);

                /*  Onglet Modifier */
                btnNewAction.hide();
                btnDeleteAction.hide();
                ShowWindow(hListBoxActions, SW_HIDE);
                sbActionPosX.hide();
                sbActionPosY.hide();
                ShowWindow(hActionTime, SW_HIDE);
                cbCommand.hide();

                /*  Onglet Options  */
                ShowWindow(hTextEditKey, SW_SHOW);
                cbCommandKey.show();
                btnEditKey.show();
            }
            return 0;
        }

        case WM_SIZE:
        {
            /*if(wParam == SIZE_RESTORED)//   uniquement lorsque la taille de la fenetre change
            {
                double widthWndRatio = (double)LOWORD(lParam)/WINDOW_WIDTH;
                double heightWndRatio = (double)HIWORD(lParam)/WINDOW_HEIGHT;
                #ifdef DEBUG
                    std::cout << "Window ratio : " << widthWndRatio << std::endl;
                #endif // DEBUG
                btnStartSeq.move(BTN_START_SEQ_POSX*widthWndRatio, BTN_START_SEQ_POSY*heightWndRatio, BTN_START_SEQ_WIDTH*widthWndRatio, BTN_START_SEQ_HEIGHT*heightWndRatio);
                btnSimulateSeq.move(BTN_SIMULATE_SEQ_POSX*widthWndRatio, BTN_SIMULATE_SEQ_POSY*heightWndRatio, BTN_SIMULATE_SEQ_WIDTH*widthWndRatio, BTN_SIMULATE_SEQ_HEIGHT*heightWndRatio);
                MoveWindow(hPBarSequence ,PBBAR_POSX*widthWndRatio, PBBAR_POSY*heightWndRatio, PBBAR_WIDTH*widthWndRatio, PBBAR_HEIGHT*heightWndRatio, true);
                MoveWindow(hMainTab ,TAB_POSX*widthWndRatio, TAB_POSY*heightWndRatio, TAB_WIDTH*widthWndRatio, TAB_HEIGHT*heightWndRatio, true);

                //  Onglet Créer
                btnCreateSeq.move(BTN_CREATE_SEQ_POSX*widthWndRatio, BTN_CREATE_SEQ_POSY*heightWndRatio, BTN_CREATE_SEQ_WIDTH*widthWndRatio, BTN_CREATE_SEQ_HEIGHT*heightWndRatio);
                MoveWindow(hTextCreationSeq, TEXT_CREATION_SEQ_POSX*widthWndRatio, TEXT_CREATION_SEQ_POSY*heightWndRatio, TEXT_CREATION_SEQ_WIDTH*widthWndRatio, TEXT_CREATION_SEQ_HEIGHT*heightWndRatio, true);
                MoveWindow(hListBoxKey, LISTBOX_KEY_POSX*widthWndRatio, LISTBOX_KEY_POSY*heightWndRatio, LISTBOX_KEY_WIDTH*widthWndRatio, LISTBOX_KEY_HEIGHT*heightWndRatio, true);
                //  Onglet Charger
                btnLoadFile.move(BTN_LOAD_FILE_POSX*widthWndRatio, BTN_LOAD_FILE_POSY*heightWndRatio, BTN_LOAD_FILE_WIDTH*widthWndRatio, BTN_LOAD_FILE_HEIGHT*heightWndRatio);
                btnSaveFile.move(BTN_SAVE_FILE_POSX*widthWndRatio, BTN_SAVE_FILE_POSY*heightWndRatio, BTN_SAVE_FILE_WIDTH*widthWndRatio, BTN_SAVE_FILE_HEIGHT*heightWndRatio);
                btnDeleteFile.move(BTN_DELETE_FILE_POSX*widthWndRatio, BTN_DELETE_FILE_POSY*heightWndRatio, BTN_DELETE_FILE_WIDTH*widthWndRatio, BTN_DELETE_FILE_HEIGHT*heightWndRatio);
                MoveWindow(hListBoxFiles, LISTBOX_FILES_POSX*widthWndRatio, LISTBOX_FILES_POSY*heightWndRatio, LISTBOX_FILES_WIDTH*widthWndRatio, LISTBOX_FILES_HEIGHT*heightWndRatio, true);
                //  Onglet Modifier
                MoveWindow(hListBoxActions, LISTBOX_ACTIONS_POSX*widthWndRatio, LISTBOX_ACTIONS_POSY*heightWndRatio, LISTBOX_ACTIONS_WIDTH*widthWndRatio, LISTBOX_ACTIONS_HEIGHT*heightWndRatio, true);
                btnNewAction.move(BTN_NEW_ACTION_POSX*widthWndRatio, BTN_NEW_ACTION_POSY*heightWndRatio, BTN_NEW_ACTION_WIDTH*widthWndRatio, BTN_NEW_ACTION_HEIGHT*heightWndRatio);
                btnDeleteAction.move(BTN_DELETE_ACTION_POSX*widthWndRatio, BTN_DELETE_ACTION_POSY*heightWndRatio, BTN_DELETE_ACTION_WIDTH*widthWndRatio, BTN_DELETE_ACTION_HEIGHT*heightWndRatio);
                sbActionPosX.move(SB_ACTIONX_POSX*widthWndRatio, SB_ACTIONX_POSY*heightWndRatio, SB_ACTIONX_WIDTH*widthWndRatio, SB_ACTIONX_HEIGHT*heightWndRatio, SB_UPDOWN_WIDTH*widthWndRatio, SB_UPDOWN_HEIGHT*heightWndRatio);
                sbActionPosY.move(SB_ACTIONY_POSX*widthWndRatio, SB_ACTIONY_POSY*heightWndRatio, SB_ACTIONY_WIDTH*widthWndRatio, SB_ACTIONY_HEIGHT*heightWndRatio, SB_UPDOWN_WIDTH*widthWndRatio, SB_UPDOWN_HEIGHT*heightWndRatio);
                MoveWindow(hActionTime, EDIT_TIME_POSX*widthWndRatio, EDIT_TIME_POSY*heightWndRatio, EDIT_TIME_WIDTH*widthWndRatio, EDIT_TIME_HEIGHT*heightWndRatio, true);
                cbCommand.move(CB_COMMAND_POSX*widthWndRatio, CB_COMMAND_POSY*heightWndRatio, CB_COMMAND_WIDTH*widthWndRatio, CB_COMMAND_HEIGHT*heightWndRatio);
                // Onglet paramètres

            }*/
            botClic.statusBarProc(lParam); //  On modifie sa position à la creation de la fenetre
            break;
        }

        /**     Couleur du texte    **/
        case WM_CTLCOLORSTATIC:
        {
            uint16_t hwndID = GetWindowLong((HWND)lParam, GWL_ID);
            if(hwndID == ID_TEXT_CREATION_SEQ || hwndID == ID_TEXT_EDIT_KEY)
            {

                SetBkMode((HDC) wParam, TRANSPARENT);//  On modifie la couleur de fond du texte
                SetTextColor((HDC) wParam, RGB(0, 0, 150)); // On modifie la couleur du texte
                return (BOOL) GetStockObject(NULL_BRUSH);
            }
            return 0;
        }

        case WM_CLOSE:
        {
            if(isLaunchedFromExe)// uniquement si lancé depuis exe, non depuis seq file
                writeConfig();//    On sauvegarde la config (menus et séquence chargé)

            UnregisterHotKey(hwnd, ID_HOT_KEY_SHOW_WINDOW);
            UnregisterHotKey(hwnd, ID_HOT_KEY_START_SEQ);
            UnregisterHotKey(hwnd, ID_HOT_KEY_CREATE_SEQ);

            DestroyWindow(hwnd);
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
            return 0;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
