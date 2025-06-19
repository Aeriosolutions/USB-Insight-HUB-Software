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
using System.IO.Ports;
using System.Linq;
using UEnumerationExtractionAgent;
using static UEnumerationExtractionAgent.UsbDeviceNode;

namespace EnumerationExtractionAgent
{
    public class UsbInsightHub
    {
        public UsbDeviceNode Hub2Node;
        public UsbDeviceNode CompanionHub3Node;
        public SerialPort _serialPort = null;
        public string COMName = "";
        public bool serialConnected = false;
        List<DeviceOnPort>[] PortsInfo = new List<DeviceOnPort>[4];
        public UsbInsightHub()
        {
            ClearPortsInfo();
        }
        private void ClearPortsInfo()
        {
            for (int i = 0; i < PortsInfo.Length; i++)
            {
                PortsInfo[i] = new List<DeviceOnPort>();
            }
        }

        public class DeviceOnPort
        {
            public String ShortName;
            public String Type;
            public String HubType;
            public UsbDeviceNode PortNode;
        }


        public void Init()
        {
            if (COMName != "")
            {
                try
                {
                    _serialPort = new SerialPort(COMName, 115200, Parity.None, 8, StopBits.One);
                    _serialPort.Handshake = Handshake.None;
                    _serialPort.WriteTimeout = 500;
                    _serialPort.ReadTimeout = 500;
                    _serialPort.Open();
                    _serialPort.DtrEnable = true; //use DTR if serial is read back. Disable if only is written
                    serialConnected = true;
                    //Console.WriteLine($"Connected to port:{COMName}");
                }
                catch (Exception ex)
                {
                    //Console.WriteLine($"Port {COMName} error:{ex.ToString()}");
                    serialConnected = false;
                }
            }
        }
        public void Deinit()
        {
            if (_serialPort != null)
            {
                if (_serialPort.IsOpen) _serialPort.Close();
            }
        }

        public void UpdateEnumerators(List<ExternalDriveInfo> driveInfo)
        {
            ClearPortsInfo();
            foreach (var port in Hub2Node.Children)
            {
                if (port != null)
                {
                    AddInstancesOf(port, int.Parse(port.Port), "2", driveInfo);
                }
            }

            if (CompanionHub3Node != null)
            {
                foreach (var port in CompanionHub3Node.Children)
                {
                    if (port != null)
                    {
                        AddInstancesOf(port, int.Parse(port.Port), "3", driveInfo);
                    }
                }
            }

        }

        //check all the references that match a filter and adds to the corresponding PortInfo list
        private void AddInstancesOf(UsbDeviceNode node, int portnum, String hubtype, List<ExternalDriveInfo> driveInfo)
        {

            String tShortName = "";
            String filter = "";

            if (node.Description.IndexOf("Hub", StringComparison.OrdinalIgnoreCase) >= 0)
            {
                tShortName = "Hub " + hubtype;
                filter = "Hub";
            }

            else if (node.Description.IndexOf("COM") >= 0)
            {
                try
                {
                    var desc = node.Description;
                    var iStart = desc.IndexOf("(COM");
                    var iEnd = desc.LastIndexOf(")") - 1;

                    if (iStart != -1)
                    {
                        tShortName = desc.Substring(iStart + 1, iEnd - iStart);
                        filter = "COM";
                    }
                }
                catch { }
            }
            else if (driveInfo.FirstOrDefault(f => f.DeviceID == node.InstanceId) != null)
            {
                var d = driveInfo.FirstOrDefault(f => f.DeviceID == node.InstanceId);
                tShortName = d.Letter;
                filter = "DISK";
            }
            else if (node.Description.IndexOf("HID", StringComparison.OrdinalIgnoreCase) >= 0)
            {
                if (node.Description.IndexOf("mouse", StringComparison.OrdinalIgnoreCase) >= 0) tShortName = "HID-MOU";
                else if (node.Description.IndexOf("keyboard", StringComparison.OrdinalIgnoreCase) >= 0) tShortName = "HID-KB";
                else tShortName = "HID-x";
                filter = "HID";
            }
            else
            {
                if(node.Children.Count == 0)
                {
                    tShortName = node.Description.Replace("Generic","");
                }
            }

            if (tShortName != "")
            {
                var t = new DeviceOnPort { ShortName = tShortName, Type = filter, HubType = hubtype, PortNode = node };
                PortsInfo[portnum - 1].Add(t);
                if (filter == "Hub")
                    return;
            }
                        
            for(int i = 0; i< node.Children.Count; i++)
            {
                AddInstancesOf(node.Children[i], portnum, hubtype,driveInfo);
            }                                    
        }



