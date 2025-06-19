/**
 *   USB Insight Hub Enumeration Extraction Agent
 *
 *   Works in tandem with USB Insight Hub hardware
 *   
 *   https://github.com/Aeriosolutions/USB-Insight-HUB-Software
 *
 *   Copyright (C) 2024 - 2025 Aeriosolutions
 *   Copyright (C) 2024 - 2025 JoDaSa

 * MIT License. 
 **/

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Management;
using System.Linq;
using static UEnumerationExtractionAgent.UsbDeviceNode;


namespace UEnumerationExtractionAgent
{
    public class UsbDeviceNode
    {
        public string Description { get; set; }
        public string InstanceId { get; set; }
        public string PortHubInfo { get; set; }  
        public string LocationPath { get; set; }
        public string PortPath{ get; set; }
        public string Port { get; set; }
        public string ContainerID { get; set; }
        public string CompanionHub { get; set; }
        public string LastLocableInstanceId { get; set; }
        public int Level { get; set; } // This is the level from the root

        public UsbDeviceNode Parent { get; set; }
        public List<UsbDeviceNode> Children { get; set; } = new List<UsbDeviceNode>();

       
        public class ExternalDriveInfo
        {
            public string Letter;
            public string DeviceID;
        }

        public void PrintTree(string indent = "", bool isLast = true)
        {
            Console.Write(indent);
            Console.Write(isLast ? "└─" : "├─");
            //Console.WriteLine(Description + $" [{InstanceId}]");
            Console.WriteLine($"{Description} [{InstanceId}][{Level}][{PortPath}][{Port}]");

            for (int i = 0; i < Children.Count; i++)
            {
                bool last = i == Children.Count - 1;
                Children[i].PrintTree(indent + (isLast ? "  " : "│ "), last);
            }
        }

        public string GetLocationFromID(string matchId)
        {
            if (InstanceId == matchId) 
                return PortPath;

            for (int i = 0; i < Children.Count; i++)
            {                
                string res = Children[i].GetLocationFromID(matchId);
                if(res != null) return res;
            }

            return null;
        }

        public string GetLocationFromVID_PID(string vid, string pid)
        {
            if (InstanceId.Contains("PID_" + pid) && InstanceId.Contains("VID_" + vid)) return PortPath;

            for (int i = 0; i < Children.Count; i++)
            {
                string res = Children[i].GetLocationFromVID_PID(vid, pid);
                if (res != null) return res;
            }

            return null;
        }

        public void InheritParentProp()
        {
            if(Children.Count > 0)
               for (int i = 0; i < Children.Count; i++)
               {
                    if (string.IsNullOrEmpty(Children[i].Port))
                    {
                        Children[i].Port = Port;
                        Children[i].PortPath = PortPath;
                        Children[i].LastLocableInstanceId = InstanceId;
                    }
                    Children[i].InheritParentProp();
               }
        }

    }

    public static class TreeTrimer
    {


        public static List<UsbDeviceNode> ExtractTopLevelMatchingSubtrees(List<UsbDeviceNode> roots, Func<UsbDeviceNode, bool> match, bool descend=false)
        {
            var matchedSubtrees = new List<UsbDeviceNode> ();

            foreach (var root in roots)
            {
                CollectMatches(root, match, matchedSubtrees,descend);
            }

            return matchedSubtrees;
        }

        // Recursive traversal with match collection
        private static void CollectMatches(UsbDeviceNode current, Func<UsbDeviceNode, bool> match, List<UsbDeviceNode> collector, bool descend = false)
        {
            if (current == null) return;

            if (match(current))
            {
                collector.Add(CloneSubtree(current));
                if(!descend) return; // Stop descending
            }

            foreach (var child in current.Children)
            {
                CollectMatches(child, match, collector,descend);
            }
        }

        // Deep copy
        private static UsbDeviceNode CloneSubtree(UsbDeviceNode node)
        {
            return new UsbDeviceNode
            {
                //InstanceId = node.InstanceId,
                Children = node.Children.Select(CloneSubtree).ToList(),

                InstanceId = node.InstanceId,
                Description = node.Description,
                LocationPath = node.LocationPath,
                PortPath = node.PortPath,
                Port = node.Port,
                ContainerID = node.ContainerID,
                CompanionHub = node.CompanionHub,
                LastLocableInstanceId = node.LastLocableInstanceId,
                Level = node.Level

            };
        }

    }

    public class UsbDeviceTreeBuilder
    {
        const int DIGCF_PRESENT = 0x00000002;
        const int DIGCF_ALLCLASSES = 0x00000004;
        const int SPDRP_DEVICEDESC = 0x00000000;

        private const int SPDRP_LOCATION_PATHS = 0x00000023;

        [StructLayout(LayoutKind.Sequential)]
        struct SP_DEVINFO_DATA
        {
            public int cbSize;
            public Guid ClassGuid;
            public int DevInst;
            public IntPtr Reserved;
        }
        //------------------- 

