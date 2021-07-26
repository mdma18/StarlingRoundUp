/* Copyright (c) 2021 */

#include "Account.hpp"

#include "Customer.hpp"

//=========================================================================================
Account::Account(Customer* pCust, std::string sUID, std::string sCat)
    : m_sAccUUID(sUID),
      m_sCategory(sCat),
      m_pCustomer(pCust),
      m_fBalance(0),
      m_fSum(0) {
  Controller();
}
//=========================================================================================

//=========================================================================================
Account::~Account() {
  delete m_pCustomer;
}
//=========================================================================================

//=========================================================================================
void Account::Controller() {
  // Try to get balance info of the acc.
  AccInfo zInfo(AccBalance);
  std::string sTemp;

  while (zInfo != Esc) {
    // Get URL for HTTP Request.
    sTemp = SetURL(zInfo);
    // Perform Request and Set parameters based on data.
    SetParams(&zInfo, m_pCustomer->CurlRequest(sTemp, GET));
    // break;
  }
}
//=========================================================================================

//=========================================================================================
void Account::SetParams(AccInfo* zInfo, json jData) {
  float nVal;
  std::ifstream iFile;
  json jTemp;

  switch (*zInfo) {
    /* Set Balance info and proceed to retrieve transaction info. */
    case AccBalance:
      m_fBalance = jData["effectiveBalance"]["minorUnits"];
      m_fBalance /= 100;
      *zInfo = AccFeed;
      break;
    case AccFeed:
      // Check if there are any transactions.
      if (jData["feedItems"].empty()) {
        // If no transactions found, escape State Machine.
        *zInfo = Esc;
      } else {
        // Initiate RoundUp
        for (int i(0); i < jData["feedItems"].size(); i++) {
          nVal = static_cast<float>(jData["feedItems"][i]["amount"]["minorUnits"]) /
                 static_cast<float>(100);
          m_fSum += (std::ceil(nVal) - nVal);
          // std::cout << fValue << std::endl;
        }
        // Enough funds?
        *zInfo = (m_fSum > 0 && m_fBalance > m_fSum) ? SavingsUUID : Esc;
      }
      break;
    case SavingsUUID:
      // Retrieve Savings Goal list and check if it exists.
      if (jData["savingsGoalList"].empty()) {
        // Create a Savings goal
        iFile.open("Config/createSavings.json");
        jTemp = json::parse(iFile);
        iFile.close();
        jTemp["name"] = "MySavings";
        jTemp["target"]["minorUnits"] = 100000;
        // PUT & leave zInfo as it is
        m_pCustomer->SetBuffer(jTemp);
        m_pCustomer->CurlRequest(SetURL(*zInfo), PUT);
        // Retry again
        *zInfo = SavingsUUID;
        break;
      } else {
        // Grab UID
        m_sSavingsUUID = jData["savingsGoalList"][0]["savingsGoalUid"];
        // std::cout << m_sSavingsUUID << std::endl;
        // std::cout << GenerateUUID() << std::endl;
        *zInfo = Transfer;
      }
    case Transfer:
      iFile.open("Config/addToSavings.json");
      jTemp = json::parse(iFile);
      iFile.close();
      jTemp["amount"]["minorUnits"] = static_cast<int>(std::roundf(m_fSum * 100.0));
      // std::cout << jData << std::endl;
      m_pCustomer->SetBuffer(jTemp);
      jData = m_pCustomer->CurlRequest(SetURL(*zInfo), PUT);
      *zInfo = Esc;
      break;
  }
}
//=========================================================================================

//=========================================================================================
std::string Account::SetURL(AccInfo zInfo) {
  std::string sURL;

  switch (zInfo) {
    case AccBalance:
      sURL = BASEURL + std::string("accounts/") + m_sAccUUID + "/balance";
      break;
    case AccFeed:
      sURL = BASEURL + std::string("feed/account/") + m_sAccUUID + "/category/" +
             m_sCategory + "/transactions-between?" + "minTransactionTimestamp=" +
             MIN_TR_TIME + "&" + "maxTransactionTimestamp=" +
             MAX_TR_TIME;
      break;
    case SavingsUUID:
      sURL = BASEURL + std::string("account/") + m_sAccUUID + "/savings-goals";
      break;
    case Transfer:
      sURL = BASEURL + std::string("account/") + m_sAccUUID + "/savings-goals/" +
             m_sSavingsUUID + "/add-money/" + GenerateUUID();
      break;
  }
  return sURL;
}
//=========================================================================================

//=========================================================================================
std::string Account::GenerateUUID() {
  std::string sUUID;
  char* czStr;
  UUID zUUID;
  ::ZeroMemory(&zUUID, sizeof(UUID));

  // Create uuid or load from a string by UuidFromString() function
  ::UuidCreate(&zUUID);

  // If you want to convert uuid to string, use UuidToString() function
  UuidToStringA(&zUUID, reinterpret_cast<RPC_CSTR*>(&czStr));
  sUUID = czStr;
  RpcStringFreeA(reinterpret_cast<RPC_CSTR*>(&czStr));

  return sUUID;
}
//=========================================================================================