        public void PrintDevicesByPort()
        {
            Console.WriteLine($"\nHub[{Hub2Node.PortPath}]");
            for (int i=0; i<3; i++)
            {
                Console.Write($"Port {i+1}:");
                PortsInfo[i].ForEach(x => Console.Write($"[{x.ShortName}-USB{x.HubType}]"));
            }
        }

        private String Truncate(String s)
        {
            s = s.Length > 6 ? s.Substring(0, 6) : s;
            s = s.PadRight(6, ' ');
            return s; 
        }

        public void SendEnumeratorsToUIH()
        {
            if(serialConnected && _serialPort.IsOpen)
            {
                string controllerFrameJSON = "";

                if (PortsInfo.Length != 0)
                {
                    //translation to old format
                    String[][] tr = new String[4][];

                    for (int i = 0; i<4; i++)
                    {
                        tr[i] = new String[] { String.Format("\"-\""), String.Format("\"-\""), "0", "0"};
                    }
                    
                    for(int j =0 ; j<4; j++)
                    {                        
                        if (PortsInfo[j].Count>0 && PortsInfo[j].Count <= 2)
                        {
                            for (int k = 0; k < PortsInfo[j].Count; k++)
                            {
                                var s = PortsInfo[j][k].ShortName;
                                s = s.Length > 7 ? s.Substring(0, 7) : s;
                                tr[j][k] = String.Format("\"{0}\"", s);  
                            }

                            tr[j][2] = PortsInfo[j].Count.ToString();
                            tr[j][3] = PortsInfo[j][0].HubType;
                        }
                        if (PortsInfo[j].Count >= 3)
                        {
                            if (PortsInfo[j].Count == 3)
                            {
                                tr[j][0] = String.Format("{{\"T1\":{{\"txt\":\"{0}\",\"align\":\"center\"}}," +
                                                           "\"T2\":{{\"txt\":\"{1}\",\"align\":\"center\"}}," +
                                                           "\"T3\":{{\"txt\":\"{2}\",\"align\":\"center\"}}}}",
                                                           PortsInfo[j][0].ShortName, PortsInfo[j][1].ShortName, PortsInfo[j][2].ShortName
                                                           );

                            }
                            if (PortsInfo[j].Count == 4)
                            {
                                tr[j][0] = String.Format("{{\"T1\":{{\"txt\":\"{0},{2}\",\"align\":\"center\"}}," +
                                                           "\"T2\":{{\"txt\":\"{1},{3}\",\"align\":\"center\"}}}}",                                                           
                                                           Truncate(PortsInfo[j][0].ShortName), Truncate(PortsInfo[j][1].ShortName),
                                                           Truncate(PortsInfo[j][2].ShortName), Truncate(PortsInfo[j][3].ShortName)
                                                           );
                            }
                            if (PortsInfo[j].Count == 5)
                            {
                                tr[j][0] = String.Format("{{\"T1\":{{\"txt\":\" {0},{3}\",\"align\":\"left\"}}," +
                                                           "\"T2\":{{\"txt\":\" {1},{4}\",\"align\":\"left\"}}," +
                                                           "\"T3\":{{\"txt\":\" {2}\",\"align\":\"left\"}}}}",
                                                           Truncate(PortsInfo[j][0].ShortName), Truncate(PortsInfo[j][1].ShortName),
                                                           Truncate(PortsInfo[j][2].ShortName), Truncate(PortsInfo[j][3].ShortName),
                                                           Truncate(PortsInfo[j][4].ShortName)
                                                           );
                            }
                            if (PortsInfo[j].Count == 6)
                            {
                                tr[j][0] = String.Format("{{\"T1\":{{\"txt\":\" {0},{3}\",\"align\":\"center\"}}," +
                                                           "\"T2\":{{\"txt\":\" {1},{4}\",\"align\":\"center\"}}," +
                                                           "\"T3\":{{\"txt\":\" {2},{5}\",\"align\":\"center\"}}}}",
                                                           Truncate(PortsInfo[j][0].ShortName), Truncate(PortsInfo[j][1].ShortName),
                                                           Truncate(PortsInfo[j][2].ShortName), Truncate(PortsInfo[j][3].ShortName),
                                                           Truncate(PortsInfo[j][4].ShortName), Truncate(PortsInfo[j][5].ShortName)
                                                           );
                            }
                            if (PortsInfo[j].Count > 6)
                            {
                                tr[j][0] = String.Format("{{\"T1\":{{\"txt\":\" {0},{3}\",\"align\":\"center\"}}," +
                                                           "\"T2\":{{\"txt\":\" {1},{4}\",\"align\":\"center\"}}," +
                                                           "\"T3\":{{\"txt\":\" {2}, +{5}\",\"align\":\"center\"}}}}",
                                                           Truncate(PortsInfo[j][0].ShortName), Truncate(PortsInfo[j][1].ShortName),
                                                           Truncate(PortsInfo[j][2].ShortName), Truncate(PortsInfo[j][3].ShortName),
                                                           Truncate(PortsInfo[j][4].ShortName), PortsInfo[j].Count-5
                                                           );
                            }


                            tr[j][2] = "10";
                            tr[j][3] = PortsInfo[j][0].HubType;
                        }



                        if (PortsInfo[j].Count >= 4 && PortsInfo[j].Count <= 6)
                        {
                            String[][] line = new String[3][];
                            for (int k = 0 ; k< 3; k++)
                            {
                                line[k] = new String [] { "-", "-" };
                            }

                        }

                    }

                    controllerFrameJSON = String.Format("{{\"action\":\"set\",\"params\":{{" +
                        "\"CH1\":{{\"Dev1_name\":{0},\"Dev2_name\":{1},\"numDev\":\"{2}\",\"usbType\":\"{3}\"}}," +
                        "\"CH2\":{{\"Dev1_name\":{4},\"Dev2_name\":{5},\"numDev\":\"{6}\",\"usbType\":\"{7}\"}}," +
                        "\"CH3\":{{\"Dev1_name\":{8},\"Dev2_name\":{9},\"numDev\":\"{10}\",\"usbType\":\"{11}\"}}" +
                        "}}}}",
                        tr[0][0], tr[0][1], tr[0][2], tr[0][3],
                        tr[1][0], tr[1][1], tr[1][2], tr[1][3],
                        tr[2][0], tr[2][1], tr[2][2], tr[2][3]
                        );
                }
                else
                    controllerFrameJSON = String.Format("{{ \"action\":\"set\",\"params\":{{ \"ledState\":\"false\"}}}}");

                try
                {
                    //Console.WriteLine(controllerFrameJSON);
                    _serialPort.WriteLine(controllerFrameJSON);
                }
                catch(Exception ex)
                {
                    Console.WriteLine($"Port {COMName} error:{ex.ToString()}");
                    serialConnected = false;
                }
                try
                {                                        
                    var res = _serialPort.ReadLine();
                    //Console.WriteLine(res);
                }
                catch (Exception ex)
                {

                }

            }
            else
            {   
                //Try to reestablish communication
                Init();
            }
        }
        
    }
}
