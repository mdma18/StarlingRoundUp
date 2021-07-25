/**
 * @file Customer.hpp
 * @author Amdadullah Md (mamdadullah18@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-07-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef D__DOWNLOAD_PROJECTS_STARLING_CPP_CUSTOMER_HPP_
#define D__DOWNLOAD_PROJECTS_STARLING_CPP_CUSTOMER_HPP_

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "Account.hpp"
#include "Definitions.hpp"

class Customer {
 public:
  explicit Customer(std::string sAuth);
  virtual ~Customer();

  /* Public Methods */
  json CurlRequest(std::string sURL,
                   ReqType zType,
                   json jData = NULL);

 private:
  /* Private Methods */
  void Configure();
  void Initialise();
  static std::size_t WriteCallback(
      const char* in,
      std::size_t size,
      std::size_t num,
      std::string* out);

 private:
  /* Defined to prevent copying */
  Customer(const Customer& other);
  Customer& operator=(const Customer& other);

  /* Attributes */
  CURL* mCURL;
  struct curl_slist* mHeaders;
  std::string mData;
  std::string mAuth;
  std::vector<Account*> mAccounts;
};

#endif  // D__DOWNLOAD_PROJECTS_STARLING_CPP_CUSTOMER_HPP_
