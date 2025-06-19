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
using System.Collections.Generic; //use lists
using System.Linq;
using EnumerationExtractionAgent.WMICLasses;
using UEnumerationExtractionAgent;


namespace EnumerationExtractionAgent
{
    internal class Program
    {
        public static bool updateUSBDevicesSemaphore = false; //Flags if the function is busy
        public static int rescheduleCount = 0;
        private static System.Timers.Timer aTimer;
        private static System.Timers.Timer refreshTimer;        

        public class CompanionHub
        {
            public string Path2;
            public string Path3;
        }

        public static List<UsbInsightHub> ActiveHubs { get; set; } = new List<UsbInsightHub>();
        public static List<CompanionHub> CompanionHubBase = new List<CompanionHub>();

        public static String controllerVid = "303A"; //ESP32-S3
        public static String controllerPid = "1001"; //ESP32-S3 
        public static String Hub_2Vid = "045B";
        public static String Hub_2Pid = "0209";
        public static String Hub_3Vid = "045B";
        public static String Hub_3Pid = "0210";

        //Match criteria to detect a valid USB Insight Hub
        public static bool IsUsb2InsightHub(UsbDeviceNode node) {

            bool isUih = false;
            var localNode = new List<UsbDeviceNode>();
            localNode.Add(node);
            isUih = node.InstanceId.Contains("PID_" + Hub_2Pid) && node.InstanceId.Contains("VID_" + Hub_2Vid);
            if(isUih)
            {
                isUih = false;
                var hubports = TreeTrimer.ExtractTopLevelMatchingSubtrees(localNode, n=> n.InstanceId.Contains("PID_" + controllerPid)&& n.InstanceId.Contains("VID_" + controllerVid), true);
                if (hubports.Count > 0)
                {
                    foreach (var hubport in hubports)
                    {
                        if (hubport.Port.Equals("4")) isUih = true;
                    }
                }                
            }

            return isUih;
        }

        public static string GetUIHCOM(UsbDeviceNode root)
        {            
            foreach(var node in root.Children)
            {
                if (node.Port.Equals("4"))
                {
                    try { 
                        var desc = node.Children[0].Description;
                        var iStart = desc.IndexOf("(COM");
                        var iEnd = desc.LastIndexOf(")") - 1;

                        if (iStart != -1)
                        {
                            return desc.Substring(iStart + 1, iEnd - iStart);
                        }
                    }
                    catch { }
                }
            }
            
            return "";
        }

        public static string GetPathHead(UsbDeviceNode node)
        {
            var pathParts = node.PortPath.Split('-');
            if (pathParts.Length >= 2)
            {
                var pathnoroot = string.Join("-", pathParts.Where((part, index) => index != (pathParts.Length - node.Level-1)));
                
                return pathnoroot;
            }

            return "";
        }

        public static UsbDeviceNode MatchWithUSB3Head(UsbDeviceNode root, List<UsbDeviceNode> hubs3)
        {
            string partHead = GetPathHead(root);

            if (hubs3.Count > 0)
            {
                foreach (var hub in hubs3)
                {
                    if (GetPathHead(hub) == partHead) { return hub; }
                }
            }
            return null;
        }

        public static void UpdateHubBase(List<UsbInsightHub> root)
        {
            foreach (var aHub in ActiveHubs)
            {
                bool found = false;
                foreach (var Base in CompanionHubBase)
                {
                    if (Base.Path2 == aHub.Hub2Node.PortPath)
                    {
                        if(aHub.CompanionHub3Node?.PortPath != null)
                            if(Base.Path3 != aHub.CompanionHub3Node.PortPath)
                                Base.Path3 = aHub.CompanionHub3Node.PortPath;
                        found = true;
                        continue;
                    }
                }
                if(!found)
                {
                    if (aHub.CompanionHub3Node?.PortPath != null)
                        CompanionHubBase.Add(new CompanionHub
                        {
                            Path2 = aHub.Hub2Node.PortPath,
                            Path3 = aHub.CompanionHub3Node.PortPath
                        });
                }                    
            }
        }

