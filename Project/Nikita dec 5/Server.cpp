#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "Blockable.h"
#include "thread.h"
#include "socketserver.h"
#include "socket.h"

using namespace Sync;

//============================================================
//------------------------------------------------------------
//------------------------------------------------------------
// GAME ROUND CLASS -------------------------------------------
// Each thread gets an instance of this
class gameInstance {
private:
    int playerNum = 0;// player 0 is the Host of game
    int score = 0;// gets updated as player answers correct Qs
    std::string name = "";// the name of the player
    bool winner = false;// for setting this player as current winner
    double timeLeft = 30;

public:
	gameInstance() {

	}
    //CONSTRUCTOR
    // - takes an int for the player order
    // - takes a name for the player
    gameInstance(int order, std::string name){
		this->playerNum = order;
		this->name = name;
    }

    //GETTERS SETTERS
    // gets the order number of this player
    int getPlayerNum(){
       return playerNum;
    }
    // gets this player's name
    std::string getPlayerName(){
        return name;
    }

    // get this player's score
    int getScore(){
        return this->score;
    }
    // reset this player's score for new round
    void resetScore(){
        this->score = 0;
    }

    // sets this player as the winner
    void setWinner(){
        this->winner = true;
    }
    // checks if this player is the current winner
    bool isWinner(){
        return this->winner;
    }
    // set this player as !winner (for potential new rounds)
    void setLoser() {
        this->winner = false;
    }

    // in case player order is shuffled around at new round
    int setPlayerOrder(int newOrder) {
        this->playerNum = newOrder;
    }

};


// ------------------------------------------
// GLOBAL VARIABLES ------------------------------------------
gameInstance playerInstance[4]; // for keeping track of who joined game first
int totalPlayers = 0; // for keeping track of how many players there are

//--------------------------------------------
// QUESTIONS and ANSWERS arrays
std::string questionArray [] = {
        "When referring to a computer monitor, what does the acronym LCD stand for?",
        "When talking about computer memory, what does the acronym ROM stand for?",
        "In a website browser address bar what does \"www\" stand for?",
        "In Computer science, what does \"GUI\" stand for?",
        "What does CPU stand for?",
        "What does ISP stand for?",
        "To reduce the heat produced by the main components, you would need a...?",
        "What is the name of the circuit board when it has several components pre-installed upon it?",
        "What is the name of the variable used to control access to a common resource?",
        "Which HTTP error code means \"Not Found\"?"
};
// THIS VERSION of the answers is sent to USER
std::string answerArray [] = {
        "Liquid Crystal Display",
        "Read Only Memory",
        "World Wide Web",
        "Graphical User Interface",
        "Central Processing Unit",
        "Internet Service Provider",
        "Fan",
        "Motherboard",
        "Semaphore",
        "404"
};
// COPY of ANSWER ARRAY IN LOWERCASE for checking correctness
std::string answerLCaseArray [] = {
        "liquid crystal display",
        "read only memory",
        "world wide web",
        "graphical user interface",
        "central processing unit",
        "internet service provider",
        "fan",
        "motherboard",
        "semaphore",
        "404"
};





//------------------------------------------------------------
// BIT OF CODE THAT grabs NEXT QUESTION FROM BANK
// uses a randomizer to return the next question
// TODO: Keep track of questions asked to avoid repeating questions...or not
std::string getNextQuestionPair(std::string questionPair){
    int index = 0;
    std::string stringPair = "";

    // produce rand # from 0 to 9
    srand(time(0));
    index = (rand() % 9) + 1;

    // grabs a question/Answer pair based on index (1 to 9) given
    // and tacks them together
    stringPair = questionArray[index] + "^" + answerArray[index];

    return stringPair;
}

// TODO: replace these with actual logic
std::string makeQuestionAnswerPair(int qIndex) {
	return "this is a question answer pair";
}

// TODO: replace these with actual logic
int getTimeLeft() {
	return 100;
}

//-----------------------------------------------------
// Forms an UPDATE packet string to send to SPECIFIC Player
// TO BE called inside an update loop function
// ...contains, in this order:
// - player's order #
// - player's current score
// - current Winner/Leader status
// - Next Question/Answer pair
// - Time Left in current round
std::string formPacketString(gameInstance instance, int qIndex){
    std::string packetString = "";

	int score = instance.getScore();


	packetString = std::to_string(instance.getPlayerNum())
                   + "^"
				   + std::to_string(score);
                   + "^"
				   + std::to_string(instance.isWinner())
                   + "^"
				   + makeQuestionAnswerPair(qIndex)
                   + "^"
				   + std::to_string(getTimeLeft());
}

