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
  json CurlRequest(
      std::string sURL,
      ReqType zType,
      json jData = NULL);

 private:
  /* Private Methods */
  void Configure();
  void Initialise();
  // Used for GET
  static std::size_t WriteCallback(
      const char* in,
      std::size_t size,
      std::size_t num,
      std::string* out);
  // Used for PUT
  static size_t ReadCallback(
      char* dest,
      size_t size,
      size_t nmemb,
      void* userp);

 private:
  /* Defined to prevent copying */
  Customer(const Customer& other);
  Customer& operator=(const Customer& other);

  /* Attributes */
  // std::vector<CURL*> m_vHandles;
  CURL* m_Handle;
  struct curl_slist* mHeaders;
  std::string mData;             // Buff
  struct WriteThis m_WriteBuff;  // Buff to POST
  std::string mAuth;
  Account* m_Account;
};

#endif  // D__DOWNLOAD_PROJECTS_STARLING_CPP_CUSTOMER_HPP_
