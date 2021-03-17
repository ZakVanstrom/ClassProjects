using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

// Zak Vanstrom - CSE 445 - Project 2- VS C# 2019
// March 13, 2021. Dr. Balasooriya

// MultiCellBuffer Class
// Manages a buffer of orders to be taken from and inserted for the Airlines and TravelAgency threads

namespace FlightApp
{
    public class MultiCellBuffer
    {
        private static int bufferSize = 3;
        private static string[] orderBuffer = new string[bufferSize];
        private static object locker = new object();
        
        public MultiCellBuffer()
        {
            for(int i = 0; i < bufferSize; i++)
            {
                orderBuffer[i] = null;
            }
        }

        // method to set order in buffer
        public static int setOneCell(string newOrder)
        {
            lock (locker)
            {
                for (int i = 0; i < bufferSize; i++)
                {
                    if (orderBuffer[i] == null)
                    {
                        orderBuffer[i] = newOrder;
                        DataCounter.OrdersFromAgentsCount++;
                        return 1;
                    }
                }
            }
            return -1;
        }

        // method to get an order from the buffer
        public static OrderClass getOneCell(int id)
        {
            OrderClass foundOrder = null;
            lock(locker)
            {
                for (int i = 0; i < bufferSize; i++)
                {
                    if (orderBuffer[i] != null)
                    {
                        OrderClass order = Security.Decoder(orderBuffer[i]);
                        if(order.getReceiverId() == id)
                        {
                            foundOrder = order;
                            orderBuffer[i] = null;
                            return foundOrder;
                        }
                    }
                }
            }
            return foundOrder;
        }
    }
}
