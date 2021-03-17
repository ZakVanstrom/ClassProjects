using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

// Zak Vanstrom - CSE 445 - Project 2- VS C# 2019
// March 13, 2021. Dr. Balasooriya

// Airline Class
// Airline (server) class acts as the server and launches events when parameters are met. Airline also grabs from the Buffer to process ordres

namespace FlightApp
{
    public class Airline
    {
        public static event priceCutEvent priceCut;
        public int airlineId;

        private int ticketPrice = 180;
        private int priceCeiling = 350;
        private int priceFloor = 80;

        private int priceCutCounter = 0;

        private static object locker = new object();

        private static Random rng = new Random();

        public Airline(int id)
        {
            airlineId = id;
        }

        // ticketPrice getter and setter (one is private so only the airline can change the price)
        public int getPrice() { return ticketPrice; }
        private void setPrice(int newPrice) { ticketPrice = newPrice; }
        public int getId() { return airlineId; }

        // Changes the price to int arg and notifies priceCut if there is any subscribers
        public void changePrice(int price)
        {
            int oldPrice = getPrice();

            if (price < oldPrice && priceCut != null)
            {
                priceCut(price, oldPrice, getId());
                priceCutCounter++;
                DataCounter.PriceCutEventCount++;
                Console.WriteLine($"\nPrice Cut #{priceCutCounter} by Airline {getId()}: \n\tOld Price => {oldPrice}\n\tNew Price => {price}\n");
            }

            setPrice(price);
        }
        // PricingModel method randomly generates an integer price that's +/- 10 from current price and returns it
        public int PricingModel()
        {
            int currentPrice = getPrice();
            int newPrice;

            // randomly generates the int to be +/- 10 from current value
            newPrice = rng.Next(currentPrice - 10, currentPrice + 11);

            // bounds the price to a range set in variable above
            if (newPrice < priceFloor)
                newPrice = priceFloor;
            if (newPrice > priceCeiling)
                newPrice = priceCeiling;

            return newPrice;
        }
        // runner method launches price changes
        public void AirlineRunner()
        {
            Thread.Sleep(100);
            while(priceCutCounter < 10)
            {
                Thread.Sleep(rng.Next(400,600));
                int newPrice = PricingModel();
                changePrice(newPrice);
            }
            Thread.Sleep(3000);
        }
        // always-running CheckOrders() method continuously attempts to clear the MultiCellBuffer and process those orders
        public void CheckOrders()
        {
            OrderClass incomingOrder;
            int orderTotal = 0;
            // this counter will allow for the program to end when there is nothing in the buffer for a large number of cycles. then summary prints
            while(DataCounter.NoneFoundCounter < 500)
            {
                // lock here allows for data to be properly held without buffer getting improperly manipulated
                lock(locker)
                {
                    // attempt to grab order from MultiCellBuffer
                    incomingOrder = MultiCellBuffer.getOneCell(getId());
                    if (incomingOrder != null)
                    {
                        // set time stamp for order processing
                        DataCounter.NoneFoundCounter = 0;
                        DateTime now = DateTime.Now;
                        string accNow = now.ToString("MM/dd/yyyy hh:mm:ss.fff tt");

                        // orderprocessing processes the order and rejects it with a -1 or returns a dollar total
                        orderTotal = OrderProcessing(incomingOrder);

                        // indicates that the order was successful for a grand total of orderTotal dollars
                        if (orderTotal > 0)
                        {
                            Console.WriteLine($"Airline {getId()} confirmed an order for {incomingOrder.getAmount()} " +
                                $"tickets from Agent {incomingOrder.getSenderId()} for a total of {orderTotal} dollars at {accNow}");
                            DataCounter.OrdersProcessed++;
                        }
                        //indicates order was declied due to odd number card
                        if(orderTotal == -1)
                        {
                            Console.WriteLine($"Airline {getId()} declined an order from Agent {incomingOrder.getSenderId()} at {accNow}");
                            DataCounter.OrdersRejected++;
                        }
                    }
                    else
                    {
                        // if nothing found in buffer, then increment NoneFoundCounter, which will hit 500 and end the loop after buffer stops being filled
                        DataCounter.NoneFoundCounter++;
                    }
                }
                Thread.Sleep(25);
            }
            Thread.Sleep(100);
            DataCounter.printSummary();
            Thread.Sleep(20000);
        }

        public int OrderProcessing(OrderClass order)
        {
            // check credit card number
            string cardString= order.getCardNo();
            long cardNumber = long.Parse(cardString);
                // if number isn't even, decline it
            if ((cardNumber % 2) != 1)
                return -1;

            // calculate charge with tax
            double taxRate = 1.08;
            int amount = order.getAmount();
            double pretax = amount * this.getPrice();
            double posttax = pretax * taxRate;
            int total = (int)Math.Round(posttax);

            return total;
        }

    }
}
