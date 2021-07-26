/* Copyright (c) 2021 */

#include "Customer.hpp"

//=========================================================================================
Customer::Customer(std::string sAuth)
    : m_Handle(curl_easy_init()),
      mHeaders(NULL),
      mAuth(sAuth) {
  // Set all Curl handle parameters
  Configure();
  // Start program
  Initialise();
}
//=========================================================================================

//=========================================================================================
Customer::~Customer() {
  // Clear headers
  curl_slist_free_all(mHeaders);
  // Cleanup CURL
  curl_easy_cleanup(m_Handle);
}
//=========================================================================================

//=========================================================================================
void Customer::Initialise() {
  // Initial (seed) URL to fetch accounts.
  std::string sURL(BASEURL + std::string("accounts"));
  // Get Accounts of Customer
  json jData(CurlRequest(sURL, GET));

  // Initialise Account objects with their respective UUIDs.
  m_Account = new Account(this, jData["accounts"][0]["accountUid"],
                          jData["accounts"][0]["defaultCategory"]);
}
//=========================================================================================

//=========================================================================================
json Customer::CurlRequest(std::string sURL, ReqType zType, json jData) {
  int nResCode(0);
  std::string sTemp(zType ? "PUT" : "GET");

  curl_easy_setopt(m_Handle, CURLOPT_UPLOAD, zType);

  // If PUT, prepare data
  if (zType) {
    m_WriteBuff.pData = jData.dump().c_str();
    m_WriteBuff.nSize = jData.dump().size();
  }

  curl_easy_setopt(m_Handle, CURLOPT_URL, sURL.c_str());
  // Run our HTTP GET command, capture the HTTP response code.
  curl_easy_perform(m_Handle);
  curl_easy_getinfo(m_Handle, CURLINFO_RESPONSE_CODE, &nResCode);

  // For printing RAW response data
  std::cout << "\n--- This is a " << sTemp << " request ---\n"
            << std::endl;
  printf("%s\n\n", mData.c_str());

  if (nResCode == 200) {
    // Parse JSON Object.
    jData = json::parse(mData);
    // Clear buffer
    mData.clear();
  } else {
    std::cout << sTemp << " Request failed with response code: " << nResCode << std::endl;
    exit(EXIT_FAILURE);
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
  curl_easy_setopt(m_Handle, CURLOPT_VERBOSE, 0L);

  /* Set the default value: strict certificate check please */
  curl_easy_setopt(m_Handle, CURLOPT_SSL_VERIFYPEER, 0);
  // Validate SSL connection
  // curl_easy_setopt(curl, CURLOPT_CAINFO, "starling-sandbox-api-certificate.crt");

  // Don't bother trying IPv6, which would increase DNS resolution time.
  curl_easy_setopt(m_Handle, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

  // Don't wait forever, time out after 10 seconds.
  curl_easy_setopt(m_Handle, CURLOPT_TIMEOUT, 10);

  // Follow HTTP redirects if necessary.
  curl_easy_setopt(m_Handle, CURLOPT_FOLLOWLOCATION, 1L);

  // API protection, providing a user-agent for identification.
  curl_easy_setopt(m_Handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  // Hook up data handling function.
  curl_easy_setopt(m_Handle, CURLOPT_WRITEFUNCTION, Customer::WriteCallback);

  // Hook up data container (will be passed as the last parameter to the
  // callback handling function).
  curl_easy_setopt(m_Handle, CURLOPT_WRITEDATA, &mData);

  curl_easy_setopt(m_Handle, CURLOPT_READFUNCTION, Customer::ReadCallback);

  curl_easy_setopt(m_Handle, CURLOPT_READDATA, &m_WriteBuff);

  // Set Request body headers.
  curl_easy_setopt(m_Handle, CURLOPT_HTTPHEADER, mHeaders);
}
//=========================================================================================

//=========================================================================================
/* Callback function used by the cURL library to process responses */
//=========================================================================================
std::size_t Customer::WriteCallback(const char *in, std::size_t size, std::size_t num,
                                    std::string *out) {
  const std::size_t totalBytes(size * num);
  out->append(in, totalBytes);
  return totalBytes;
}
//=========================================================================================

//=========================================================================================
std::size_t Customer::ReadCallback(char *dest, size_t size, size_t nmemb, void *userp) {
  WriteThis *wt = (struct WriteThis *)userp;
  size_t nBuff = size * nmemb;

  if (wt->nSize) {
    /* copy as much as possible from the source to the destination */
    size_t nCopy = wt->nSize;
    if (nCopy > nBuff)
      nCopy = nBuff;
    memcpy(dest, wt->pData, nCopy);

    wt->pData += nCopy;
    wt->nSize -= nCopy;
    return nCopy; /* we copied this many bytes */
  }
  return 0; /* no more data left to deliver */
}
//=========================================================================================

//=========================================================================================
int main() {
  std::string sTemp;
  // Auth file
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