        [DllImport("setupapi.dll", SetLastError = true)]
        private static extern IntPtr SetupDiGetClassDevs(
            IntPtr ClassGuid,
            string Enumerator,
            IntPtr hwndParent,
            int Flags);

        [DllImport("setupapi.dll", SetLastError = true)]
        private static extern bool SetupDiEnumDeviceInfo(
        IntPtr DeviceInfoSet,
        uint MemberIndex,
        ref SP_DEVINFO_DATA DeviceInfoData);

        [DllImport("setupapi.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        private static extern bool SetupDiGetDeviceRegistryProperty(
            IntPtr DeviceInfoSet,
            ref SP_DEVINFO_DATA DeviceInfoData,
            int Property,
            out uint PropertyRegDataType,
            [Out] StringBuilder PropertyBuffer,
            int PropertyBufferSize,
            out uint RequiredSize);

        [DllImport("setupapi.dll", SetLastError = true)]
        private static extern bool SetupDiDestroyDeviceInfoList(IntPtr DeviceInfoSet);

        [DllImport("cfgmgr32.dll", CharSet = CharSet.Unicode)]
        private static extern int CM_Get_Device_ID(uint devInst, StringBuilder buffer, int bufferLen, int flags);    

        [DllImport("cfgmgr32.dll", CharSet = CharSet.Unicode)]
        private static extern int CM_Get_Parent(out uint parent, uint child, int flags);

        [DllImport("cfgmgr32.dll", CharSet = CharSet.Unicode)]
        private static extern int CM_Locate_DevNode(out uint devInst, string pDeviceID, int flags);

        //-------------------------

        private static string GetParentDeviceId(string deviceId)
        {
            const int CR_SUCCESS = 0;

            if (CM_Locate_DevNode(out uint devInst, deviceId, 0) != CR_SUCCESS)
                return null;

            if (CM_Get_Parent(out uint parentDevInst, devInst, 0) != CR_SUCCESS)
                return null;

            // Use Win32 SetupDi to get the Instance ID of parentDevInst
            StringBuilder parentId = new StringBuilder(260);
            int result = CM_Get_Device_ID(parentDevInst, parentId, parentId.Capacity, 0);
            return result == CR_SUCCESS ? parentId.ToString() : null;
        }

        public List<UsbDeviceNode> BuildTree()
        {
            var nodes = new Dictionary<string, UsbDeviceNode>();
            var roots = new List<UsbDeviceNode>();

            //var searcher = new ManagementObjectSearcher("SELECT * FROM Win32_PnPEntity WHERE PNPDeviceID LIKE '%USB%'");
            var searcher = new ManagementObjectSearcher("SELECT * FROM Win32_PnPEntity");

            foreach (ManagementObject device in searcher.Get())
            {                
                string instanceId = (string)device["PNPDeviceID"];
                string portHubInfo = ExtractPortHubInfo(instanceId);
                string description = (string)device["Name"];
                string locationPath = GetDeviceLocationPath(instanceId);
                string portPath = ParseUsbPortPath(locationPath);                
                string port = "";
                string containerID = "";
                string companionHub = "";
                //Guid? contID = ContainerIdFetcher.GetContainerIdFromPnpDeviceId(instanceId);
                //if (contID.HasValue) containerID = contID.ToString();

                string lastLocableInstanceId = "";
                if (!string.IsNullOrEmpty(portPath))
                {
                    var portparts = portPath.Split('-');
                    port = portparts[portparts.Length-1];
                    lastLocableInstanceId = instanceId;
                }

                var node = new UsbDeviceNode
                {
                    InstanceId = instanceId,
                    Description = description ?? "Unknown Device",
                    LocationPath = locationPath,
                    PortPath = portPath,
                    Port = port,
                    ContainerID = containerID,
                    CompanionHub = companionHub,
                    LastLocableInstanceId = lastLocableInstanceId
                };

                nodes[instanceId] = node;
            }

            // Second pass: establish parent-child relationships
            foreach (var kvp in nodes)
            {
                string instanceId = kvp.Key;
                UsbDeviceNode node = kvp.Value;

                string parentId = GetParentDeviceId(instanceId);

                if (parentId != null && nodes.ContainsKey(parentId))
                {
                    var parent = nodes[parentId];
                    node.Parent = parent;
                    parent.Children.Add(node);
                }
                else
                {
                    // No parent found — treat as root
                    roots.Add(node);
                }
            }

            foreach (var r in roots)
            {
                r.InheritParentProp();
            }

            //trim the tree for devices with top level usb parents

            var test = TreeTrimer.ExtractTopLevelMatchingSubtrees(roots, node => node.Description.ToLower().Contains("usb"));
            return test;
        }

        public static void AssignLevelsRecursive(UsbDeviceNode node, int currentLevel = 0)
        {
            node.Level = currentLevel;

            foreach (var child in node.Children)
            {
                AssignLevelsRecursive(child, currentLevel + 1);
            }
        }

        public string GetHubAndPortPath(string pnpDeviceId)
        {
            var tree = BuildTree();
            var flatList = new List<UsbDeviceNode>();
            void Flatten(UsbDeviceNode node)
            {
                flatList.Add(node);
                foreach (var child in node.Children)
                    Flatten(child);
            }

            foreach (var root in tree)
                Flatten(root);

            var target = flatList.Find(n => n.InstanceId.Equals(pnpDeviceId, StringComparison.OrdinalIgnoreCase));
            if (target == null) return null;

            var path = new Stack<string>();
            var current = target;
            while (current != null)
            {
                path.Push(current.Description);
                current = current.Parent;
            }
            return string.Join(" -> ", path);
        }

        private static string ExtractPortHubInfo(string instanceId)
        {
            if (instanceId == null)
                return "";

            int portIndex = instanceId.IndexOf("Port_#");
            if (portIndex >= 0)
            {
                return instanceId.Substring(portIndex);
            }

            return "";
        }

        private static string GetDeviceLocationPath(string deviceInstanceId)
        {
            IntPtr deviceInfoSet = SetupDiGetClassDevs(IntPtr.Zero, null, IntPtr.Zero, DIGCF_ALLCLASSES | DIGCF_PRESENT);

            if (deviceInfoSet == IntPtr.Zero || deviceInfoSet.ToInt64() == -1)
                return "";

            try
            {
                SP_DEVINFO_DATA devInfoData = new SP_DEVINFO_DATA();
                devInfoData.cbSize = Marshal.SizeOf(typeof(SP_DEVINFO_DATA));

                for (uint i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, ref devInfoData); i++)
                {
                    var idBuilder = new StringBuilder(512);
                    if (CM_Get_Device_ID((uint)devInfoData.DevInst, idBuilder, idBuilder.Capacity, 0) == 0)
                    {
                        string foundId = idBuilder.ToString();
                        if (string.Equals(foundId, deviceInstanceId, StringComparison.OrdinalIgnoreCase))
                        {
                            var locInfo = new StringBuilder(512);
                            bool success = SetupDiGetDeviceRegistryProperty(deviceInfoSet, ref devInfoData,
                                SPDRP_LOCATION_PATHS, out _, locInfo, locInfo.Capacity, out _);

                            return success ? locInfo.ToString() : "";
                        }
                    }
                }
            }
            finally
            {
                SetupDiDestroyDeviceInfoList(deviceInfoSet);
            }

            return "";
        }
       
