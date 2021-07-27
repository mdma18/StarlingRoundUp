/* Copyright (c) 2021 */

#include "Customer.hpp"

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

Customer::~Customer() {
  // Clear headers
  curl_slist_free_all(mHeaders);
  // Cleanup CURL
  curl_easy_cleanup(m_Handle);
}
//=========================================================================================

void Customer::Initialise() {
  // Initial (seed) URL to fetch accounts.
  std::string sURL(BASEURL + std::string("accounts"));
  // Get Accounts of Customer
  json jData(CurlRequest(sURL, GET));
  // Initialise Account objects with their respective UUIDs.
  m_Account = std::unique_ptr<Account>(new Account(
      this,
      jData["accounts"][0]["accountUid"],
      jData["accounts"][0]["defaultCategory"]));
}
//=========================================================================================

json Customer::CurlRequest(std::string sURL, ReqType zType) {
  int nResCode(0);
  std::string sTemp(zType ? "PUT" : "GET");
  json jData;
  std::stringstream ssErr;

  // PUT (1) or GET (0) Request.
  curl_easy_setopt(m_Handle, CURLOPT_UPLOAD, zType);
  // Set the URL for Curl.
  curl_easy_setopt(m_Handle, CURLOPT_URL, sURL.c_str());
  // Perform Curl operation.
  curl_easy_perform(m_Handle);
  // Get response info code.
  curl_easy_getinfo(m_Handle, CURLINFO_RESPONSE_CODE, &nResCode);

  // Uncomment to print RAW Curl response data
  std::cout << "\n--- This is a " << sTemp << " request ---\n"
            << std::endl;
  printf("%s\n\n", m_ReadBuff.c_str());

  // Parse data from buffer into JSON object
  jData = json::parse(m_ReadBuff);

  // If Response OK, Parse data & clear buffer.
  if (nResCode == 200) {
    m_ReadBuff.clear();
  } else {
    ssErr << sTemp << " Request failed with response code: " << nResCode << "\n";
    if (!jData.empty()) {
      ssErr << jData["error_description"];
    }
    throw ssErr.str();
  }
  return jData;
}
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

  // Hook up data handling function. For GET (Write from Curl)
  curl_easy_setopt(m_Handle, CURLOPT_WRITEFUNCTION, Customer::WriteCallback);

  // Data buffer for GET (Passed to WRITEFUNCTION).
  curl_easy_setopt(m_Handle, CURLOPT_WRITEDATA, &m_ReadBuff);

  // Set Callback function for HTTP POST/PUT (Pass to Curl)
  curl_easy_setopt(m_Handle, CURLOPT_READFUNCTION, Customer::ReadCallback);

  // Data buffer for PUT request.
  curl_easy_setopt(m_Handle, CURLOPT_READDATA, &m_WriteBuff);

  // Set Request body headers.
  curl_easy_setopt(m_Handle, CURLOPT_HTTPHEADER, mHeaders);
}
//=========================================================================================

std::size_t Customer::WriteCallback(const char *in, std::size_t size, std::size_t num,
                                    std::string *psData) {
  const std::size_t totalBytes(size * num);
  psData->append(in, totalBytes);
  return totalBytes;
}
//=========================================================================================

std::size_t Customer::ReadCallback(char *dest, size_t size, size_t nmemb, void *userp) {
  WriteThis *wt = (struct WriteThis *)userp;
  size_t nBuff = size * nmemb;

  if (wt->nSize) {
    // Copy as much as possible from the source to the destination.
    size_t nCopy = wt->nSize;
    if (nCopy > nBuff)
      nCopy = nBuff;
    memcpy(dest, wt->pData, nCopy);

    wt->pData += nCopy;
    wt->nSize -= nCopy;
    // Copied this many bytes.
    return nCopy;
  }
  // No more data left to deliver.
  return 0;
}
//=========================================================================================

void Customer::SetBuffer(json jData) {
  m_WriteBuff.pData = jData.dump().c_str();
  m_WriteBuff.nSize = jData.dump().size();
}
//=========================================================================================

//=========================================================================================
int main() {
  std::string sTemp, sMaxTime, sMinTime;
  // Auth file
  std::ifstream file("Config/Auth.txt");

  if (!file) {
    std::cout << "Unable to open file";
  } else {
    std::getline(file, sTemp);
    try {
      Customer johnDoe("Authorization: Bearer " + sTemp);
    } catch (std::string ssMsg) {
      std::cout << ssMsg << std::endl;
    }
  }
  file.close();

  return 0;
}
