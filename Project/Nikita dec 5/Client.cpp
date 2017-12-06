#include <string>
#include <thread>
#include <iostream>
#include "socket.h"

int main(int argc, char **argv) {

    // set local IP and port
    const std::string ip("34.238.245.255");
//    const std::string ip("54.167.215.132");
//    int port(2010);
    int port(2000);

    std::cout << std::endl << "[ --- CLIENT --- ]" << std::endl;

    // declare attributes
    std::string serverResponse;
    std::string userInput = "stuff";
    Sync::ByteArray byteResponse;// raw data format

    // Create socket with IP/PORT from above
    Sync::Socket newSocket(ip, port);
    newSocket.Open();// Attempt to open created Socket

    std::cout << "Socket is Opened..." << std::endl;

    // .Read(ByteArray & buffer) = BLOCKING call...Blocks until data is received
    // Take data received and populate ByteArray param
    newSocket.Read(byteResponse);
    serverResponse = byteResponse.ToString();// convert ByteArray to String


    // display response to terminal
    //std::cout << "The response from Server is: " << std::endl;
    //std::cout << serverResponse << std::endl;

    // Loops until "done" command is input by user
    while (userInput != "done") {

        std::cout << "Enter something: ";
        std::getline(std::cin, userInput);
        std::cout << std::endl;

        // .Write(ByteArray const & buffer) = NEVER blocks
        // Convert user input into ByteArray and send thru socket
        newSocket.Write(userInput);

        if (newSocket.Read(byteResponse) <= 0) {
            std::cout << "Response is < 0...ERROR..." << std::endl;
        }

        // Display current response from Server
        serverResponse = byteResponse.ToString();
        std::cout << "The response from Server is: " << std::endl;
        std::cout << serverResponse << std::endl;
    }

    // Close the socket connection
    newSocket.Close();
    return 0;
}