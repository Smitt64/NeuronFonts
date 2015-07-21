#include <iostream>
#include <comdef.h>
#include <Shlwapi.h>
#import "NeuronFonts.tlb" no_namespace

#pragma comment(lib,"Shlwapi.lib")

using namespace std;

void AttachAppPath(char *file, char *out, wchar_t *out_t)
{
    char memory[256] = {0};
    GetModuleFileNameA(GetModuleHandle(NULL), memory, (sizeof(memory)));
    PathRemoveFileSpecA(memory);
    sprintf(memory, "%s\\%s", memory, file);

    if (out)
    {
        strcpy(out, memory);
    }

    if (out_t)
    {
        mbstowcs(out_t, memory, 256);
    }
}

void CheckFile(INeuronFontsPtr &nf, const char *file)
{
    wchar_t *recfile = new wchar_t[strlen(file) + 1];
    memset(recfile, 0, (strlen(file) + 1) * sizeof(wchar_t));
    mbstowcs(recfile, file, 256);

    int id = -1;
    cout << endl <<  "Recognize file: " << file << endl;
    nf->recognizefile(recfile, &id);

    if (id != -1)
    {
        cout << "I think it is: " << id + 128 << " - " << (char)(id + 128) << endl;
        cout << "It's right? (1 - yes, 2 - no): ";

        short ans = 1;
        cin >> ans;

        if (ans == 2)
        {
            char ch = 0;
            cout << "Enter right answer: ";
            cin >> ch;

            int st = toupper(ch) - 128;
            cout << "Study symbol " << (char)toupper(ch) << endl;
            nf->study(&st);
        }
    }

    delete recfile;
}

int main(int argc, char **argv)
{
    HRESULT		hr;
    hr = CoInitialize(0);

    if(SUCCEEDED(hr))
    {
        INeuronFontsPtr pNf(__uuidof(NeuronFonts));

        if (!pNf)
        {
            cout << "INeuronFontsPtr failed." << endl;
            return -1;
        }

        wchar_t memfile[256] = {0};
        AttachAppPath("memory.db", NULL, memfile);
        pNf->MemoryFile = memfile;
        pNf->openMemory();

        char file[256] = {0};
        if (argc == 1)
        {
            AttachAppPath("sogoe.bmp", file, NULL);
            CheckFile(pNf, file);
        }
        else
        {
            for (int i = 1; i < argc; i++)
            {
                AttachAppPath(argv[i], file, NULL);
                CheckFile(pNf, file);
                memset(file, 0, sizeof(file));
            }
        }
    }
    else
    {
        cout << "CoInitialize failed." << endl;
    }

    CoUninitialize();
    return 0;
}