// SET PLAYER ORDER ACCORDING TO WHO JOINED FIRST -------------------------------
// TODO: this goes inside an update loop


//-------------------------------------------------------
// RETURN INDEX OF WHO HAS HIGHEST SCORE
// - takes the order array of players of type gameInstance
std::string getWinnerScore(gameInstance playerInst[]){
    int highest = 0;
    int winnerIndex = 0;

    // cycle through instances and check who has highest score
    for(int i = 0; i < 5; i++){
        if ( playerInst[i].getScore() > highest) {
            highest = playerInst[i].getScore();
            winnerIndex = i;
        }
    }
    //reset all players to losers
    for(int i = 0; i < 5; i++){
        playerInst[i].setLoser();
    }
    // Set NEW winning player instance as current "Winner"
    playerInst[winnerIndex].isWinner();

    //return winner's order index
	return std::to_string(winnerIndex);
}

//------------------------------------------------------------
// CHECKS IF ANSWER FROM CLIENT IS CORRECT
// - takes a string for the answer and a questionIndex to know
//   which question it's referring to
bool isAnswerCorrect(std::string answer, int questionIndex){

    // compare stored lower case answer to given answer
    // returns true if answer is correct
    return answerLCaseArray[questionIndex].compare(answer);
}


//------------------------------------------------------------
//------------------------------------------------------------
//============================================================






//----------------------------------------------------------
// CLASS FOR THE MONITORING-THREAD
//class threadMonitor : public Thread {
//// TODO: this is supposed to create a SocketServer which handles the socket connections
////        -- like a server for sockets inside the Main Server...Xzibit.jpg
//    // has SocketServer object
//private: SocketServer *theSocketServer;
//private: bool *finish;

//    // CONSTRUCTOR
//public: threadMonitor(SocketServer *ss, bool *tf):Thread() {
//        theSocketServer = ss;
//        finish = tf;
//    }

//    // VIRTUAL FUNCTION inside MONITORING THREAD
//    virtual long ThreadMain() {

//        bool run = true;
//        std::string command;

//        std::cout << std::endl << "Enter command [close] to terminate server: " << std::endl;

//        // If "close" command is received from command line
//        // proceed to terminate the SocketServer
//        while (run) {
//            std::getline(std::cin, command);
//            if (command == "close") {
//                *finish = false;
//                run = false;
//            }
//        }
//        // SHUT IT DOWN D:
//        theSocketServer->Shutdown();
//        return 0;
//    }
//};

////--------------------------------------------
//// Waits for 'CLOSE' command to shut down the server
void threadMonitor(SocketServer &ss, bool &c) {
	bool run = true;
	std::string command;

    std::cout << std::endl << "--------------------------" << std::endl;
    std::cout << std::endl << "-- Running threadMonitor--" << std::endl;
	std::cout << std::endl << "Enter command [close] at any time to terminate server: " << std::endl;
    std::cout << std::endl << "--------------------------" << std::endl;

	// If "close" command is received from command line
	// proceed to terminate the SocketServer
	while (run) {
		std::getline(std::cin, command);
		if (command == "close") {
			c = false;
			run = false;
		}
	}
	// Shut down socket server
	ss.Shutdown();
}

////--------------------------------------------------
////  most GAME STUFF goes in here... Not being reached by Program at the moment
////--------------------------------------------------
void gameThread(std::vector<Socket> &sockets, bool &tf) {
	std::cout << " -- Inside gameThread -- " << std::endl;
}



////----------------------------------------------------------
////  THREAD CLASS for arbitrary # of Clients
////  - TODO: This isn't being used right now...it's been replaced by Nikita's std::thread below
////            -- this functionality needs to be implemented back somehow
class newThread : public Thread {
//class newThread : public std::thread {  // My attempt at a fix..doesn't work

private:
    std::vector<Socket*> newSocket;// declare new Socket
private:
    bool finish;

    // CONSTRUCTOR --- CREATE NEW THREAD WITH sockets and BOOL e -> socket closed, true/false?
public:
    newThread(std::vector<Socket> sockets, bool tf ) :  Thread() {

        for (Socket i : sockets) {
            sockets.push_back(i);// load the new socket into the new socket vector
        }
        finish = tf;// assign true/false to exit BOOL
    }

    // THREAD CLASS DESTROYER
public:
    ~newThread() {
        //// TODO: gotta put something in here to destroy thread properly
    }

