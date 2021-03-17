using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

// Zak Vanstrom - CSE 445 - Project 2- VS C# 2019
// March 13, 2021. Dr. Balasooriya

// Data Counter Class
    // Keeps track of data for end of run report

namespace FlightApp
{
   // this class used to gather data from other classes and print a summary at the end
    public static class DataCounter
    {
        public static int PriceCutEventCount = 0;
        public static int OrdersFromAgentsCount = 0;
        public static int OrdersProcessed = 0;
        public static int OrdersRejected = 0;
        public static int NoneFoundCounter = 0;

        private static Boolean needsSummary = true;

        // prints summary at end (and makes sure that it only happens once, rather than once for each airline object)
        public static void printSummary()
        {
            if(needsSummary)
            {
                needsSummary = false;
                Console.WriteLine($"\n- - - - - - - - - - - -\nThere were {PriceCutEventCount} launched Price Cut Events during this run");
                Console.WriteLine($"There were {OrdersFromAgentsCount} Orders given from agents");
                Console.WriteLine($"There were {OrdersProcessed} Orders accepted by Airlines");
                Console.WriteLine($"There were {OrdersRejected} Orders rejected by Airlines");
                Console.WriteLine($"Therefore, there were {OrdersFromAgentsCount - (OrdersProcessed + OrdersRejected)} unprocessed orders! \n- - - - - - - - - - - -");
            }
        }
    }
}
