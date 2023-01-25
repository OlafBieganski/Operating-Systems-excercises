#include <iostream>
#include <openssl/evp.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <csignal>
#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>

#define THREAD_NR 3 // number of searching threads

std::vector<std::string> dictionary, hash, emails;
std::vector<std::array<std::string, 3>> passwdPairs; // first is pswd second hash, third email
bool passwdFound = false; // flags  for threads
bool continueFlag = true;
bool isInput = false;
bool threadsFinished = false;
std::array<std::thread, THREAD_NR> threadArr;
std::array<bool, THREAD_NR> thread_state; // inform if threads finished with no result
std::string userInput;
std::mutex passwdFound_mtx, passwdPairs_mtx, userInput_mtx, dict_mtx, hashEmail_mtx, cont_mtx, threads_finished_mtx;
std::condition_variable newPasswd_CV, newInput_CV, threadsFinished_CV;

// function to peform conversion from char array to hash code array
void bytes2md5(const char *data, int len, char *md5buf) {
	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	const EVP_MD *md = EVP_md5();
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len, i;
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, data, len);
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_free(mdctx);
	for (i = 0; i < md_len; i++) {
		snprintf(&(md5buf[i * 2]), 16 * 2, "%02x", md_value[i]);
	}
}

// signal handling for SIGHUP
void signalHandler(int sigcode ) {
	std::cout << "Interrupt signal (" << sigcode << ") received.\n";
	// signal reaction code
	if(passwdPairs.empty()){
		std::cout << "No password has been found so far.\n";
		return;
	}
	for(const auto &arr : passwdPairs)
		std::cout << "The deciphered password is: " << arr[0]
		 << ". Associated email: " << arr[2] << std::endl;  
}

// perform mutex protected read of var
template<typename T>
const T mtx_protrd(std::mutex &mtx, const T &var){
	T temp;
	mtx.lock();
	temp = var;
	mtx.unlock();
	return temp;
}

// perform mutex protecetd write to var
template<typename T>
void mtx_protwrt(std::mutex &mtx, T &var, const T &newVal){
	mtx.lock();
	var = newVal;
	mtx.unlock();
}

const std::string readUserInput(){
	userInput_mtx.lock();
	const std::string temp = userInput;
	userInput_mtx.unlock();
	return temp;
}

void getInput(){
	std::string tempInput;
	std::cout << "Provide file with passwords or \"q\" for exit. \n";
	while(userInput != "q"){
		// get whole line from input
		std::getline(std::cin, tempInput);
		std::unique_lock<std::mutex> lock(userInput_mtx);
		userInput = tempInput;
		isInput = true;
		newInput_CV.notify_one();
		lock.unlock(); // may cause an error?
	}
}

void checkPswd(const std::string &currWord){
	if(hash.empty()) return;
	char md5[33]; // 32 characters + null terminator
	const char *word = currWord.c_str();
	bytes2md5(word, strlen(word), md5);
	for(auto it = hash.begin(); it != hash.end(); ){
		if(*it == md5){
			uint idx = std::distance(hash.begin(), it);
			std::array<std::string, 3> newPswd = {currWord, hash[idx], emails[idx]};
			//delete hash and email from list
			hashEmail_mtx.lock();
			it = hash.erase(it);
			emails.erase(emails.begin() + idx);
			hashEmail_mtx.unlock();
			// inform consumer that passwd found via condVar
			std::unique_lock<std::mutex> lock(passwdFound_mtx);
			passwdPairs.push_back(newPswd);
			passwdFound = true;
			newPasswd_CV.notify_one();
			if(hash.empty()){
				// stop threads
				mtx_protwrt(cont_mtx, continueFlag,false);
				// notify inform thread
				threadsFinished_CV.notify_one();
				std::cout << "End of the password list. Provide new file." << std::endl;
				return;
			}
			lock.unlock(); // may cause an error?
		}
	}
}

enum class producerNR {zero, one, two};
enum class digitPlace {noDigit, prefixDigit, postfixDigit, middleDigit, prePostDigit};
std::array<std::string,16> symbols = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
 "!", "@", "#", "$", "%", "&"};

