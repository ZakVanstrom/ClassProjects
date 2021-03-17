using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

// Zak Vanstrom - CSE 445 - Project 2- VS C# 2019
// March 13, 2021. Dr. Balasooriya

// Security Class
// maintains the methods for encoding and decoding the OrderClass object so as to be inserted into buffer and understood after removal

namespace FlightApp
{
    public class Security
    {
        // in a real situation, I would encrypt the string built by the encoder.
        // Encoder() builds a string to be decoded using ',' delimiters
        public static string Encoder(OrderClass order)
        {
            int senderId = order.getSenderId();
            int receiverId = order.getReceiverId();
            int amount = order.getAmount();
            string cardString = order.getCardNo();

            string builder = "";
            builder += senderId.ToString() + ",";
            builder += receiverId.ToString() + ",";
            builder += amount.ToString() + ",";
            builder += cardString;

            return builder;
        }

        // Decoder() takes in a string and parses it from what the encoded string into a OrderClass object and returns the object
        public static OrderClass Decoder(string encryptedOrder)
        {
            string[] subs = encryptedOrder.Split(',');

            // print statement for the array of strings (testing purposes)
            // for(int i = 0; i < subs.Length; i++)
            // {
            //     Console.WriteLine(subs[i]);
            // }

            int senderId = int.Parse(subs[0]);
            int receiverId = int.Parse(subs[1]);
            int amount = int.Parse(subs[2]);
            string cardNo = subs[3];

            //testing print for variables
            //Console.WriteLine($"{senderId} {receiverId} {amount} {cardNo}");

            OrderClass decodedOrder = new OrderClass();

            decodedOrder.setSenderId(senderId);
            decodedOrder.setReceiverId(receiverId);
            decodedOrder.setAmount(amount);
            decodedOrder.setCardNo(cardNo);

            return decodedOrder;
        }
    }
}
