using System;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.Win32.SafeHandles;
using System.ComponentModel;

class CompanionHubFetcher
{

    private const uint FILE_SHARE_READ = 0x00000001;
    private const uint FILE_SHARE_WRITE = 0x00000002;
    private const uint OPEN_EXISTING = 3;
    private const uint GENERIC_WRITE = 0x40000000;
    private const uint GENERIC_READ = 0x80000000;
    private const int IOCTL_USB_GET_PORT_CONNECTOR_PROPERTIES = 0x22044C;

    [StructLayout(LayoutKind.Sequential)]
    public struct USB_PORT_CONNECTOR_PROPERTIES
    {
        public uint ConnectionIndex;
        public uint ActualLength;
        public USB_PORT_PROPERTIES UsbPortProperties;
        public ushort CompanionIndex;
        public ushort CompanionPortNumber;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1)]
        public string CompanionHubSymbolicLinkName;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct USB_PORT_PROPERTIES
    {
        public uint PortProperties;
    }

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern SafeFileHandle CreateFile(
        string lpFileName,
        uint dwDesiredAccess,
        uint dwShareMode,
        IntPtr lpSecurityAttributes,
        uint dwCreationDisposition,
        uint dwFlagsAndAttributes,
        IntPtr hTemplateFile
    );
    /*
    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern bool DeviceIoControl(
        SafeFileHandle hDevice,
        uint dwIoControlCode,
        ref USB_PORT_CONNECTOR_PROPERTIES lpInBuffer,
        uint nInBufferSize,
        ref USB_PORT_CONNECTOR_PROPERTIES lpOutBuffer,
        uint nOutBufferSize,
        out uint lpBytesReturned,
        IntPtr lpOverlapped
    );*/

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern bool DeviceIoControl(
    SafeFileHandle hDevice,
    uint dwIoControlCode,
    IntPtr lpInBuffer,
    uint nInBufferSize,
    IntPtr lpOutBuffer,
    uint nOutBufferSize,
    out uint lpBytesReturned,
    IntPtr lpOverlapped);

    //---------------------------------------------
    private static readonly Guid GUID_DEVINTERFACE_USB_HUB = new Guid("f18a0e88-c30c-11d0-8815-00a0c906bed8");

    [DllImport("setupapi.dll", SetLastError = true)]
    private static extern IntPtr SetupDiGetClassDevs(
        [In()] ref Guid ClassGuid,
        [MarshalAs(UnmanagedType.LPWStr)] string Enumerator,
        IntPtr hwndParent,
        uint Flags);

    [DllImport("setupapi.dll", SetLastError = true)]
    private static extern bool SetupDiEnumDeviceInterfaces(
        IntPtr DeviceInfoSet,
        IntPtr DeviceInfoData,
        ref Guid InterfaceClassGuid,
        uint MemberIndex,
        ref SP_DEVICE_INTERFACE_DATA DeviceInterfaceData);

    [DllImport("setupapi.dll", SetLastError = true)]
    private static extern bool SetupDiGetDeviceInterfaceDetail(
        IntPtr DeviceInfoSet,
        ref SP_DEVICE_INTERFACE_DATA DeviceInterfaceData,
        IntPtr DeviceInterfaceDetailData,
        uint DeviceInterfaceDetailDataSize,
        out uint RequiredSize,
        ref SP_DEVINFO_DATA DeviceInfoData);

    [DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Auto)]
    private static extern bool SetupDiEnumDeviceInfo(
        IntPtr DeviceInfoSet,
        uint MemberIndex,
        ref SP_DEVINFO_DATA DeviceInfoData);

    [DllImport("setupapi.dll", CharSet = CharSet.Auto, SetLastError = true)]
    private static extern bool SetupDiGetDeviceInstanceId(
        IntPtr DeviceInfoSet,
        ref SP_DEVINFO_DATA DeviceInfoData,
        [Out] StringBuilder DeviceInstanceId,
        int DeviceInstanceIdSize,
        out int RequiredSize);

    [StructLayout(LayoutKind.Sequential)]
    private struct SP_DEVICE_INTERFACE_DATA
    {
        public int cbSize;
        public Guid InterfaceClassGuid;
        public int Flags;
        public IntPtr Reserved;
    }

    [StructLayout(LayoutKind.Sequential)]
    private struct SP_DEVINFO_DATA
    {
        public int cbSize;
        public Guid ClassGuid;
        public uint DevInst;
        public IntPtr Reserved;
    }

    private const uint DIGCF_PRESENT = 0x02;
    private const uint DIGCF_DEVICEINTERFACE = 0x10;

    public static string GetDevicePathFromInstanceId(string instanceId)
    {
        Guid classGuid = GUID_DEVINTERFACE_USB_HUB;
        IntPtr deviceInfoSet = SetupDiGetClassDevs(ref classGuid, null, IntPtr.Zero, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
        if (deviceInfoSet == IntPtr.Zero || deviceInfoSet.ToInt64() == -1)
            return null;

        try
        {
            uint index = 0;
            SP_DEVICE_INTERFACE_DATA interfaceData = new SP_DEVICE_INTERFACE_DATA();
            interfaceData.cbSize = Marshal.SizeOf(interfaceData);

            while (SetupDiEnumDeviceInterfaces(deviceInfoSet, IntPtr.Zero, ref classGuid, index, ref interfaceData))
            {
                SP_DEVINFO_DATA devInfoData = new SP_DEVINFO_DATA();
                devInfoData.cbSize = Marshal.SizeOf(devInfoData);

                uint requiredSize;
                SetupDiGetDeviceInterfaceDetail(deviceInfoSet, ref interfaceData, IntPtr.Zero, 0, out requiredSize, ref devInfoData);

                IntPtr detailDataBuffer = Marshal.AllocHGlobal((int)requiredSize);
                try
                {
                    Marshal.WriteInt32(detailDataBuffer, IntPtr.Size == 8 ? 8 : 6);

                    if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, ref interfaceData, detailDataBuffer, requiredSize, out requiredSize, ref devInfoData))
                    {                                                
                        IntPtr pDevicePathName = IntPtr.Add(detailDataBuffer, 4);

                        string devicePath = Marshal.PtrToStringAnsi(pDevicePathName);
                        

                        if (!string.IsNullOrEmpty(devicePath))
                        {
                            StringBuilder idBuilder = new StringBuilder(512);
                            int sizeOut;
                            if (SetupDiGetDeviceInstanceId(deviceInfoSet, ref devInfoData, idBuilder, idBuilder.Capacity, out sizeOut))
                            {
                                if (string.Equals(idBuilder.ToString(), instanceId, StringComparison.OrdinalIgnoreCase))
                                {
                                    return devicePath;
                                }
                            }
                        }
                    }
                }
                finally
                {
                    Marshal.FreeHGlobal(detailDataBuffer);
                }

                index++;
            }
        }
        finally
        {
            // Cleanup
        }

        return null;
    }


    //---------------------------------------------
    /*
    public static string GetCompanionHubSymbolicLinkName(string hubDevicePath, uint portNumber)
        {
            using (SafeFileHandle hubHandle = CreateFile(
                hubDevicePath,
                GENERIC_WRITE | GENERIC_READ,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                IntPtr.Zero,
                OPEN_EXISTING,
                0,
                IntPtr.Zero))
            {
                if (hubHandle.IsInvalid)
                {
                    throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error(), "Failed to open hub device handle.");
                }

                USB_PORT_CONNECTOR_PROPERTIES connectorProperties = new USB_PORT_CONNECTOR_PROPERTIES
                {
                    ConnectionIndex = portNumber,
                    CompanionIndex = 0
                };

                uint bytesReturned;
                bool success = DeviceIoControl(
                    hubHandle,
                    IOCTL_USB_GET_PORT_CONNECTOR_PROPERTIES,
                    ref connectorProperties,
                    (uint)Marshal.SizeOf(connectorProperties),
                    ref connectorProperties,
                    (uint)Marshal.SizeOf(connectorProperties),
                    out bytesReturned,
                    IntPtr.Zero);

                if (!success)
                {
                    throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error(), "DeviceIoControl failed.");
                }

                return connectorProperties.CompanionHubSymbolicLinkName;
            }
        }*/

    public static string GetCompanionHubSymbolicLinkName(string hubDevicePath, uint portNumber)
    {
        using (SafeFileHandle hubHandle = CreateFile(
            hubDevicePath,
            GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            IntPtr.Zero,
            OPEN_EXISTING,
            0,
            IntPtr.Zero))
        {
            if (hubHandle.IsInvalid)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error(), "Failed to open hub device handle.");
            }

            int bufferSize = 1024;
            IntPtr buffer = Marshal.AllocHGlobal(bufferSize);
            try
            {
                // Clear buffer
                for (int i = 0; i < bufferSize; i++) Marshal.WriteByte(buffer, i, 0);

                // Set ConnectionIndex (first field)
                Marshal.WriteInt32(buffer, (int)portNumber);

                uint bytesReturned;
                bool success = DeviceIoControl(
                    hubHandle,
                    IOCTL_USB_GET_PORT_CONNECTOR_PROPERTIES,
                    buffer,
                    (uint)bufferSize,
                    buffer,
                    (uint)bufferSize,
                    out bytesReturned,
                    IntPtr.Zero);

                if (!success)
                {
                    int err = Marshal.GetLastWin32Error();
                    throw new Win32Exception(err, $"DeviceIoControl failed with error {err}");
                }

                // CompanionHubSymbolicLinkName is a WCHAR* (starts at offset 12h for x86, 16h for x64 struct layout)
                int stringOffset = 20; // safe for both x86/x64 fixed portion
                string result = Marshal.PtrToStringAuto(IntPtr.Add(buffer, stringOffset));

                return string.IsNullOrWhiteSpace(result) ? null : result;
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }
    }

}