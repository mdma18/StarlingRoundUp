/* Copyright (c) 2021 */

#include "Customer.hpp"

//=========================================================================================
Customer::Customer(std::string sAuth)
    : mCURL(curl_easy_init()),
      mHeaders(NULL),
      mAuth(sAuth) {
}
//=========================================================================================

//=========================================================================================
Customer::~Customer() {
  // Clear headers
  curl_slist_free_all(mHeaders);
  // Cleanup CURL
  curl_easy_cleanup(mCURL);
}
//=========================================================================================

//=========================================================================================
void Customer::Initialise() {
  std::string sURL(BASEURL + std::string("accounts"));

  // Get Accounts of Customer
  jResponse = GetRequest(sURL);

  for (int i(0); i < N_ACCOUNTS; i++) {
    mAccounts.push_back(new Account(this,
                                    jResponse["accounts"][i]["accountUid"],
                                    jResponse["accounts"][i]["defaultCategory"]));
  }
}
//=========================================================================================

//=========================================================================================
/* This function take */
//=========================================================================================
json Customer::GetRequest(std::string sURL) {
  int nResCode(0);
  json jData;

  // Retrieve accounts
  curl_easy_setopt(mCURL, CURLOPT_URL, sURL.c_str());
  // Run our HTTP GET command, capture the HTTP response code.
  curl_easy_perform(mCURL);
  curl_easy_getinfo(mCURL, CURLINFO_RESPONSE_CODE, &nResCode);

  // For printing RAW response data
  printf("\n -----GET REQUEST----- \n\n%s\n\n", mData.c_str());

  if (nResCode == OK) {
    // Parse JSON Object.
    jData = json::parse(mData);
    // Clear buffer
    mData.clear();
  } else {
    std::cout << "\nGET request failed with response code: " << nResCode << std::endl;
  }
  return jData;
}
//=========================================================================================

//=========================================================================================
/* This function take */
//=========================================================================================
json Customer::PutRequest(std::string sURL, json jData) {
  int nResCode(0);
  json jRes;

  // Retrieve accounts
  curl_easy_setopt(mCURL, CURLOPT_URL, sURL.c_str());

  // Run our HTTP PUT command, capture the HTTP response code.
  // Enable PUT over HTTP
  // curl_easy_setopt(mCURL, CURLOPT_UPLOAD, 1L);
  curl_easy_setopt(mCURL, CURLOPT_CUSTOMREQUEST, "PUT");             /* !!! */
  curl_easy_setopt(mCURL, CURLOPT_POSTFIELDS, jData.dump().c_str()); /* data goes here */

  curl_easy_perform(mCURL);
  curl_easy_getinfo(mCURL, CURLINFO_RESPONSE_CODE, &nResCode);
  curl_easy_setopt(mCURL, CURLOPT_CUSTOMREQUEST, NULL);
  // For printing RAW response data
  printf("\n -----PUT REQUEST----- \n\n%s\n\n", mData.c_str());

  if (nResCode == OK) {
    // Parse JSON Object.
    jRes = json::parse(mData);
    // Clear buffer
    mData.clear();
  } else {
    std::cout << "\nPUT request failed with response code: " << nResCode << std::endl;
  }
  return jRes;
}
//=========================================================================================

//=========================================================================================
/* Configuration function to set up parameters for cURL */
//=========================================================================================
void Customer::Configure() {
  // Append Header info for Authentication: Accept and Authorization
  mHeaders = curl_slist_append(mHeaders, TYPE);
  mHeaders = curl_slist_append(mHeaders, mAuth.c_str());
  mHeaders = curl_slist_append(mHeaders, C_TYPE);

  // Verbose to debug
  curl_easy_setopt(mCURL, CURLOPT_VERBOSE, 0);

  /* Set the default value: strict certificate check please */
  curl_easy_setopt(mCURL, CURLOPT_SSL_VERIFYPEER, 0);
  // Validate SSL connection
  // curl_easy_setopt(curl, CURLOPT_CAINFO, "starling-sandbox-api-certificate.crt");

  // Don't bother trying IPv6, which would increase DNS resolution time.
  curl_easy_setopt(mCURL, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

  // Don't wait forever, time out after 10 seconds.
  curl_easy_setopt(mCURL, CURLOPT_TIMEOUT, 10);

  // Follow HTTP redirects if necessary.
  curl_easy_setopt(mCURL, CURLOPT_FOLLOWLOCATION, 1L);

  // API protection, providing a user-agent for identification.
  curl_easy_setopt(mCURL, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  // Hook up data handling function.
  curl_easy_setopt(mCURL, CURLOPT_WRITEFUNCTION, Customer::WriteCallback);

  // Hook up data container (will be passed as the last parameter to the
  // callback handling function).
  curl_easy_setopt(mCURL, CURLOPT_WRITEDATA, &mData);

  // Set Request body headers.
  curl_easy_setopt(mCURL, CURLOPT_HTTPHEADER, mHeaders);
}
//=========================================================================================

//=========================================================================================
/* Callback function used by the cURL library to process responses */
//=========================================================================================
std::size_t Customer::WriteCallback(
    const char *in,
    std::size_t size,
    std::size_t num,
    std::string *out) {
  const std::size_t totalBytes(size * num);
  out->append(in, totalBytes);
  return totalBytes;
}
//=========================================================================================

int main() {
  std::string sTemp;
  std::ifstream file("Config/Auth.txt");

  if (!file) {
    std::cout << "Unable to open file";
  } else {
    std::getline(file, sTemp);
    Customer johnDoe("Authorization: Bearer " + sTemp);
    johnDoe.Configure();
    johnDoe.Initialise();
  }
  file.close();

  return 0;
}