//
/*void producer1word(const std::string &_crrhash, producerNR _producer){

	for(const std::string &x : dictionary){

		std::string word = mtx_protrd(dict_mtx, x);

		// change the processed word based on producer type
		switch(_producer){
			case producerNR::one:
				word[0] = toupper(word[0]);
				break;
			case producerNR::two:
				for(auto &letter : word)
					letter = toupper(letter);
				break;
		}

		if(checkPswd(word, _crrhash)) return;

		// check all 1 digit pre and post combinations
		for(int i = 0; i <= 9; i++){
			auto prefix = std::to_string(i) + word;
			if(checkPswd(prefix, _crrhash)) return;
			auto wordpost = word + std::to_string(i);
			if(checkPswd(wordpost, _crrhash)) return;
			for(int j = 0; j <= 9; j++){
				if(!continueFlag || passwdFound) return;
				auto preword = std::to_string(j) + wordpost;
				if(checkPswd(preword, _crrhash)) return;
			}
		}

		// all pre and post 2 digit combinations
		for(int i = 0; i <= 99; i++){
			auto x = std::to_string(i);
			if(i < 10) x = "0" + x;
			auto prefix = x + word;
			if(checkPswd(prefix, _crrhash)) return;
			auto wordpost = word + x;
			if(checkPswd(wordpost, _crrhash)) return;
			for(int j = 0; j <= 99; j++){
				if(!continueFlag || passwdFound) return;
				auto y = std::to_string(j);
				if(j < 10) y = "0" + y;
				auto preword = y + wordpost;
				if(checkPswd(preword, _crrhash)) return;
			}
		}

		// all pre and post 3 digit combinations
		for(int i = 0; i <= 999; i++){
			auto x = std::to_string(i);
			if(i < 10) x = "00" + x;
			if(i < 100) x = "0" + x;
			auto prefix = x + word;
			if(checkPswd(prefix, _crrhash)) return;
			auto wordpost = word + x;
			if(checkPswd(wordpost, _crrhash)) return;
			for(int j = 0; j <= 999; j++){
				if(!continueFlag || passwdFound) return;
				auto y = std::to_string(j);
				if(j < 10) y = "0" + y;
				if(j < 100) y = "0" + y;
				auto preword = y + wordpost;
				if(checkPswd(preword, _crrhash)) return;
			}
		}
	}

	// if no passwd found in this or other thread
	// setting state as finished
	for(auto &state : thread_state)
		if(state == false){
			state = true;
			break;
		}
}*/

// possible check for 2 word passwd with all letter size cominations
// and pre, post and middle additional numbers up to 2 symbols gigit and special signs combinations
void producer2word(producerNR _producer, digitPlace _digitPlace){
	for(const std::string &x : dictionary){

		std::string word1 = mtx_protrd(dict_mtx, x);
		// first word letter size based on thread argument
		switch(_producer){
			case producerNR::one:
				word1[0] = toupper(word1[0]);
				break;
			case producerNR::two:
				for(auto &letter : word1)
					letter = toupper(letter);
				break;
		}

		for(const std::string &y : dictionary){
			// all possible letter size combinations for second word
			if(!mtx_protrd(cont_mtx, continueFlag)) return;
			std::string word2 = mtx_protrd(dict_mtx, y);
			auto word2FUP = word2;
			word2FUP[0] = toupper(word2[0]);
			auto word2AUP = word2;
			for(auto &letter : word2AUP)
				letter = toupper(letter);
			// adding digits
			switch(_digitPlace){
				case digitPlace::noDigit: break;
				case digitPlace::prefixDigit:
					for(auto &sym : symbols){
						auto prefix = sym;
						for(auto &sym2 : symbols){
							if(!mtx_protrd(cont_mtx, continueFlag)) return;
							prefix = prefix + sym2;
							checkPswd(prefix + word1 + word2);
							checkPswd(prefix + word1 + word2FUP);
							checkPswd(prefix + word1 + word2AUP);
						}
					}
				break;
				case digitPlace::postfixDigit:
					for(auto &sym : symbols){
						auto postfix = sym;
						for(auto &sym2 : symbols){
							if(!mtx_protrd(cont_mtx, continueFlag)) return;
							postfix = postfix + sym2;
							checkPswd(word1 + word2 + postfix);
							checkPswd(word1 + word2FUP + postfix);
							checkPswd(word1 + word2AUP + postfix);
						}
					}
				break;
				case digitPlace::middleDigit:
					for(auto &sym : symbols){
						auto middle = sym;
						for(auto &sym2 : symbols){
							if(!mtx_protrd(cont_mtx, continueFlag)) return;
							middle = middle + sym2;
							checkPswd(word1 + middle + word2);
							checkPswd(word1 + middle + word2FUP);
							checkPswd(word1 + middle + word2AUP);
						}
					}
				break;
				case digitPlace::prePostDigit:
					for(uint i = 0; i < 100; i++){
						auto prefix = std::to_string(i);
						for(uint i = 0; i < 100; i++){
							if(!mtx_protrd(cont_mtx, continueFlag)) return;
							auto postfix = std::to_string(i);
							checkPswd(prefix + word1 + word2  + postfix);
							checkPswd(prefix + word1 + word2FUP + postfix);
							checkPswd(prefix + word1 + word2AUP + postfix);
						}
					}
				break;
			}
			// check all three options
			if(_digitPlace != digitPlace::noDigit) break;
			checkPswd(word1 + word2);
			checkPswd(word1 + word2FUP);
			checkPswd(word1 + word2AUP);
		}
	}
}

