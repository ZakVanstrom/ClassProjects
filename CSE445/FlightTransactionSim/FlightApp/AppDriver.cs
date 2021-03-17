using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

// Zak Vanstrom - CSE 445 - Project 2 - VS C# 2019
// March 13, 2021. Dr. Balasooriya

// AppDriver Class
    // Driver class with main method. instantiates all other types. starts the threads.
namespace FlightApp
{
    public delegate void priceCutEvent(int price, int oldPrice, int id);
    public class AppDriver
    {
        static void Main(string[] args)
        {

            MultiCellBuffer buffer = new MultiCellBuffer();

            // instantiate and initialize the
            // includes a thread for the running of the system and for grabbing from the buffer
            Airline airline1 = new Airline(1);
            Thread airline1Runner = new Thread(new ThreadStart(airline1.AirlineRunner));
            airline1Runner.Start();
            Thread airline1Listener = new Thread(new ThreadStart(airline1.CheckOrders));
            airline1Listener.Start();

            Airline airline2 = new Airline(2);
            Thread airline2Runner = new Thread(new ThreadStart(airline2.AirlineRunner));
            airline2Runner.Start();
            Thread airline2Listener = new Thread(new ThreadStart(airline2.CheckOrders));
            airline2Listener.Start();

            // creates first (and only) object of TravelAgency
            TravelAgency agent = new TravelAgency();

            // subscribes the agent to the priceCut event
            Airline.priceCut += new priceCutEvent(agent.PriceCut);
        }
    }
}

//    My Written Requirements
//Operation
//1.Airline uses pricing model to calculate ticket price
//    1. if new price<old price, emit promo event & call event handlers in travel agencies that subscribed
//2. TravelAgency evaluates price, generates OrderObject and sends order to Encoder
//3. Encoder converts Order into string
//4. Encoder sends back to TravelAgency (caller)
//5. TravelAgency sends encoded string to free cell in MultiCellBuffer
//6. Airline receives encoded string from MultiCellBuffer and sends string to Decoder
//7. Decoder sends OrderObject to Airline with same values as before encoding
//8. Airline creates new thread to process order
//9. OrderProcessingThread processes order (does checks & calculates totals)
//10.OrderProcessingThread sends confirmation to travel agency and prints order

//Components
//Airline (1…n) 
//objects of a class on the airline side
//2 objects of Airline Class
//each object has method to be started as a thread by main method and will perform # of functions
//uses PricingModel to determine seat prices
//defines price-cut event that emits an event and calls event handlers in TravelAgency’s if there is a price-cut according to pricing Model
//Event handler will send the new price and the airline ID
//receives orders in a string from the MultiCellBuffer, calls decoder to convert the string into order object
//for each order, use existing thread or start new thread from OrderProcessing class (or method) to process the order based on the current price
//counter p in airline —> after p (eg p = 10), price cuts have been made, airline thread terminates
//unlimited seats with airline
//k = 2 (two airlines, and each airline has own ID)
//Pricing Model
//class or method in airline class
//Method —> PricingModel()
//decides price of seat
//increases and decreases price
//has mathematical model to determine price based on the order received within given time period and number of seats available in the airline in the same time period
//can use hard-coded table of the price in each week day
//model will allow the price to go up some times, and down other times
//Order Processing
//class or method in class
//ProcessOrder()
//on airline side (server side)
//when order needs processing, new thread instantiated from this class to process order
//check validity of credit card number (self defined credit card format)
//each thread calculates total amount for charge (unitPrice * NoOfSeats + Tax)
//if CC invalid, decline order
//TravelAgency (1…n)
//n threads instantiated from the travel agency class
//5 threads of TravelAgency Class
//event-driven actions
//call-back method (event handler)
//calculate number of seats to order based on need and difference between previous price and current price
//each order is OrderClass object
//object sent to Encoder for encoding
//encoded string sent back to travel agency
//then travel agency sends order in string format to multicellbuffer
//before doing so, save time stamp
//when confirmation received, time of order calculated and saved (or printed)
//set n = 5
//terminates after airline thread terminates
//OrderClass
//Class
//		senderId: identity of sender (travel agent ID) -(thread name or thread id)
//		cardNo: integer for credit card number

//        receiverID: identity of receiver(thread name or unique name for Airline)
//    amount: int for # of seats to order
//use public methods to set and get above private data
//decide if these need to be synchronized
//instances from this class are the OrderObjects
//MultiCellBuffer
//Class
//communication between travel agencies (clients) and the airlines (server)
//n data cells (n=3)
//# cells available must be < max # of travel agencies
//setOneCell(), getOneCell() to read and write data from available cells
//keeps orders to both Airlines
//each Airline reads orders intended for that Airline only
//if Airline reads order intended for other Airline, do not read from multi-cell buffer
//Encoder
//Class or method in Class
//Method —> Encode()
//convert OrderObject into string
//choose any way to do so, as long as it can be decoded back into OrderObject
//Decoder
//Class or method in a Class
//Method —> Decode()
//convert encoded string back into OrderObject
//Main
//Perform necessary prep
//Create buffer classes
//Instantiate objects
//create threads
//start threads

//Expected Output
//In Console display following information:
//-Price Cut Events
//- Travel Agents Placing Orders
//- Airlines Processing order
//- Confirmation/Decline after airline processes order
//- summary of
//    - number of price cut events
//    - number of orders from travel agents
//    - number of orders successfully processed by airlines
//    - number of orders rejected
