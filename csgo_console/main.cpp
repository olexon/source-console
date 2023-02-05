#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <psapi.h>

using namespace std;

unsigned int csgo_instances, skip_command;
string command_input;

vector<DWORD> pids = {};
vector<HWND> wnd_handles = {};
vector<HWND> wnd_classess = {};
vector<string> wnd_class_names = {};

void GetAllProcessess( DWORD processID )
{
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    // Get a handle to the process.

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, processID );

    // Get the process name.

    if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
             &cbNeeded) )
        {
            GetModuleBaseName( hProcess, hMod, szProcessName, 
                               sizeof(szProcessName)/sizeof(TCHAR) );
        }
    }

    // Print the process name and identifier.

    //cout << szProcessName << " " << "(PID: " << processID << ")" << endl;

    // Release the handle to the process.
    if (!stricmp(szProcessName, "csgo.exe")) 
    {
        csgo_instances++;
        pids.push_back(processID);
    } 
    
    CloseHandle(hProcess);
}

void CheckProcessess()
{
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        return;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    for ( i = 0; i < cProcesses; i++ )
    {
        if( aProcesses[i] != 0 )
        {
            GetAllProcessess( aProcesses[i] );
        }
    }

    if (csgo_instances <= 0) 
    {
        cout << "CSGO not found!" << endl;
        system("pause >nul");
        exit(0);
    }
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    char class_name[80];
    DWORD lpdwProcessId;

    GetWindowThreadProcessId(hwnd, &lpdwProcessId);

    if(lpdwProcessId==lParam)
    {
        GetClassName(hwnd, class_name, sizeof(class_name));
        string dupa(class_name);

        if(dupa.find("Valve001") != string::npos)
        {
            wnd_class_names.push_back(class_name);
            wnd_handles.push_back(hwnd);

            return FALSE;
        }
    }
    return TRUE;
}

void GetWindowInfo()
{
    for(int i = 0; i <= pids.size() - 1; i++) //мечет
    {
        EnumWindows(EnumWindowsProc, pids[i]);

        char cName[MAX_PATH];
        GetWindowText(wnd_handles[i], cName, sizeof(cName));

        //TCHAR class_name[MAX_PATH];
        //GetClassName(wnd_handles[i], class_name, _countof(class_name));

        //wnd_class_names.push_back(class_name); //for debugging

        HWND window_class_handle = FindWindow(wnd_class_names[i].c_str(), cName);

        wnd_classess.push_back(window_class_handle);

        CloseHandle(wnd_handles[i]);
    }
}

void SendCommand(HWND m_hEngine, const char* m_pTestCommand) {
    COPYDATASTRUCT m_cData;

    m_cData.cbData = strlen(m_pTestCommand) + 1;
    m_cData.dwData = 0;
    m_cData.lpData = (void*)m_pTestCommand;

    SendMessageA(m_hEngine, WM_COPYDATA, 0, (LPARAM)&m_cData);
}

int main() {
    SetConsoleTitleA("CSGO Console");

    CheckProcessess();

    cout << "CSGO instances: " << csgo_instances << endl;

    GetWindowInfo();

    for(;;)
    {
        cout << "Command -> "; getline(cin, command_input);

        if(!stricmp(command_input.c_str(), "clear"))
        {
            system("cls");
        }
        else if(!stricmp(command_input.c_str(), "--print-handles"))
        {
            for(auto i = begin(wnd_handles); i != end(wnd_handles); i++)
            {
                cout << hex << "0x" << *i << dec << endl;
            }

            skip_command++;
        }
        else if(!stricmp(command_input.c_str(), "--print-window-names"))
        {
            for(auto i = begin(wnd_class_names); i != end(wnd_class_names); i++)
            {
                cout << *i << endl;
            }

            skip_command++;
        }

        for(int i = 0; i <= pids.size() - 1; i++) //мечет
        {
            if(skip_command)
            {
                skip_command = 0;
                break;
            }

            SendCommand(wnd_classess[i], command_input.c_str());
        }
    }

    return 0;
}