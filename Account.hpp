/**
 * @file Account.hpp
 * @author Amdadullah Md (mamdadullah18@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-07-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef D__DOWNLOAD_PROJECTS_STARLING_CPP_ACCOUNT_HPP_
#define D__DOWNLOAD_PROJECTS_STARLING_CPP_ACCOUNT_HPP_

#include <memory>
#include <string>

#include "Definitions.hpp"

class Customer;

class Account {
 public:
  /**
   * @brief Construct a new Account object.
   * 
   * @param pCust Pointer to "Parent" Customer class.
   * @param sUID Store this account's UUID.
   * @param sCat Store this account's Category n.
   */
  Account(Customer* pCust, std::string sUID, std::string sCat);
  virtual ~Account();

  /* Public Methods */
  /**
   * @brief Parses a .json file into a json object.
   * 
   * @param sPath File path to json file.
   * @return json Returns the json object with the data.
   */
  json Parser(std::string sPath);

 private:
  /* Private Methods */
  /**
   * @brief Controller function which calls up a state machine
   * to decide which information to obtain and request.
   * 
   * @param zInfo Variable to indicate which Request to make.
   */
  void Controller(AccInfo zInfo = AccBalance);
  /**
   * @brief A function to generate random
   * UUID values, required to track transactions.
   * 
   * @return std::string Returns the UUID as a string.
   */
  std::string GenerateUUID();
  /**
   * @brief This function implements a state machine
   * with different URLs as options, these are chosen
   * by the Controller().
   * 
   * @param zInfo Enum type specifying which stage of 
   * the request process to set.
   * @return std::string Returns the desired URL based on the 
   * zInfo value.
   */
  std::string SetURL(AccInfo zInfo);
  /**
   * @brief Sets the Parameters required for the Account info
   * This function implements a State machine to choose the various 
   * requests to make. Parses all the data and indicates which 
   * request to make next.
   * 
   * @param zInfo Variable indicating the state of the request process.
   * @param jData JSON object containing the info from the request
   */
  void SetParams(AccInfo* zInfo, json jData);
  /**
   * @brief Get the the date via user input
   * 
   * @param sTemp MAX or MIN timestamp?
   * @return std::string return date as a string
   */
  std::string GetDate(std::string sTemp);

 private:
  /* Defined to prevent copying */
  Account(const Account& other);
  Account& operator=(const Account& other);

  /* Attributes */
  Customer* m_pCustomer;       // Pointer to Customer class
  std::string m_sAccUUID;      // Stores the Account UUID.
  std::string m_sCategory;     // Stores the Account category value.
  std::string m_sSavingsUUID;  // Stores the UUID of a Savings Goal.
  float m_fBalance;            // Stores the Account balance.
  float m_fSum;                // Stores the total sum calculated from the Round-Up.
};

#endif  // D__DOWNLOAD_PROJECTS_STARLING_CPP_ACCOUNT_HPP_
