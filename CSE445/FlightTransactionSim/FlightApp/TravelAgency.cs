using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

// Zak Vanstrom - CSE 445 - Project 2- VS C# 2019
// March 13, 2021. Dr. Balasooriya

// TravelAgency Class
// Manages TravelAgency threads to work on creating orders and submitting them

namespace FlightApp
{
    public class TravelAgency
    {
        private object locker = new object();
        private static Random rng = new Random();
        public void PriceCut(int newPrice, int oldPrice, int id)
        {
            // initialize number of agent threads (adjustable to increase or decrease)
            int agentCount = 5;

            // initialize thread list with agentCount
            Thread[] agentThreadList = new Thread[agentCount];

            // create a thread, add to threadList, start the thread
            // repeat for size of agentCount
            for (int i = 0; i < agentCount; i++)
            {
                //agentThreadList[i] = new Thread(new ThreadStart(agent.handleSale));
                agentThreadList[i] = new Thread(() => HandleSale(newPrice, oldPrice, id));
                agentThreadList[i].Name = (i + 1).ToString();
                agentThreadList[i].Start();
            }
        }
        public void HandleSale(int newPrice, int oldPrice, int id)
        {
            //Console.WriteLine($"There's a new order from Agent 1! for {amountToBuy} seats from Airline {id} using {cardString}");
            int difference = oldPrice - newPrice;

            lock (locker)
            {
                // set threadname for use later
                int threadName = int.Parse(Thread.CurrentThread.Name);
                int requiredDifference = rng.Next(0, 10);

                // build a card number via a random string
                string cardString = "";
                for (int i = 0; i < 4; i++)
                    cardString += rng.Next(1000, 9999).ToString();

                // use below method to determine how many seats to buy
                int amountToBuy = evaluatePrice(difference, requiredDifference);

                if (amountToBuy == 0)
                    return;

                OrderClass newOrder = new OrderClass(threadName, id, amountToBuy, cardString);

                // timestamp
                DateTime now = DateTime.Now;
                string accNow = now.ToString("MM/dd/yyyy hh:mm:ss.fff tt");

                // encode the order
                string encodedOrder = Security.Encoder(newOrder);

                // write the attempted order to the console
                Console.WriteLine($"Agent {threadName} placed an order for {amountToBuy} tickets with Airline {id} at {accNow}");

                // attempt to insert order into buffer
                int trySet = MultiCellBuffer.setOneCell(encodedOrder);
            }  
        }
        // price evaluation determines number of tickets to buy from the price difference and the randomly generated RequiredPriceDifference
        public int evaluatePrice(int priceDifference, int requiredPriceDifference)
        {
            int numToBuy = 0;
            if(priceDifference >= requiredPriceDifference)
                numToBuy = requiredPriceDifference * priceDifference;

            return numToBuy;
        }
    }
}
