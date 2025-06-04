//USBInfoHub
//Derived from project https://github.com/vurdalakov/usbdevices


namespace USBInfoHub.DeviceFinder
{
    using System;
    using System.Management;
    using System.Runtime.CompilerServices;
    using System.Timers;
    using System.Collections.Generic; //use lists
    using System.Linq.Expressions;
    using System.IO;
    using System.IO.Ports;
    using System.Linq;
    using System.Web.Script.Serialization;
    using System.ComponentModel.Design.Serialization;

    //using Microsoft.Win32;


    class Program
    {

        public static bool updateUSBDevicesSemaphore = false; //Flags if the function is busy

        public static bool controllerisPresent = false;
        public static SerialPort _serialPort = null;
        public static bool controllerisConnected = false;
        public static int rescheduleCount = 0;
        public static String controllerFrame = "HB";
        public static String controllerFrameJSON = "";
        public static bool updateList = false;

        //public static String controllerVid = "303A"; //ESP32-S2
        //public static String controllerPid = "0002"; //ESP32-S2
        public static String controllerVid = "303A"; //ESP32-S3
        public static String controllerPid = "1001"; //ESP32-S3 
        public static String controllerHub = "";
        public static String controllerPort = "";
        public static String Hub_2Vid = "045B";
        public static String Hub_2Pid = "0209";
        public static String Hub2_DeviceID = "NONE";
        public static String Hub_3Vid = "045B";
        public static String Hub_3Pid = "0210";
        public static String Hub3_DeviceID = "NONE";

        public static String Hub2_Location = null;
        public static String Hub3_Location = null;

        private static Timer aTimer;
        private static Timer refreshTimer;

        public static ManagementScope sc;
        public static ObjectQuery query;
        public static ManagementObjectSearcher searcher;

        public class MatchedResult
        {
            public ManagementObject PNPData;
            public String hub;
            public String port;
            public String parentHubID;

            public MatchedResult()
            {
                PNPData = null;
                hub = null;
                port = null;
                parentHubID = null;
            }
        }

        public class DeviceOnPort
        {
            public String port;
            public int busType;
            public List<String> comEnumerators;
            public List<String> prevComEnumerators;
            public List<String> pnpDeviceID;

            public DeviceOnPort(String aPort)
            {
                port = aPort;
                comEnumerators = new List<String>();
                prevComEnumerators = new List<String>();
            }
            
        }

        private static DeviceOnPort[] DevicesOnPorts;

