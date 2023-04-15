#include <iostream>
#include <fstream>
#include <math.h>
//#include <bits/stdc++.h> 
using namespace std;


int main() {

	//creates and opens linearLatency file to write averages to
	ofstream linearLatency;
	linearLatency.open("linearLatency.txt");

	//loop for number of devices starting at 100 and incriimenting by 100 to 6000
	for (int number_of_devices = 100; number_of_devices <= 6000; number_of_devices += 100) {

		int avg_latency = 0; //initializes the the average latency variable

		//loop to test the backoff 10 times
		for (int trial = 0; trial < 10; trial++) {

			int devices = number_of_devices; //number of devices per trial that will becrimented
			int windows_size = 2; //initializes the window size
			int latency = 0; //initializes the latency variable for each trial

			//loop for iterating through the backoff until all devices have succeeded
			while (devices > 0) {

				int Device_Choices[windows_size]; //creates an array to store number of devices selecting a slot
				memset(Device_Choices, 0, sizeof(Device_Choices)); //initializes all variables in the array to 0

				//generates a random number for each device that has not successfully sent and incriments that array index
				for (int i = 0; i < devices; i++) {

					int random_var = rand() % windows_size; //generates a random number based on the window size
					Device_Choices[random_var] = Device_Choices[random_var] + 1; //incriments the slot based upon the random variable selected
				}

				//loop to check if any devices successfully sent
				for (int i = 0; i < windows_size; i++) {

					//checks if only 1 device is in a given slot
					if (Device_Choices[i] == 1) {

						devices--; //decreases the number of devices to still send

						//if no devices are left the latency of the run is the slot of the last device to send plus all other windows
						if (devices == 0) {

							latency += i; //increases the latency
						}
					}
				}

				//if devices are still left to send increase the latency by the current windo size
				if (devices > 0) {

					latency += windows_size; //increases the latency
				}

				windows_size++; //increases the window size by 1
			}

			avg_latency += latency; //adds the current trial latency to the average latency variable
		}

		linearLatency << avg_latency / 10 << "\n"; //adds the average latency of 10 trials per number of devices to a file
	}

	linearLatency.close(); //closes the text file



	//creates and opens binaryLatency file to write averages to
	ofstream binaryLatency;
	binaryLatency.open("binaryLatency.txt");

	//loop for number of devices starting at 100 and incriimenting by 100 to 6000
	for (int number_of_devices = 100; number_of_devices <= 6000; number_of_devices += 100) {

		int avg_latency = 0; //initializes the the average latency variable

		//loop to test the backoff 10 times
		for (int trial = 0; trial < 10; trial++) {

			int devices = number_of_devices; //number of devices per trial that will becrimented
			int windows_size = 2; //initializes the window size
			int latency = 0; //initializes the latency variable for each trial

			//loop for iterating through the backoff until all devices have succeeded
			while (devices > 0) {

				int Device_Choices[windows_size]; //creates an array to store number of devices selecting a slot
				memset(Device_Choices, 0, sizeof(Device_Choices)); //initializes all variables in the array to 0

				//generates a random number for each device that has not successfully sent and incriments that array index
				for (int i = 0; i < devices; i++) {

					int random_var = rand() % windows_size; //generates a random number based on the window size
					Device_Choices[random_var] = Device_Choices[random_var] + 1; //incriments the slot based upon the random variable selected
				}

				//loop to check if any devices successfully sent
				for (int i = 0; i < windows_size; i++) {

					//checks if only 1 device is in a given slot
					if (Device_Choices[i] == 1) {

						devices--; //decreases the number of devices to still send

						//if no devices are left the latency of the run is the slot of the last device to send plus all other windows
						if (devices == 0) {

							latency += i; //increases the latency
						}
					}
				}

				//if devices are still left to send increase the latency by the current windo size
				if (devices > 0) {

					latency += windows_size; //increases the latency
				}

				windows_size *= 2; //incriments the window size according to the specifications of binary backoff
			}

			avg_latency += latency; //adds the current trial latency to the average latency variable
		}

		binaryLatency << avg_latency / 10 << "\n"; //adds the average latency of 10 trials per number of devices to a file
	}

	binaryLatency.close(); //closes the text file



	//creates and opens logLatency file to write averages to
	ofstream logLatency;
	logLatency.open("logLatency.txt");

	//loop for number of devices starting at 100 and incriimenting by 100 to 6000
	for (int number_of_devices = 100; number_of_devices <= 6000; number_of_devices += 100) {

		int avg_latency = 0; //initializes the the average latency variable

		//loop to test the backoff 10 times
		for (int trial = 0; trial < 10; trial++) {

			int devices = number_of_devices; //number of devices per trial that will becrimented
			int windows_size = 2; //initializes the window size
			int latency = 0; //initializes the latency variable for each trial

			//loop for iterating through the backoff until all devices have succeeded
			while (devices > 0) {

				int Device_Choices[windows_size]; //creates an array to store number of devices selecting a slot
				memset(Device_Choices, 0, sizeof(Device_Choices)); //initializes all variables in the array to 0

				//generates a random number for each device that has not successfully sent and incriments that array index
				for (int i = 0; i < devices; i++) {

					int random_var = rand() % windows_size; //generates a random number based on the window size
					Device_Choices[random_var] = Device_Choices[random_var] + 1; //incriments the slot based upon the random variable selected
				}

				//loop to check if any devices successfully sent
				for (int i = 0; i < windows_size; i++) {

					//checks if only 1 device is in a given slot
					if (Device_Choices[i] == 1) {

						devices--; //decreases the number of devices to still send

						//if no devices are left the latency of the run is the slot of the last device to send plus all other windows
						if (devices == 0) {

							latency += i; //increases the latency
						}
					}
				}

				//if devices are still left to send increase the latency by the current windo size
				if (devices > 0) {

					latency += windows_size; //increases the latency
				}

				windows_size = (1 + (1 / log2(windows_size))) * windows_size; //incriments the window size according to the specifications of logarithmic backoff
			}

			avg_latency += latency; //adds the current trial latency to the average latency variable
		}

		logLatency << avg_latency / 10 << "\n"; //adds the average latency of 10 trials per number of devices to a file
	}

	logLatency.close(); //closes the text file


	return 0;

}