    // FUNCTION TO CLOSE SOCKETS
public:
    void closeSocket() {
        ByteArray userInput;

        // FOR EACH SOCKET IN THE "THREAD CLASS" ATTRIBUTE 'NEWSOCKET'
        // IF THE return code isn't 0, then close the socket from this end
        for (Socket *i : newSocket) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (i->Read(userInput) != 0) {
                i->Close();
            }
        }

    }

    // LOCAL CLIENT CAN "KILL" ALL SOCKETS
public:
    bool checkKill(std::string r) {
        //checks for kill signal from client
        if (r == "kill") {
            std::string closed = "Pool's closed";
            for (int i = 0; i < 4; i++) {
                newSocket.at(i)->Write(closed);
            }
            closeSocket();
            return true;
        } else { return false; }

    }

};

////----------------------------------------------------------
////      MAIN DRIVER
int main(int argc, char **argv) {

    try {
        int port = 2000;
        bool run = true;
        std::string terminate = "";

		std::vector<std::thread> threads;
        std::vector<Socket> sockets;

        SocketServer newSocketServer(port);// make new SocketServer at port ____
        ByteArray userInput;// declare userInput of type ByteArray for raw data from Socket
        std::cout << "...finished setup..." << std::endl;
        std::cout << std::endl <<
                  std::endl <<
                  std::endl <<
                  std::endl <<
                  std::endl << "[========== SERVER is now ONLINE ==========]" << std::endl << std::endl;

        //  Thread monitoring check
//		threadMonitor monitor(newSocketServer, &run);
        std::cout << "[[ ★☆。. :*:･”ﾟ★ Running Nikita's monitor thread ★｡.:*:･”☆★ ]]" << std::endl;
		std::thread monitorThread(threadMonitor, std::ref(newSocketServer), std::ref(run));

		while (run) {
            // create new socket vector
            std::vector<Socket> newSocketVector;
            std::cout << std::endl << "-- Inside while loop in Main --" << std::endl;

            try {
                for (int i = 0; i<4; i++) {
                    // "Accept" new Socket...returns new Socket object
                    std::cout << std::endl << ">> Waiting to accept new socket..." << std::endl;
                    Socket newSocket = newSocketServer.Accept();

                    std::cout << " - Accepted a Socket connection..." << std::endl;
                    std::cout << " - About to push accepted Socket into Socket Vector..." << std::endl;
                    newSocketVector.push_back(newSocket);// put newly Accepted Socket into Socket vector

                    std::cout << " - About to push accepted Socket into 'sockets' vector..." << std::endl;
                    sockets.push_back(newSocket);// also copy it into "sockets" vector

                    // writes message to newest Socket CONNECTION saying which # they are
                    std::string message = " -- You are CONNECTION # " + std::to_string(i);
                    newSocketVector.at(i).Write(message);
                }
                // create new thread for this socket connection
//				newThread newestThread = new newThread(sockets, run);

///////TODO vvv  This line needs to be reworked...it's not doing anything right now
				std::thread newestThread(gameThread, std::ref(newSocketVector), std::ref(run));
				threads.push_back(std::move(newestThread));// add it to the "threads" vector
                std::cout << " - pushed newestThread into 'threads' vector" << std::endl;
            }
            catch (int e) {
                // socket/thread connection didn't work...
                std::string errmsg = "Something done goofed";
                std::cout << errmsg << std::endl;
                // send out error msg
                for (Socket i : newSocketVector) {
                    std::cout << "  -About to write error messages to each Socket..." << std::endl;
                    i.Write(errmsg);

                    std::this_thread::sleep_for(std::chrono::milliseconds(800));// slow down

                    // if things don't look good from the other end, close
                    if (i.Read(userInput) != 0) {
                        std::cout << "< Socket connection is missing...close from this end" << std::endl;
                        i.Close();
                    }
                }
            }


        }// end while loop

//// TODO:  Gotta fix logic here...right now, program isn't reaching this point
////        - This is what shuts down the server without CORE DUMPS

        // Terminate all threads
        //std::this_thread::sleep_for(std::chrono::milliseconds(800));
       /* for (newThread i : threads) {
            std::cout << "Thread is terminated" << std::endl;
            delete i;
        }*/
    	for (auto iter = threads.begin(); iter < threads.end(); iter++) {
			iter->join();
    		threads.erase(iter);
    	}
        // Terminate all sockets
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        /*for (Socket *i : sockets) {
            delete i;
        }*/
		for (auto iter = sockets.begin(); iter < sockets.end(); iter++) {
			sockets.erase(iter);
		}


    }
    catch (std::string e) {
        std::cout << "The socket ain't done closed good" << std::endl;
    }

    return 0;
}