        public static void HandleHubChanges(List<UsbInsightHub> newhubs)
        {
            //a hub was removed?
            for (int i = ActiveHubs.Count - 1; i >= 0; i--)
            {
                var hub = ActiveHubs[i];
                var hubport = hub.Hub2Node.PortPath;
                if (newhubs.FirstOrDefault(f => f.Hub2Node.PortPath == hubport) == null)
                {
                    ActiveHubs[i].Deinit();
                    ActiveHubs.RemoveAt(i);
                }
            }            

            //ActiveHubs.RemoveAll(hub =>newhubs.FirstOrDefault(f => f.Hub2Node.PortPath == hub.Hub2Node.PortPath) == null);

            //a hub was added?
            foreach (var hub in newhubs)
            {   
                
                if(ActiveHubs?.FirstOrDefault(f => f.Hub2Node.PortPath == hub.Hub2Node.PortPath) == null)
                {
                    ActiveHubs.Add(hub);
                    hub.Init();                    
                }

                ActiveHubs.ForEach(h => { 
                    if(hub.COMName == h.COMName)
                    {
                        h.Hub2Node = hub.Hub2Node;
                        h.CompanionHub3Node = hub.CompanionHub3Node;                        
                    }
                });

            }

        }

        public static void updateUSBDevices()
        {
            updateUSBDevicesSemaphore = true;
            rescheduleCount = 0;

            var watch = System.Diagnostics.Stopwatch.StartNew();

            watch.Start();

            // Build the tree

            var builder = new UsbDeviceTreeBuilder();            
            var roots = builder.BuildTree();

            Console.WriteLine("Full USB Device Tree:");

            foreach (var root in roots)
            {
                root.PrintTree(); // Recursively prints each device and its children
                //assign a level to each node from the root. Used to identify companion hubs in level 0
                UsbDeviceTreeBuilder.AssignLevelsRecursive(root,-2); 
            }
            
            // identify USB Insight hubs and populate the hubs list
            Console.WriteLine("\nUSB Insight Hubs subtrees:");
            var hubs2nodes = TreeTrimer.ExtractTopLevelMatchingSubtrees(roots, IsUsb2InsightHub,true);
            var hubs3nodes = TreeTrimer.ExtractTopLevelMatchingSubtrees(roots, node => node.InstanceId.Contains("PID_" + Hub_3Pid) && node.InstanceId.Contains("VID_" + Hub_3Vid), true);
            List<UsbDeviceNode> uniquehubs3 = new List<UsbDeviceNode>();
            List<UsbDeviceNode> duplicatehubs3 = new List<UsbDeviceNode>();
            int rootDuplicates = 0;
            var ExternalDriveInfo = builder.GetDriveLetterByPNPDeviceId();

            hubs3nodes.ForEach(p => p.PrintTree());

            List<UsbInsightHub> tActiveHubs = new List<UsbInsightHub>();
            foreach (var node in hubs2nodes)
            {
                node.PrintTree();
                var hubnode = new UsbInsightHub();
                hubnode.Hub2Node = node;
                hubnode.COMName = GetUIHCOM(node);                

                //look into the base of previous connections for a matching companion for level 0 hubs
                var cref = CompanionHubBase.FirstOrDefault(s => s.Path2 == node.PortPath);
                if (cref != null && node.Level==0)
                {
                    var match = hubs3nodes.FirstOrDefault(f => f.PortPath == cref.Path3);
                    if (match != null)
                    {
                        hubnode.CompanionHub3Node = match;
                        hubs3nodes.RemoveAll(h => h.PortPath.Equals(match.PortPath));
                    }
                    else
                        hubnode.CompanionHub3Node = null;
                }
                else
                    hubnode.CompanionHub3Node = null;

                tActiveHubs.Add(hubnode);
            }

            //detect duplicate path heads at level 0
            if (hubs3nodes.Count > 1)
            {
                // Count how many times the same path head
                var idCounts = hubs3nodes
                    .GroupBy(item => GetPathHead(item))
                    .ToDictionary(g => g.Key, g => g.Count());

                // Split into unique and duplicate lists
                uniquehubs3 = hubs3nodes
                    .Where(item => idCounts[GetPathHead(item)] == 1)
                    .ToList();

                duplicatehubs3 = hubs3nodes
                    .Where(item => idCounts[GetPathHead(item)] > 1)
                    .ToList();

                //get number of duplicate path heads in root hubs

                foreach (var hub in duplicatehubs3)
                {
                    if (hub.Level == 0) rootDuplicates++;
                }

            }

            foreach ( var hubnode in tActiveHubs)
            {
                if(hubnode.CompanionHub3Node == null)
                    if (rootDuplicates == 0)
                    {
                        hubnode.CompanionHub3Node = MatchWithUSB3Head(hubnode.Hub2Node, hubs3nodes);
                    }
                    else
                    {
                        hubnode.CompanionHub3Node = MatchWithUSB3Head(hubnode.Hub2Node, uniquehubs3);
                    }
            }

            HandleHubChanges(tActiveHubs);

            Console.WriteLine("Active Hubs:");
            foreach (var ahub in ActiveHubs)
            {
                Console.WriteLine($"[{ahub.Hub2Node.PortPath}][{ahub.CompanionHub3Node?.PortPath}][{ahub.serialConnected}][{ahub.COMName}]");
            }

            
            //update companion hub pairs Base
            UpdateHubBase(ActiveHubs);

            //ExternalDriveInfo.ForEach(x => Console.WriteLine($"[{x.Letter}][{x.DeviceID}]"));

            ActiveHubs.ForEach(hub => hub.UpdateEnumerators(ExternalDriveInfo));
            ActiveHubs.ForEach(hub => hub.PrintDevicesByPort());
            
            Console.WriteLine("\nHistory Base:");
            foreach (var list in CompanionHubBase)
            {
                Console.WriteLine($"[{list.Path2}][{list.Path3}]");
            }

            watch.Stop();
            Console.WriteLine($"Update USB Devices Execution Time: {watch.ElapsedMilliseconds} ms");
            updateUSBDevicesSemaphore = false;


        }

