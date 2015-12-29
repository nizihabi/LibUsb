#include "Usb.h"

Usb::Usb(int vid,int pid)
{
    m_ventorID = vid;
    m_ProductID = pid;

    m_devhandle = NULL;
    m_ctx = NULL;
    m_opened = false;
    m_claimed =false;
    m_interface_number = 0;
}

Usb::~Usb()
{
    if(m_opened)
        Close();

    if(m_devs)
        delete m_devs;

    if(m_devhandle)
        delete m_devhandle;

    if(m_ctx)
        delete m_ctx;

}

bool Usb::Init()
{
    res = libusb_init(&m_ctx); //initialize the library for the session we just declared
	if(res < 0)
	{
		return false;
	}

	return true;
}

bool Usb::Open()
{

    ssize_t cnt;
    cnt = libusb_get_device_list(m_ctx, &m_devs); //get the list of devices
	if(cnt < 0)
	{
		return false;
	}

	m_devhandle = libusb_open_device_with_vid_pid(m_ctx, m_ventorID, m_ProductID);
	if(m_devhandle == NULL)
	{
        libusb_free_device_list(m_devs, 1); //free the list, unref the devices in it
        return false;
	}
	else
	{
        m_opened = true;
        libusb_free_device_list(m_devs, 1);
        return true;
	}

}

bool Usb::Claim(int interface)
{
    if(m_opened)
    {
        m_interface_number = interface;
        if(libusb_kernel_driver_active(m_devhandle, interface) > 0)
        { //find out if kernel driver is attached
            libusb_detach_kernel_driver(m_devhandle, interface);//detach it
        }
        res = libusb_claim_interface(m_devhandle, interface);
        if(res < 0)
        {
            return false;
        }
        m_claimed = true;
        return true;
    }
    return false;
}

void Usb::Release()
{
    if(m_claimed)
    {
        libusb_release_interface(m_devhandle, m_interface_number);
        m_claimed = false;
    }
}


void Usb::Close()
{
    libusb_close(m_devhandle);
	libusb_exit(m_ctx);
	m_opened = false;
}

unsigned char Usb::Get_Device_satus()
{
    if(m_claimed)
    {
        int interface = 0;
        unsigned char byte;
        libusb_control_transfer(m_devhandle, LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
                LIBUSB_REQUEST_CLEAR_FEATURE,
                0,
                interface,
                &byte, 1, 5000);
        return byte;
    }

    return 0;

}

int Usb::Read(unsigned char *data,unsigned int size,unsigned int timeout)
{
    int readed = 0;
    if(m_claimed)
    {
        res = libusb_bulk_transfer(m_devhandle, LIBUSB_ENDPOINT_IN, data, size, &readed, timeout);
    }
    return readed;
}

int Usb::Write(unsigned char *data,unsigned int size,unsigned int timeout)
{
    int wrote = 0;
    if(m_claimed)
    {
        res = libusb_bulk_transfer(m_devhandle, LIBUSB_ENDPOINT_OUT, data, size, &wrote, timeout);
    }
    return wrote;
}
