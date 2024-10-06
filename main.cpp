#include "utils.h"

string input;

int main() 
{
#if defined(ENV64)
    SetConsoleTitleA("Source-Console");
#elif defined(ENV32)
    SetConsoleTitleA("Source-Console (32bit)");
    cout << "You are currently running 32 bit version of Source-Console, it might cause some weird issues." << endl;
#endif

    cout << "Compilation Date: " << __DATE__ << " " << __TIME__ << endl;

    utils::Config();

    cout << "Looking for: ";

    for (string& proc_name : utils::proc_names)
    {
        cout << proc_name << " ";
    }

    cout << endl;

    utils::Initialize();

    cout << "Instances: " << utils::Instances << endl;
    while (true)
    {
        cout << "> "; getline(cin, input);

        if (input == "clear")
            system("cls");

        utils::SendCommand(input.c_str());
    }

    return 0;
}