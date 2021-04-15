#include "Command.hpp"


using namespace std;

Command::Command()
{

}

void Command::init(bool writeFile)
{
    initDescTab();
    initKeyTab(writeFile);
}

void Command::setCurPos(int posX, int posY)
{
    SetCursorPos(posX, posY);
}

int Command::getCurPosX()
{
    POINT pt;
    GetCursorPos(&pt);
    return pt.x;
}

int Command::getCurPosY()
{
    POINT pt;
    GetCursorPos(&pt);
    return pt.y;
}
char* Command::getDescByCommand(unsigned int command)
{
    /*  Si commanc est compris entre la plus petite et la plus grande: elle existe   */
    if(command <= NBR_COMMAND+1 && command >= 0)
        return descriptionTab[command-1];   //  On retire 1 car BC_RIGHT_CLIC vaut 1 et est l'index 0 etc...
    else
        return (char*)"Commande inconnue";
}

int Command::getKeyByCommand(unsigned int command)
{
    /*  Si commanc est compris entre la plus petite et la plus grande: elle existe   */
    if(command <= NBR_COMMAND+1 && command >= 0)
        return keyTab[command-1];   //  On retire 1 car BC_RIGHT_CLIC vaut 1 et est l'index 0 etc...
    else
        return 0;   //  0 ne correspond a aucune touche. La 1er -> 0x01 -> Left mouse button
}

void Command::doCommand(unsigned int command)
{
    switch(command)
    {
        case BC_MOUSE_MOVE://   On ne fait rien en plus
            break;
        case BC_RIGHT_CLIC:
        {
            POINT pt;
            GetCursorPos(&pt);

            mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, 0);
            mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, 0);
            break;
        }
        case BC_LEFT_CLIC:
        {
            POINT pt;
            GetCursorPos(&pt);

            mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, 0);
            mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, 0);
            break;
        }
        case BC_COPY:
        {
            keybd_event(VK_LCONTROL,0,0,0);
            keybd_event(0x43,0,0,0);
            keybd_event(VK_LCONTROL,0,KEYEVENTF_KEYUP,0);
            keybd_event(0x43,0,KEYEVENTF_KEYUP,0);
            break;
        }
        case BC_PASTE:
        {
            keybd_event(VK_LCONTROL,0,0,0);
            keybd_event(0x56,0,0,0);
            keybd_event(VK_LCONTROL,0,KEYEVENTF_KEYUP,0);
            keybd_event(0x56,0,KEYEVENTF_KEYUP,0);
            break;
        }
        case BC_SELECT_ALL:
        {
            keybd_event(VK_LCONTROL,0,0,0);
            keybd_event(0x41,0,0,0);
            keybd_event(VK_LCONTROL,0,KEYEVENTF_KEYUP,0);
            keybd_event(0x41,0,KEYEVENTF_KEYUP,0);
            break;
        }
        case BC_CONTROL_TAB:
        {
            keybd_event(VK_LCONTROL,0,0,0);
            keybd_event(VK_TAB,0,0,0);
            keybd_event(VK_LCONTROL,0,KEYEVENTF_KEYUP,0);
            keybd_event(VK_TAB,0,KEYEVENTF_KEYUP,0);
            break;
        }
        case BC_ENTER:
        {
            keybd_event(VK_RETURN,0,0,0);
            keybd_event(VK_RETURN,0,KEYEVENTF_KEYUP,0);
            break;
        }
        case BC_LEFT_CLIC_DOWN:
        {
            POINT pt;
            GetCursorPos(&pt);
            mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, 0);
            break;
        }
        case BC_LEFT_CLIC_UP:
        {
            POINT pt;
            GetCursorPos(&pt);
            mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, 0);
            break;
        }
        case BC_RIGHT_CLIC_DOWN:
        {
            POINT pt;
            GetCursorPos(&pt);
            mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, 0);
            break;
        }
        case BC_RIGHT_CLIC_UP:
        {
            POINT pt;
            GetCursorPos(&pt);
            mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, 0);
            break;
        }
        case BC_DELETE:// suppr
        {
            keybd_event(VK_DELETE,0,0,0);
            keybd_event(VK_DELETE,0,KEYEVENTF_KEYUP,0);
            break;
        }

        default:
        {
            #ifdef DEBUG
                std::cout << "Commande inconnue : " << command << std::endl;
            #endif // DEBUG
        }
    }
}

bool Command::editKey(unsigned int command, uint8_t key, bool writeFile)
{
    //  Si la commande est comprise entre 1 est NBR_COMMAND, elle est OK
    if(command >= 1 && command <= NBR_COMMAND)
    {
        keyTab[command-1] = key;    //  On modifie la touche de la commande voulu

        if(writeFile)
        {
            std::string keyTabFilePath;
            keyTabFilePath += SOURCE_FOLDER_NAME;
            keyTabFilePath += KEYTAB_FILE_NAME;
            std::ofstream fluxOut(keyTabFilePath.c_str());
            for(int i(0); i < NBR_COMMAND; i++)//  On écrit toute les valeurs de keyTab dans le fichier
            {
                if(fluxOut.good())
                    fluxOut << (int)keyTab[i] << endl;
            }
        }

        return true;
    }
    else    //  Sinon la commande n'est pas valide
        return false;

}

