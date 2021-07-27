/**
 * @file Customer.hpp
 * @author Amdadullah Md (mamdadullah18@gmail.com)
 * @brief Customer Class, Stores Account object and
 * performs all HTTP transactions, takes care of 
 * handles and interaction with CURL.
 * 
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

#include "Account.hpp"
#include "Definitions.hpp"

class Customer {
 public:
  /**
   * @brief Construct a new Customer object
   * 
   * @param sAuth Stores the Access Token
   */
  explicit Customer(std::string sAuth);
  virtual ~Customer();

  /* Public Methods */
  /**
   * @brief This function performs all the HTTP requests
   * using the built-in curl tool and libcurl library. 
   * 
   * @param sURL The full URL path for the request
   * @param zType Type of request: PUT(1) or GET(0)
   * @return json JSON constructed object (using external lib),
   * this object returns the full JSON response data.
   */
  json CurlRequest(std::string sURL, ReqType zType);
  /**
   * @brief This function converts the JSON object into 
   * a custom WriteThis struct object which is required 
   * to pass onto Curl for PUT requests.
   * 
   * @param jData JSON object containing PUT data to send.
   */
  void SetBuffer(json jData);

 private:
  /* Private Methods */
  /**
   * @brief This function configures all the necessary 
   * Curl settings required to perform PUT and GET transfers.
   * Needs to be called before any HTTP processing.
   * 
   */
  void Configure();
  /**
   * @brief Initialiser function to kick-off the 
   * initial HTTP transaction and retrieve the account 
   * details of the user.
   * 
   */
  void Initialise();
  /**
   * @brief This callback function gets called by libcurl 
   * as soon as there is data received that needs to be saved. 
   * For most transfers, this callback gets called many times 
   * and each invoke delivers another chunk of data. psDataOut points 
   * to the delivered data, and the nSize of that data is nDataSize; 
   * nSize is always 1.
   * 
   * @param pcDataIn Char array with data
   * @param nSize Size of char arr
   * @param nDataSize Size of final data
   * @param psDataOut String object with data, ready to be used.
   * @return std::size_t Returns the number of bytes actually taken care 
   * of. If that amount differs from the amount passed to the callback 
   * function, it'll signal an error condition to the library. This will 
   * cause the transfer to get aborted and the libcurl function used will 
   * return CURLE_WRITE_ERROR.
   */
  static std::size_t WriteCallback(
      const char* pcDataIn,
      std::size_t nSize,
      std::size_t nDataSize,
      std::string* psDataOut);
  /**
   * @brief This callback function gets called by libcurl as soon 
   * as it needs to read data in order to send it to the peer - i.e 
   * asking it to upload or post data to the server. The pData area 
   * pointed at by the pointer buffer should be filled up with at 
   * most size multiplied with nitems number of bytes by the function.
   * 
   * @param cpDest Char arr containing the data needed to PUT.
   * @param nSize Size of data to send
   * @param nDataSize Size of struct data, retrieved from struct.
   * @param pData Struct WriteThis object containing data as string.
   * @return size_t Returns the actual number of bytes that it stored 
   * in the data area pointed at by the pointer buffer. Returning 0 will 
   * signal end-of-file to the library and cause it to stop the current 
   * transfer.
   */
  static size_t ReadCallback(
      char* cpDest,
      size_t nSize,
      size_t nDataSize,
      void* pData);

 private:
  /* Defined to prevent copying */
  Customer(const Customer& other);
  Customer& operator=(const Customer& other);

  /* Attributes */
  CURL* m_Handle;                      // Stores the actual Curl handle.
  struct curl_slist* mHeaders;         // Stores a list of headers to be sent with a request.
  std::string m_ReadBuff;              // Buffer which stores data from a GET operation.
  struct WriteThis m_WriteBuff;        // Buffer which stores the data needed for a PUT/POST operation.
  std::string mAuth;                   // Stores the Access Token of the user.
  std::unique_ptr<Account> m_Account;  // Account object storing all acc info.
};

#endif  // D__DOWNLOAD_PROJECTS_STARLING_CPP_CUSTOMER_HPP_