        public List<ExternalDriveInfo> GetDriveLetterByPNPDeviceId()
        {
            var usbSearcher = new ManagementObjectSearcher("SELECT * FROM Win32_LogicalDisk WHERE DriveType = 2");
            var results = new List<ExternalDriveInfo>(); 

            foreach (ManagementObject logicalDisk in usbSearcher.Get())
            {
                string driveLetter = logicalDisk["Name"]?.ToString();

                if (string.IsNullOrEmpty(driveLetter))
                    continue;

                //Console.WriteLine("Removable Drive Letter: " + driveLetter);

                try
                {
                    // Step 1: Get Partition from LogicalDisk
                    var partitionQuery = new ManagementObjectSearcher(
                        "ASSOCIATORS OF {Win32_LogicalDisk.DeviceID='" + driveLetter +
                        "'} WHERE AssocClass = Win32_LogicalDiskToPartition"
                    );

                    foreach (ManagementObject partition in partitionQuery.Get())
                    {
                        string partitionID = partition["DeviceID"]?.ToString();

                        // Step 2: Get DiskDrive from Partition
                        var driveQuery = new ManagementObjectSearcher(
                            "ASSOCIATORS OF {Win32_DiskPartition.DeviceID='" + partitionID +
                            "'} WHERE AssocClass = Win32_DiskDriveToDiskPartition"
                        );

                        foreach (ManagementObject diskDrive in driveQuery.Get())
                        {
                            string PNPDeviceID = diskDrive["PNPDeviceID"]?.ToString();                            
                            //Console.WriteLine($"[{driveLetter}][{PNPDeviceID}]");
                            results.Add(new ExternalDriveInfo { Letter = driveLetter, DeviceID= PNPDeviceID});
                        }
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Error tracing disk: " + ex.Message);
                }
                
            }

            return results;
        }


        private static string ParseUsbPortPath(string locationPath)
        {
            if (string.IsNullOrEmpty(locationPath))
                return "";

            var parts = locationPath.Split('#');
            var usbPorts = new List<string>();

            foreach (var part in parts)
            {
                if (part.StartsWith("USB(", StringComparison.OrdinalIgnoreCase) || 
                    part.StartsWith("PCI(", StringComparison.OrdinalIgnoreCase) || 
                    part.StartsWith("USBROOT(", StringComparison.OrdinalIgnoreCase))
                {
                    int start = part.IndexOf('(') + 1;
                    int end = part.IndexOf(')', start);
                    if (start > 0 && end > start)
                    {
                        string port = part.Substring(start, end - start);
                        usbPorts.Add(port);
                    }
                }
            }

            return usbPorts.Count > 0 ? string.Join("-", usbPorts) : "";
        }

    }
}
