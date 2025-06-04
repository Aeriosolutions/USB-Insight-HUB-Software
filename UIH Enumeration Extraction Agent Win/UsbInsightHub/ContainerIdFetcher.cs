using System;
using System.Runtime.InteropServices;

class ContainerIdFetcher
{
    public static Guid? GetContainerIdFromPnpDeviceId(string pnpDeviceId)
    {
        IntPtr deviceInfoSet = SetupDiCreateDeviceInfoList(IntPtr.Zero, IntPtr.Zero);
        if (deviceInfoSet == IntPtr.Zero || deviceInfoSet == new IntPtr(-1))
            return null;

        try
        {
            SP_DEVINFO_DATA deviceInfoData = new SP_DEVINFO_DATA();
            deviceInfoData.cbSize = (uint)Marshal.SizeOf(typeof(SP_DEVINFO_DATA));

            bool success = SetupDiOpenDeviceInfo(
                deviceInfoSet,
                pnpDeviceId,
                IntPtr.Zero,
                0,
                ref deviceInfoData);

            if (!success)
                return null;

            byte[] buffer = new byte[16];
            uint propertyType;
            uint requiredSize;

            DEVPROPKEY DEVPKEY_Device_ContainerId = new DEVPROPKEY
            {
                fmtid = new Guid("8c7ed206-3f8a-4827-b3ab-ae9e1faefc6c"),
                pid = 2
            };

            bool result = SetupDiGetDeviceProperty(
                deviceInfoSet,
                ref deviceInfoData,
                ref DEVPKEY_Device_ContainerId,
                out propertyType,
                buffer,
                (uint)buffer.Length,
                out requiredSize,
                0);

            if (!result)
                return null;

            return new Guid(buffer);
        }
        finally
        {
            SetupDiDestroyDeviceInfoList(deviceInfoSet);
        }
    }

    // Required constants, structs, and P/Invoke declarations

    const uint DIGCF_PRESENT = 0x02;
    const uint DIGCF_ALLCLASSES = 0x04;

    [StructLayout(LayoutKind.Sequential)]
    public struct SP_DEVINFO_DATA
    {
        public uint cbSize;
        public Guid ClassGuid;
        public uint DevInst;
        public IntPtr Reserved;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct DEVPROPKEY
    {
        public Guid fmtid;
        public uint pid;
    }

    [DllImport("setupapi.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern IntPtr SetupDiGetClassDevs(
        IntPtr ClassGuid,
        [MarshalAs(UnmanagedType.LPWStr)] string Enumerator,
        IntPtr hwndParent,
        uint Flags);

    [DllImport("setupapi.dll", SetLastError = true)]
    static extern bool SetupDiEnumDeviceInfo(
        IntPtr DeviceInfoSet,
        uint MemberIndex,
        ref SP_DEVINFO_DATA DeviceInfoData);

    [DllImport("setupapi.dll", SetLastError = true)]
    static extern bool SetupDiDestroyDeviceInfoList(IntPtr DeviceInfoSet);

    [DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Unicode)]
    static extern bool SetupDiGetDeviceProperty(
        IntPtr deviceInfoSet,
        ref SP_DEVINFO_DATA deviceInfoData,
        ref DEVPROPKEY propertyKey,
        out uint propertyType,
        [Out] byte[] propertyBuffer,
        uint propertyBufferSize,
        out uint requiredSize,
        uint flags);

    [DllImport("setupapi.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern bool SetupDiOpenDeviceInfo(
        IntPtr deviceInfoSet,
        string deviceInstanceId,
        IntPtr hwndParent,
        uint openFlags,
        ref SP_DEVINFO_DATA deviceInfoData);

    [DllImport("setupapi.dll", SetLastError = true)]
    static extern IntPtr SetupDiCreateDeviceInfoList(IntPtr ClassGuid, IntPtr hwndParent);

}