template<std::size_t SIZE>
void stopThreads(std::array<std::thread, SIZE> &thArr){
	// stop all threads
	continueFlag = false;
	//wait a litlle bit
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	// joining threads to terminate them
	for(int i = 0; i < SIZE; i++)
		if(threadArr[i].joinable())
			threadArr[i].join();
}

bool threadNoResult(){
	for(auto &state: thread_state)
		if(!state) return false;
	// if previously no found, we have no result
	return !passwdFound;
}

// before use of this function you have to stop all running threads
// and run utility threads
void threadStartnewSearch(){
	for(auto &state : thread_state) state = false;
	threadArr[0] = std::thread(producer2word, producerNR::zero, digitPlace::prePostDigit);
	//threadArr[1] = std::thread(producer2word, producerNR::one, digitPlace::prefixDigit);
	//threadArr[2] = std::thread(producer2word, producerNR::two, digitPlace::postfixDigit);
}

// sets thread's state as finished after work is done
// and notify suitable thread if all have finished
void threadSetState(){
	// set state as work finished
	for(auto &state : thread_state)
		if(state == false){
			state = true;
			break;
		}
	// if still false values do not notify
	for(auto &state : thread_state)
		if(state == false) return;
	// if all threads finished notify about it
	std::unique_lock<std::mutex> lock(threads_finished_mtx);
	threadsFinished = true;
	threadsFinished_CV.notify_one();
}

void consumer(){
	// setting initial state
	continueFlag = true;
	passwdFound = false;
	// while threads run check for new passwd
	while(continueFlag){
		std::unique_lock<std::mutex> lock(passwdFound_mtx);
		if(!passwdFound){
			newPasswd_CV.wait(lock);
		}
		std::cout << "The deciphered password is: " << passwdPairs.back()[0]
			<< ". Associated email: " << passwdPairs.back()[2] << std::endl;
		passwdFound = false;
	}
}

// has to be called with working threads each time new passwds provided
void informThreadsFinish(){
	threadsFinished = false;
	std::unique_lock<std::mutex> lock(threads_finished_mtx);
	if(!threadsFinished){
		threadsFinished_CV.wait(lock);
	}
	if(continueFlag) return; // if threads finished cause of contFlag do not print
	std::cout << "All threads finished its work. Not all passwords have been found. Provide new file" << 
		" or \"q\" for exit." << std::endl;
}

int main(int argc, char* argv[]){
	// enable signal handling
	signal(SIGHUP, signalHandler);

	std::ifstream passwdFile;
	char *filename;

	// Check if dictionary file is given
    if (argc < 2){
        std::cout << "Argument 1 not specified. Filename required!\n";
        exit(-1);
    }
	// read dictionary to memory
	std::string dictName = argv[1];
	std::ifstream dictFile(dictName, std::ios::out);
	if(!dictFile){
		std::cout << "Invalid dictionary file argument." << std::endl;
		exit(-1);
	}
	if(dictFile.is_open()){
		// we assume each line is new word
		std::string line;
		while(dictFile){
			std::getline(dictFile, line);
			dictionary.push_back(line);
		}
		std::cout << "Dictionary " << dictName << " successfuly read to memory."
		 << std::endl;
	}
	
	// start user input handling thread
	std::thread inputThread(getInput), consumerThread, informFinish;

	while(true){
		std::unique_lock<std::mutex> lock(userInput_mtx);
		if(!(isInput)){
			newInput_CV.wait(lock);
		}
		if(userInput == "q"){
			// before exit stop all threads
			stopThreads(threadArr);
			if(inputThread.joinable()) inputThread.join();
			if(consumerThread.joinable()) consumerThread.join();
			if(informFinish.joinable()) informFinish.join();
			break;
		}
		passwdFile.open(userInput, std::ios::out);
		if(!passwdFile){
			std::cout << "No file of such name in current directory. Provide correct filename."
			 << std::endl;
		}
		if(passwdFile){
			// stop threads' execution
			stopThreads(threadArr);
			if(consumerThread.joinable()) consumerThread.join();
			if(informFinish.joinable()) informFinish.join();
			// clear data structures
			hash.clear();
			emails.clear();
			passwdPairs.clear(); 
			// read new hashed passwords to memory
			std::string lineHash, lineEmail, trash;
			if(passwdFile.is_open()) {
				do{
					passwdFile >> trash >> lineHash >> lineEmail;
					std::getline(passwdFile, trash);
					hash.push_back(lineHash);
					emails.push_back(lineEmail);
				}while(passwdFile);
				std::cout << "Passwords and emails successfuly read to memory. Starting new search." << std::endl;
				passwdFile.close();
			}
			isInput = false;
			lock.unlock(); // here?
			// start threads to find passwords
			consumerThread = std::thread(consumer);
			informFinish = std::thread(informThreadsFinish);
			threadStartnewSearch();
		}
	}
    return 0;
}