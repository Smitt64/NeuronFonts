#define _AFXDLL
#include <afxwin.h>
#include <iostream>
#include <comdef.h>
#include "NeuronFontsCom.h"

using namespace std;

int main(int argc, char **argv)
{
    HRESULT		hr;
    hr = CoInitialize(0);

    INeuronFonts *nf = NULL;

    if(SUCCEEDED(hr))
    {
        hr = CoCreateInstance(CLSID_NeuronFonts,
                            NULL,
                            CLSCTX_LOCAL_SERVER,
                            DIID_INeuronFonts,
                            (void**) &nf);
        if(SUCCEEDED(hr))
        {
        }
        else
        {
            //cout << "CoCreateInstance Failed2 " << _com_error(hr).ErrorMessage() << endl;

            AfxMessageBox(_com_error(hr).ErrorMessage(), MB_ICONSTOP);
        }
    }

    CoUninitialize();
    return 0;
}
