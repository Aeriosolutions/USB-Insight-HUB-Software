﻿namespace USBInfoHub.DeviceFinder
{
    using System;

    public class UsbDeviceInterface
    {
        public String InterfaceId { get; internal set; }

        internal UsbDeviceInterface(String interfaceId)
        {
            this.InterfaceId = interfaceId;
        }
    }
}
