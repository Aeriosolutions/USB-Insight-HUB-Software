using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics.Eventing.Reader;

namespace EnumerationExtractionAgent
{
    internal class UsbEventLogQuery
    {
        public static DateTime? GetLastUsbConnectTime(string deviceIdFilter)
        {
            string query = "*[System[Provider[@Name='Microsoft-Windows-DriverFrameworks-UserMode'] and (EventID=2003)]]";
            var logQuery = new EventLogQuery("Microsoft-Windows-DriverFrameworks-UserMode/Operational", PathType.LogName, query);

            DateTime? latestTime = null;

            try
            {
                var reader = new EventLogReader(logQuery);

                for (EventRecord eventInstance = reader.ReadEvent();
                     eventInstance != null;
                     eventInstance = reader.ReadEvent())
                {
                    string description = eventInstance.FormatDescription();

                    if (description != null &&
                        description.Contains(deviceIdFilter))
                    {
                        if (eventInstance.TimeCreated.HasValue)
                        {
                            if (!latestTime.HasValue || eventInstance.TimeCreated > latestTime)
                            {
                                latestTime = eventInstance.TimeCreated;
                            }
                        }
                    }
                }
            }
            catch (EventLogNotFoundException e)
            {
                Console.WriteLine("Event log not found: " + e.Message);
            }

            return latestTime;
        }
    }
}
