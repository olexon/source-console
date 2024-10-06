#include "utils.h"

vector<DWORD> pids = {};
vector<HWND> wnd_handles = {};
vector<HWND> wnd_classes = {};

vector<string> utils::proc_names = {};
unsigned int utils::Instances = 0;

void GetProcessIDs()
{
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    for (i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            char szProcessName[MAX_PATH];

            // Get a handle to the process.

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                PROCESS_VM_READ,
                FALSE, aProcesses[i]);

            // Get the process name.

            if (NULL != hProcess)
            {
                HMODULE hMod;
                DWORD cbNeeded;

                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
                    &cbNeeded))
                {
                    GetModuleBaseNameA(hProcess, hMod, szProcessName,
                        sizeof(szProcessName) / sizeof(char));
                }
            }

            // Print the process name and identifier.

            //cout << szProcessName << " " << "(PID: " << processID << ")" << endl;

            // Release the handle to the process.
            for (string& proc_name : utils::proc_names)
            {    
                if (!_stricmp(szProcessName, proc_name.c_str()))
                {
                    utils::Instances++;
                    pids.push_back(aProcesses[i]);
                }
            }

            CloseHandle(hProcess);
        }
    }

    if (utils::Instances <= 0)
    {
        cout << "No instances found!" << endl;
        system("pause");
        exit(-1);
    }
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    char class_name[80];
    DWORD lpdwProcessId;

    GetWindowThreadProcessId(hwnd, &lpdwProcessId);

    if (lpdwProcessId == lParam)
    {
        GetClassNameA(hwnd, class_name, sizeof(class_name));
        string _class(class_name);

        if (_class.find("Valve001") != string::npos)
        {
            wnd_handles.push_back(hwnd);

            return FALSE;
        }
    }
    return TRUE;
}

void utils::Initialize()
{
    GetProcessIDs();

    for (int i = 0; i <= pids.size() - 1; i++)
    {
        EnumWindows(EnumWindowsProc, pids[i]);

        char cName[MAX_PATH];
        GetWindowTextA(wnd_handles[i], cName, sizeof(cName));

        HWND window_class_handle = FindWindowA("Valve001", cName);

        wnd_classes.push_back(window_class_handle);

        CloseHandle(wnd_handles[i]);
    }
}

void utils::SendCommand(const char* m_pTestCommand) 
{
    COPYDATASTRUCT m_cData;

    m_cData.cbData = strlen(m_pTestCommand) + 1;
    m_cData.dwData = 0;
    m_cData.lpData = (void*)m_pTestCommand;

    for (HWND& wnd_class : wnd_classes)
    {
        SendMessageA(wnd_class, WM_COPYDATA, 0, (LPARAM)&m_cData);
    }
}

#define CONFIG_NAME "source_console.cfg"
void utils::Config()
{
    vector<string> TargetNames = { "tf_win64.exe\n", "tf.exe\n", "hl2.exe\n", "csgo.exe" };
    ifstream file(CONFIG_NAME);

    if (file.good()) 
    {
        string line;

        while (getline(file, line)) {
            proc_names.push_back(line);
        }

        file.close();
    }
    else {
        ofstream NewFile(CONFIG_NAME);

        if (NewFile.is_open()) 
        {
            for (string& TargetName : TargetNames)
            {
                NewFile << TargetName;
            }
            
            NewFile.close();
        }
        else 
        {
            cout << "Unable to create a new config file!" << endl;
            system("pause");
            exit(-2);
        }

        file.open(CONFIG_NAME);
        string line;

        if (file.good())
        {
            while (getline(file, line)) {
                proc_names.push_back(line);
            }
        }

        file.close();
    }
}