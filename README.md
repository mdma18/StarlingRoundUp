# Starling's Round-Up Application
This repo contains the source code for the project and some config files required to run the application.

## Structure 
- The Config folder contains the necessary JSON templates and a file required for Authentication.
- Please copy your "*Starling Access Token*" and paste it in Config/Auth.txt
- `Customer.cpp` is the main class containing core functionalities of the project.  
- `main()` Main method is included within the Customer class. 

## Setup
- Clone the repo in your local machine using: `$git clone https://github.com/MDMA99/SmartPricing.git`
- The project utilises GNU's **make** features, hence the project can be executed using the provided *Makefile*

## Makefile
- Available commands: help, all, run and clean.
- To automate everything, simply run: `$make`, this will compile and and run the project.
- Further info can be found by running: `$make help`

**Project Author**: *Amdadullah Md*