        public static void updateUSBDevices()
        {
            updateUSBDevicesSemaphore = true;
            rescheduleCount = 0;

            var watch = System.Diagnostics.Stopwatch.StartNew();

            watch.Start();

            UsbDevice[] usbDevices = UsbDevice.GetDevices();

            var builder = new UsbDeviceTreeBuilder();

            // Build the tree
            
            var roots = builder.BuildTree();


            Console.WriteLine("Full USB Device Tree:");
            
            foreach (var root in roots)
            {
                root.PrintTree(); // Recursively prints each device and its children
                //Console.WriteLine($"[{root.Description}] has {root.Children.Count} children");
            }

            foreach (var device in usbDevices)
            {
                //Debug.Print(String.Format("{0}", device.Description));
                if (String.Compare(device.Pid, controllerPid) == 0 && String.Compare(device.Vid, controllerVid) == 0 && String.Compare(device.Port,"0004")==0)
                {
                    Console.WriteLine(String.Format("ESP32 controller found on {0}:{1}", device.Hub, device.Port));
                   // Console.WriteLine(String.Format("{0}", device.Vid));
                    controllerPort = device.Port;
                    controllerHub = device.Hub;
                    Hub2_DeviceID = device.Properties[27].Value.ToString();
                    
                    foreach (var root in roots)
                    {
                        Hub2_Location = root.GetLocationFromVID_PID(Hub_2Vid, Hub_2Pid);
                        if (Hub2_Location != null) break;
                    }
                    foreach (var root in roots)
                    {
                        Hub3_Location = root.GetLocationFromVID_PID(Hub_3Vid, Hub_3Pid);
                        if (Hub3_Location != null) break;
                    }

                    controllerisPresent = true;  
                    break;
                }

                controllerisPresent = false;
                controllerisConnected = false;
                if(_serialPort!=null)
                    if(_serialPort.IsOpen) _serialPort.Close();
            }

            if(!controllerisPresent) Console.WriteLine("ESP32 Controller not Found!");

            if (controllerisPresent)
            {
                ManagementObjectCollection result = searcher.Get();                
                string pnpClass = "";
                string pnpDeviceID = "";

                bool localUpdateList = false;

                foreach(var obj in DevicesOnPorts)
                {
                    obj.comEnumerators = new List<String>();
                    obj.pnpDeviceID = new List<String>();
                }



                //Search if the Extra High speed HUB is present 
                foreach (ManagementObject obj in result)
                {
                    pnpDeviceID = obj["PNPDeviceID"].ToString();                                        

                    if (pnpDeviceID.Contains("PID_" + Hub_3Pid) && pnpDeviceID.Contains("VID_" + Hub_3Vid))
                    {                        
                        Console.WriteLine("HUB 3 Found!");
                        //If there is a connection as USB 3, the hub node is one number higher or lower than the USB 2 node.
                        Hub3_DeviceID = pnpDeviceID;
                        break;
                    }
                        
                }

                

                //Match all the elements in the result list whith the hub and port of the interfaces devices list
                //Makes the best effort to filter out the PNPEntities that have same PID and VID but are at different ports
                List<MatchedResult> matchedResult = new List<MatchedResult>();
                

                foreach (var device in usbDevices)
                {

                    int hub2Pass = String.Compare(device.Properties[27].Value.ToString(), Hub2_DeviceID, true);
                    int hub3Pass = String.Compare(device.Properties[27].Value.ToString(), Hub3_DeviceID, true);

                    if(hub2Pass==0 || hub3Pass==0)
                    {

                        List<String> thisDeviceIds = new List<String>();
                        List<String> DeviceIdsPattern = new List<String>();
                    
                        thisDeviceIds.Add(device.DeviceId);

                        foreach (var Interface in device.Interfaces)
                        {
                            thisDeviceIds.Add(Interface.InterfaceId);
                        }
                        
                        foreach (var pat in thisDeviceIds)
                        {
                            var index = pat.LastIndexOf("\\");
                            String tmp = pat.Substring(index + 1);
                            if (tmp.Length > 5)
                                tmp = tmp.Substring(0, tmp.Length - 4);
                            if(!tmp.Contains("0000")) //The last child device ends on 0000. Removing this filter breaks all
                                DeviceIdsPattern.Add(tmp);
                        }
                        
                        //Console.WriteLine("-----RAW----");
                        //DeviceIdsPattern.ForEach(Console.WriteLine);                        
                        DeviceIdsPattern = DeviceIdsPattern.Distinct().ToList(); //Remove duplicates from list
                        //Console.WriteLine("-----DEDUP----");
                        //DeviceIdsPattern.ForEach(Console.WriteLine);

                        foreach (ManagementObject obj in result)
                        {
                            pnpDeviceID = obj["PNPDeviceID"].ToString();
                            if (pnpDeviceID.Contains("PID_" + device.Pid) && pnpDeviceID.Contains("VID_" + device.Vid) )                      
                                foreach(var pat in DeviceIdsPattern)
                                //foreach(var pat in thisDeviceIds)
                                {
                                    if(pnpDeviceID.Contains(pat))
                                    {
                                        MatchedResult mtemp = new MatchedResult();
                                        mtemp.PNPData = obj;
                                        mtemp.port = device.Port;
                                        mtemp.hub = device.Hub;
                                        mtemp.parentHubID = device.Properties[27].Value.ToString();
                                        matchedResult.Add(mtemp);
                                        break;
                                    }
                                }                                                  
                        }
                    }

                }


                //-----------------------------------------------------------------------------------------------------------
                foreach (var match in matchedResult)
                {

                    //Devices in the same hub
                    //device.Properties[27].Value corresponds to the DeviceID of the hub where the device is connected
                    int hub2Pass = String.Compare(match.parentHubID, Hub2_DeviceID, true);
                    int hub3Pass = String.Compare(match.parentHubID, Hub3_DeviceID, true);

                    if (String.Compare(match.hub, controllerHub) == 0 || hub3Pass == 0)
                    {
                        //Console.WriteLine("------------------------------------------------");                      

                        if (match.PNPData["PNPClass"] != null)
                        {
                            pnpClass = match.PNPData["PNPClass"].ToString();
                            pnpDeviceID = match.PNPData["PNPDeviceID"].ToString();

                            DevicesOnPorts[portToIndex(match.port)].pnpDeviceID.Add(pnpDeviceID);

                            if (hub3Pass == 0)
                            {
                                DevicesOnPorts[portToIndex(match.port)].busType = 3;
                            }
                            else
                            {
                                DevicesOnPorts[portToIndex(match.port)].busType = 2;
                            }

                            String caption = match.PNPData["Caption"].ToString();

                            if (pnpClass.CompareTo("Ports") == 0 && match.PNPData["Caption"] != null)
                            {
                                
                                var index = caption.IndexOf("(COM");
                                var indexend = caption.LastIndexOf(")") - 1;

                                if (index != -1)
                                {
                                    //if (obj["Description"] != null) Console.WriteLine(String.Format("Found: {0} on port {1} with PID:{2} and VID:{3}", obj["Description"].ToString(), device.Port, device.Pid, device.Vid));                                            
                                    DevicesOnPorts[portToIndex(match.port)].comEnumerators.Add(caption.Substring(index + 1, indexend - index));
                                }

                            }
                            
                        }
                        
                    }
                }


                //-----------------------------------------------------------------------------------------------------------
                //Find USB devices

                var usbSearcher = new ManagementObjectSearcher(
                    "SELECT * FROM Win32_LogicalDisk WHERE DriveType = 2"
                );

                foreach (ManagementObject logicalDisk in usbSearcher.Get())
                {
                    string driveLetter = logicalDisk["Name"]?.ToString();
                    

                    if (string.IsNullOrEmpty(driveLetter))
                        continue;

                    Console.WriteLine("Removable Drive Letter: " + driveLetter);                    
                    Console.WriteLine("Device ID: " + logicalDisk["DeviceID"]?.ToString()); 

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
                                string deviceID = diskDrive["DeviceID"]?.ToString();
                                string location = "";
                                int porti = 0;
                                foreach (var r in roots)
                                {
                                    location = r.GetLocationFromID(PNPDeviceID);
                                    if (location != null)
                                    {
                                        porti = int.Parse(location.Substring(location.Length-1)) - 1;
                                        break;
                                    }
                                }

                                Console.WriteLine("  PNPDeviceID: " + PNPDeviceID);
                                Console.WriteLine("  Location: " + location + ", Index: " +porti);

                                if (location.Contains(Hub2_Location) || location.Contains(Hub3_Location)) 
                                    DevicesOnPorts[porti].comEnumerators.Add(driveLetter);
                                

                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine("Error tracing disk: " + ex.Message);
                    }
                }


                //Find USB drives https://itecnote.com/tecnote/c-how-to-get-the-drive-letter-of-usb-device-using-wmi/

                /*var usbDrives = new ManagementObjectSearcher(@"SELECT * FROM Win32_DiskDrive WHERE InterfaceType LIKE 'USB%'").Get();

                foreach (ManagementObject d in usbDrives)
                {              
                    string deviceID = (string)d.GetPropertyValue("DeviceID");
                    string PNPdeviceID = (string)d.GetPropertyValue("PNPDeviceID");

                    foreach (ManagementObject partition in new ManagementObjectSearcher(
                        "ASSOCIATORS OF {Win32_DiskDrive.DeviceID='" + d.Properties["DeviceID"].Value
                        + "'} WHERE AssocClass = Win32_DiskDriveToDiskPartition").Get())
                    {
                        foreach (ManagementObject disk in new ManagementObjectSearcher(
                                    "ASSOCIATORS OF {Win32_DiskPartition.DeviceID='"
                                        + partition["DeviceID"]
                                        + "'} WHERE AssocClass = Win32_LogicalDiskToPartition").Get())
                        {
                            //Console.WriteLine("Drive letter " + disk["Name"]);

                            var index = 0;
                            foreach (DeviceOnPort donport in DevicesOnPorts)
                            {
                                
                                if(donport.pnpDeviceID.Count != 0)
                                    for(int i = 0; i< donport.pnpDeviceID.Count; i++)
                                    {
                                        index = donport.pnpDeviceID[i].LastIndexOf("\\")+1;
                                        //Console.WriteLine(donport.pnpDeviceID[i].Substring(index));
                                        if (PNPdeviceID.Contains(donport.pnpDeviceID[i].Substring(index)))
                                            DevicesOnPorts[portToIndex(donport.port)].comEnumerators.Add(disk["Name"].ToString());
                                    }
                            }

                        }
                    }

                }*/


                foreach (var obj in DevicesOnPorts)
                {

                    if (obj.comEnumerators.Count != obj.prevComEnumerators.Count)                        
                        updateList = true;
                }

                if (localUpdateList)
                {
                    foreach (var obj in DevicesOnPorts)
                    {
                        foreach (var element in obj.comEnumerators)
                        {
                            Console.WriteLine(element);                          
                        }
                        obj.prevComEnumerators = obj.comEnumerators;

                    }
                }

                if (controllerisPresent && !controllerisConnected)
                {
                    //try to connect to the controller
                    var controllerCOM = DevicesOnPorts[portToIndex(controllerPort)].comEnumerators[0];
                    try
                    {
                        _serialPort = new SerialPort(controllerCOM, 115200, Parity.None, 8, StopBits.One);
                        _serialPort.Handshake = Handshake.None;
                        _serialPort.WriteTimeout = 500;
                        _serialPort.Open();
                        controllerisConnected = true;
                        Console.WriteLine("Connected to controller on port:{0}", controllerCOM);
                    }
                    catch(Exception ex)
                    {
                        Console.WriteLine("Port error:{0}",ex.ToString());
                        controllerisConnected = false;
                    }

                }

                //if (updateList) serialUpdateController(1);
            }
            
