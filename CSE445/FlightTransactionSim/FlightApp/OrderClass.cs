using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

// Zak Vanstrom - CSE 445 - Project 2- VS C# 2019
// March 13, 2021. Dr. Balasooriya

// OrderClass 
// Maintains individual order information within object. can be encoded and decoded using security class.

namespace FlightApp
{
    public class OrderClass
    {
        private int senderId;
        private int receiverId;
        private int amount;
        private string cardNo;

        public OrderClass() { }
        public OrderClass(int sender , int receiver, int newAmount, string card)
        {
            senderId = sender;
            receiverId = receiver;
            amount = newAmount;
            cardNo = card;
        }

        // getters and setters for private variables in OrderClass
        public int getSenderId() { return senderId; }
        public void setSenderId(int newSenderId) { senderId = newSenderId; }

        public int getReceiverId() { return receiverId; }
        public void setReceiverId(int newReceiverId) { receiverId = newReceiverId; }

        public int getAmount() { return amount; }
        public void setAmount(int newAmount) { amount = newAmount; }

        public string getCardNo() { return cardNo; }
        public void setCardNo(string newCardNo) { cardNo = newCardNo; }

        public void printDetails()
        {
            Console.WriteLine("\n\tOrder Details\n\t- - - - - - - - - -");

            Console.WriteLine("\tSender ID: " + getSenderId());
            Console.WriteLine("\tReceiver ID: " + getReceiverId());
            Console.WriteLine("\tAmount of seats: " + getAmount());
            Console.WriteLine("\tCard Number: " + getCardNo());

            Console.WriteLine("\t- - - - - - - - - -\n");
        }
    }
}
