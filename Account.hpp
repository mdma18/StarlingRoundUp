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
  Account(Customer* pCust, std::string sUID, std::string sCat);
  virtual ~Account();

  /* Public Methods */

 private:
  /* Private Methods */
  void Controller();
  std::string GenerateUUID();
  std::string SetURL(AccInfo zInfo);
  void SetParams(AccInfo* zInfo, json jData);

 private:
  /* Defined to prevent copying */
  Account(const Account& other);
  Account& operator=(const Account& other);

  /* Attributes */
  std::string m_sAccUUID;
  std::string m_sCategory;
  std::string m_sSavingsUUID;
  float m_fBalance;
  Customer* m_pCustomer;
  float m_fSum;
};

#endif  // D__DOWNLOAD_PROJECTS_STARLING_CPP_ACCOUNT_HPP_
