# Starling's Round-Up Application
This serverless application enables a user to take all the transactions in a given week and round-up them
up to the nearest £1 and then transfer the total sum to a savings goal. For example with spending of £4.35, 
£5.20 and £0.87, the round-up would be £1.58. This amount then gets transferred into a savings goal, helping 
the customer save for future adventures.

## Usage
The application retrieves the accounts for the user based on the given `starling-access-token`, it then 
retrieves the account's information. Once a Round-Up value has been calculated from the transactions feed, 
the program creates a `SavingsGoal` account to transfer the money to; if accounts exist already, it will 
transfer it to the first account found.

## Structure 
- The Config folder contains the necessary JSON templates and a file required for Authentication.
- Please create a `.txt` file in the *Config* folder, named `Auth`. Copy your "*Starling Access Token*" 
- into Config/Auth.txt
- `Customer.cpp` is the main class containing the core functionalities of the project.  
- `main()` Main method is included within the Customer class. 

## Setup
- Clone the repo in your local machine using: `$git clone https://github.com/MDMA99/SmartPricing.git`
- The project utilises GNU's **make** features, hence the project can be executed using the provided *Makefile*

## Makefile
- Available commands: help, all, run and clean.
- To automate everything, simply run: `$make`, this will compile and and run the project.
- Further info can be found by running: `$make help`

**Project Author**: *Amdadullah Md*