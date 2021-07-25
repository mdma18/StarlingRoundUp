/* Copyright (c) 2021 */

#include "Customer.hpp"

//=========================================================================================
Customer::Customer(std::string sAuth)
    : mCURL(curl_easy_init()),
      mHeaders(NULL),
      mAuth(sAuth) {
  Configure();
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
  // Initial (seed) URL to fetch accounts.
  std::string sURL(BASEURL + std::string("accounts"));
  // Get Accounts of Customer
  json jData(CurlRequest(sURL, GET));

  // Initialise Account objects with their respective UUIDs.
  for (int i(0); i < N_ACCOUNTS; i++) {
    mAccounts.push_back(new Account(this,
                                    jData["accounts"][i]["accountUid"],
                                    jData["accounts"][i]["defaultCategory"]));
  }
}
//=========================================================================================

//=========================================================================================
json Customer::CurlRequest(std::string sURL, ReqType zType, json jData) {
  int nResCode(0);
  std::string sTemp(zType ? "PUT" : "GET");

  switch (zType) {
    case PUT:
      curl_easy_setopt(mCURL, CURLOPT_CUSTOMREQUEST, "PUT");             /* !!! */
      curl_easy_setopt(mCURL, CURLOPT_POSTFIELDS, jData.dump().c_str()); /* data goes here */
    case GET:
      curl_easy_setopt(mCURL, CURLOPT_URL, sURL.c_str());
      // Run our HTTP GET command, capture the HTTP response code.
      curl_easy_perform(mCURL);
      curl_easy_getinfo(mCURL, CURLINFO_RESPONSE_CODE, &nResCode);
      break;
  }
  // For printing RAW response data
  std::cout << "\n--- This is a " << sTemp << " request ---\n"
            << std::endl;
  printf("\n\n%s\n\n", mData.c_str());

  if (nResCode == OK) {
    // Parse JSON Object.
    jData = json::parse(mData);
    // Clear buffer
    mData.clear();
  } else {
    std::cout << sTemp << " Request failed with response code: " << nResCode << std::endl;
  }
  return jData;
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
  // Start program
  Initialise();
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
  }
  file.close();

  return 0;
}