/**     PROTECTED METHODES      **/
void Command::initDescTab()
{
    //  On initialise le tableau des descriptions
    descriptionTab[BC_MOUSE_MOVE-1]     = (char*)"Déplacement de souris";
    descriptionTab[BC_RIGHT_CLIC-1]     = (char*)"Clic droit";//    Cast non obligatoire
    descriptionTab[BC_LEFT_CLIC-1]      = (char*)"Clic gauche";
    descriptionTab[BC_COPY-1]           = (char*)"Ctrl + C (Copier)";
    descriptionTab[BC_PASTE-1]          = (char*)"Ctrl + V (Coller)";
    descriptionTab[BC_SELECT_ALL-1]     = (char*)"Ctrl + A (Tout sélectionner)";
    descriptionTab[BC_CONTROL_TAB-1]    = (char*)"Ctrl + Tab (Onglet suivant)";
    descriptionTab[BC_ENTER-1]          = (char*)"Entrer";
    descriptionTab[BC_LEFT_CLIC_DOWN-1] = (char*)"Clic gauche appuyé";
    descriptionTab[BC_LEFT_CLIC_UP-1]   = (char*)"Clic gauche relaché";
    descriptionTab[BC_RIGHT_CLIC_DOWN-1]= (char*)"Clic droit appuyé";
    descriptionTab[BC_RIGHT_CLIC_UP-1]  = (char*)"Clic droit relaché";
    descriptionTab[BC_DELETE-1]         = (char*)"Supprimer";
}

void Command::initKeyTab(bool writeFile)
{
    /**     Touche par deffaut  **/
    keyTab[BC_MOUSE_MOVE-1]     = 0x44;    //  "D"
    keyTab[BC_RIGHT_CLIC-1]     = 0x4F;    //  "O"
    keyTab[BC_LEFT_CLIC-1]      = 0x49;    //  "I"
    keyTab[BC_COPY-1]           = 0x43;    //  "C"
    keyTab[BC_PASTE-1]          = 0x56;    //  "V"
    keyTab[BC_SELECT_ALL-1]     = 0x41;    //  "A"
    keyTab[BC_CONTROL_TAB-1]    = 0x54;    //  "T"
    keyTab[BC_ENTER-1]          = 0x45;    //  "E"
    keyTab[BC_LEFT_CLIC_DOWN-1] = 0x55;    //  "U"
    keyTab[BC_LEFT_CLIC_UP-1]   = 0x4B;    //  "K"      u - i - o - p
    keyTab[BC_RIGHT_CLIC_DOWN-1]= 0x50;    //  "P"          k - l
    keyTab[BC_RIGHT_CLIC_UP-1]  = 0x4C;    //  "L"
    keyTab[BC_DELETE-1]         = 0x53;    //  "S"

    /**     On récupère les key code du fichier si autorisé **/
    if(writeFile)// uniquement si on a décidé d'écrir le fichier
    {
        std::string keyTabFilePath;
        keyTabFilePath += SOURCE_FOLDER_NAME;
        keyTabFilePath += KEYTAB_FILE_NAME;

        std::ifstream fluxIn(keyTabFilePath);

        if(!fluxIn)
        {
            #ifdef DEBUG
                cout << "Command::initKeyTab : Can't open keyTab.BC. Creation of the file" << endl;
            #endif // DEBUG
            /**     Le fichier n'a pas pu être ouvert : on le crée et le remplis   **/
            std::ofstream fluxOut(keyTabFilePath);
            //  On remplis le fichier avec les touches par déffaut
            fluxOut << (int)keyTab[0];//  On écrit le premier pour éviter le endl en trop
            for(int i(1); i < NBR_COMMAND; i++)
                fluxOut << endl << (int)keyTab[i];//    Cast en int pour éviter que des lettre soient écritent (uint8_t = char)

            return;//  On arrète la fonction car on ne relis pas le fichier après
        }
        else
        {
            #ifdef DEBUG
                std::cout << "keyTab.BC succesfully oppened" << endl;
            #endif // DEBUG
        }

        ///  On remplis keyTab depuis le fichier
        for(int i(0); i < NBR_COMMAND; i++)
        {
            //  On modifie la valeur de keyTab tant qu'il n'y a pas eofbit, failbit ou badbit
            if(fluxIn.good())
            {
                int newKey(0);
                fluxIn >> newKey;   //  Comparer newKey avec keyTab[i] ??
                keyTab[i] = (uint8_t)newKey;    //  On cast le int en uint8_t
            }
        }
    }
}