        static void serialUpdateController(int command = 1)
        {
            foreach (var hub in ActiveHubs)
            {
                hub.SendEnumeratorsToUIH();
            }
        }

        static void Main(string[] args)
        {
            updateUSBDevices();
            //when there is an usb event, a delay is created to wait until the usb tree is updated
            aTimer = new System.Timers.Timer();
            aTimer.Interval = 500;
            aTimer.Elapsed += OnTimedEvent;
            aTimer.AutoReset = false;

            //periodic refresh to controller
            refreshTimer = new System.Timers.Timer();
            refreshTimer.Interval = 500;
            refreshTimer.Elapsed += RefreshTimedEvent;
            refreshTimer.AutoReset = true;
            refreshTimer.Enabled = true;

            // Create a USB device watcher
            Win32UsbControllerDevices win32UsbControllerDevices = new Win32UsbControllerDevices();

            win32UsbControllerDevices.DeviceConnected += OnWin32UsbControllerDevicesDeviceConnected;
            win32UsbControllerDevices.DeviceDisconnected += OnWin32UsbControllerDevicesDeviceDisconnected;
            win32UsbControllerDevices.DeviceModified += OnWin32UsbControllerDevicesDeviceModified;

            win32UsbControllerDevices.StartWatcher();

            Console.WriteLine("USB device monitoring has started. Press any key to exit.");
            Console.ReadKey();

            // Stop monitoring USB device events before exiting
            win32UsbControllerDevices.StopWatcher();
        }

        private static void OnWin32UsbControllerDevicesDeviceConnected(Object sender, Win32UsbControllerDeviceEventArgs e)
        {
            //Console.WriteLine($"USB device connected: {e.Device.DeviceId}");
            //aTimer.Enabled = true;
            Console.WriteLine("Device Connected");
            aTimer.Stop();
            aTimer.Start();

        }

        private static void OnWin32UsbControllerDevicesDeviceDisconnected(Object sender, Win32UsbControllerDeviceEventArgs e)
        {
            //Console.WriteLine($"USB device disconnected: {e.Device.DeviceId}");
            //aTimer.Enabled = true;
            Console.WriteLine("Device Disconnected");
            aTimer.Stop();
            aTimer.Start();

        }

        private static void OnWin32UsbControllerDevicesDeviceModified(object sender, Win32UsbControllerDeviceEventArgs e)
        {
            //Console.WriteLine($"USB device modified: {e.Device.DeviceId}");
            //aTimer.Enabled = true;
            Console.WriteLine("Device Modified");
            aTimer.Stop();
            aTimer.Start();
        }

        private static void OnTimedEvent(Object source, System.Timers.ElapsedEventArgs e)
        {
            //Console.WriteLine("The Elapsed event was raised at {0}", e.SignalTime);
            Console.WriteLine("Trigger Update List");
            if (!updateUSBDevicesSemaphore)
            {
                updateUSBDevices();
            }
            else
            {
                rescheduleCount++;
                Console.WriteLine("USB Devices Update Busy - Reschedule...");

                if (rescheduleCount++ > 4) //this prevents that if updateUSBDevicesSemaphore stays in true, all the applications stops working
                {
                    Console.WriteLine("Reschedule timeout!!");
                    updateUSBDevicesSemaphore = false;
                    updateUSBDevices();
                }
                else
                {
                    aTimer.Stop();
                    aTimer.Start();
                }
            }

        }

        private static void RefreshTimedEvent(Object source, System.Timers.ElapsedEventArgs e)
        {
            if (!updateUSBDevicesSemaphore)
            {
                serialUpdateController(2); //heartbeat}
            }
        }

    }
}