            watch.Stop();
            Console.WriteLine($"Update USB Devices Execution Time: {watch.ElapsedMilliseconds} ms");
            updateUSBDevicesSemaphore = false;

        }
        
        
        static void serialUpdateController(int command = 1)
        {
            
            if (controllerisConnected)
            {

                if(updateList) { 
                    string ch1enum_a = "-";
                    string ch1enum_b = "-";
                    string ch2enum_a = "-";
                    string ch2enum_b = "-";
                    string ch3enum_a = "-";
                    string ch3enum_b = "-";

                    int ch1count = DevicesOnPorts[0].comEnumerators.Count;
                    int ch2count = DevicesOnPorts[1].comEnumerators.Count;
                    int ch3count = DevicesOnPorts[2].comEnumerators.Count;

                    int ch1type = 0;
                    int ch2type = 0;
                    int ch3type = 0;

                    if (ch1count > 0) ch1type = DevicesOnPorts[0].busType;
                    if (ch2count > 0) ch2type = DevicesOnPorts[1].busType;
                    if (ch3count > 0) ch3type = DevicesOnPorts[2].busType;


                    if (ch1count >= 1)
                    { ch1enum_a = DevicesOnPorts[0].comEnumerators[0].ToString(); }
                    else { ch1enum_a = "-"; }
                    if (ch1count == 2)
                    { ch1enum_b = DevicesOnPorts[0].comEnumerators[1].ToString(); }
                    else { ch1enum_b = "-"; }
                    if (ch2count >= 1)
                    { ch2enum_a = DevicesOnPorts[1].comEnumerators[0].ToString(); }
                    else { ch2enum_a = "-"; }
                    if (ch2count == 2)
                    { ch2enum_b = DevicesOnPorts[1].comEnumerators[1].ToString(); }
                    else { ch2enum_b = "-"; }
                    if (ch3count >= 1)
                    { ch3enum_a = DevicesOnPorts[2].comEnumerators[0].ToString(); }
                    else { ch3enum_a = "-"; }
                    if (ch3count == 2)
                    { ch3enum_b = DevicesOnPorts[2].comEnumerators[1].ToString(); }
                    else { ch3enum_b = "-"; }

                    //controllerFrameJSON = "{\"action\":\"set\",\"params\":{\"CH1\":{\"Dev1_name\":\"COM1\",\"numDev\":\"0\",\"usbType\":\"0\"},\"CH2\":{\"Dev1_name\":\"COM2\",\"numDev\":\"0\",\"usbType\":\"\"}}}";

                    foreach (var obj in DevicesOnPorts)
                    {
                        if (obj.pnpDeviceID.Count != 0)
                            for (int i = 0; i < obj.pnpDeviceID.Count; i++)
                            {
                                Console.WriteLine("Port {0} PNPDev Id{1}: {2}", obj.port, i, obj.pnpDeviceID[i]);
                            }
                    }


                    controllerFrameJSON = String.Format("{{\"action\":\"set\",\"params\":{{" +
                        "\"CH1\":{{\"Dev1_name\":\"{0}\",\"Dev2_name\":\"{1}\",\"numDev\":\"{2}\",\"usbType\":\"{3}\"}}," +
                        "\"CH2\":{{\"Dev1_name\":\"{4}\",\"Dev2_name\":\"{5}\",\"numDev\":\"{6}\",\"usbType\":\"{7}\"}}," +
                        "\"CH3\":{{\"Dev1_name\":\"{8}\",\"Dev2_name\":\"{9}\",\"numDev\":\"{10}\",\"usbType\":\"{11}\"}}" +
                        "}}}}",
                        ch1enum_a, ch1enum_b, ch1count.ToString(), ch1type,
                        ch2enum_a, ch2enum_b, ch2count.ToString(), ch2type,
                        ch3enum_a, ch3enum_b, ch3count.ToString(), ch3type
                        );

                    controllerFrame = String.Format("PC,{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11}",
                                        ch1count.ToString(), ch1enum_a, ch1enum_b, ch1type,
                                        ch2count.ToString(), ch2enum_a, ch2enum_b, ch2type,
                                        ch3count.ToString(), ch3enum_a, ch3enum_b, ch3type);

                    Console.WriteLine(controllerFrameJSON);
                    updateList = false;
                }
                /*
                if (command==2)
                {
                    controllerFrame = "HB";
                }*/

                try
                {                   
                    _serialPort.WriteLine(controllerFrameJSON);
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Port error:{0}", ex.ToString());
                    controllerisConnected = false;
                    controllerisPresent = false;
                }

            }
        }

        static int portToIndex(String port)
        {
            return int.Parse(port.Substring(3))-1;
        }

        static void Main(String[] args)
        {

            DevicesOnPorts = new DeviceOnPort[4];

            for (int i = 0; i < 4; i++)
            {
                DevicesOnPorts[i] = new DeviceOnPort("000" + (i + 1).ToString());
            }
          

            //Configure the search in the device tree
            sc = new ManagementScope();
            sc.Path = new ManagementPath(@"\\" + Environment.MachineName + @"\root\CIMV2");
            query = new ObjectQuery("SELECT * FROM Win32_PnPEntity WHERE ConfigManagerErrorCode = 0");
            searcher = new ManagementObjectSearcher(sc, query);
            //each object has the following properties
            //https://learn.microsoft.com/es-es/windows/win32/cimwin32prov/win32-pnpentity

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
                } else
                {
                    aTimer.Stop();
                    aTimer.Start();
                }
            }
            
        }

        private static void RefreshTimedEvent(Object source, System.Timers.ElapsedEventArgs e)
        {
            serialUpdateController(2); //heartbeat
        }

    }
}
