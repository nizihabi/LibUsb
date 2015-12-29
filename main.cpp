#include <iostream>
#include <dlfcn.h>
#include <stdio.h>
#include "Usb.h"

// First, use "lsusb" see vid and pid.
// there is my printer(hp deskjet 1010) vid and pid.
#define VID 0x131d
#define PID 0x0158

using namespace std;


int main() {


    Usb *myUsb = new Usb(VID,PID);
    if(!myUsb->Init())
        cout<<"Init Erro"<<endl;

    if(!myUsb->Open())
        cout<<"Open Dev Erro"<<endl;

    myUsb->Claim();

    unsigned char status;

    status = myUsb->Get_Device_satus();
    printf("status:0x%x\n", status);

    unsigned char data[1024];
    myUsb->Read(data,1024);
    cout<<data<<endl;

    myUsb->Close();

	return 0;
}
