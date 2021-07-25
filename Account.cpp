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
  // ReqBalance();
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
    SetParams(&zInfo, m_pCustomer->GetRequest(sTemp));
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
          // Enough funds?
          *zInfo = (m_fSum > 0 && m_fBalance > m_fSum) ? SavingsUUID : Esc;
        }
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
        jTemp["target"]["minorUnits"] = 10000;
        // PUT & leave zInfo as it is
        m_pCustomer->PutRequest(SetURL(*zInfo), jTemp);
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
      jData = m_pCustomer->PutRequest(SetURL(*zInfo), jTemp);
      *zInfo = Esc;
      break;
  }
}
//=========================================================================================

//=========================================================================================
// void Account::ReqBalance() {
//   m_sURL = BASEURL + std::string("accounts/") + m_sAccUUID + "/balance";

//   m_jResponse = m_pCustomer->GetRequest(m_sURL);
//   m_fBalance = m_jResponse["effectiveBalance"]["minorUnits"];
//   m_fBalance /= 100;
//   m_jResponse.clear();
//   m_sURL.clear();
//   // std::cout.precision(7);
//   // std::cout << m_fBalance << std::fixed << std::endl;
//   ReqFeedItems();
// }
//=========================================================================================

//=========================================================================================
// void Account::ReqFeedItems() {
//   m_sURL = BASEURL + std::string("feed/account/") + m_sAccUUID + "/category/" +
//            m_sCategory + "/transactions-between?" + "minTransactionTimestamp=" +
//            MIN_TR_TIME + "&" + "maxTransactionTimestamp=" +
//            MAX_TR_TIME;

//   // printf("\n%s\n", m_sURL.c_str());
//   m_jResponse = m_pCustomer->GetRequest(m_sURL);
//   m_sURL.clear();
//   RoundUp();
//   // std::cout << m_jResponse << std::endl;
// }
//=========================================================================================

//=========================================================================================
// void Account::RoundUp() {
//   float fValue;
//   for (int i(0); i < m_jResponse["feedItems"].size(); i++) {
//     fValue = static_cast<float>(m_jResponse["feedItems"][i]["amount"]["minorUnits"]) /
//              static_cast<float>(100);
//     m_fSum += (std::ceil(fValue) - fValue);
//     // std::cout << fValue << std::endl;
//   }
//   // Clear buffer
//   m_jResponse.clear();

//   // Proceed to SavingsGoal
//   if (m_fSum > 0 && m_fBalance > m_fSum) {
//     m_sURL = BASEURL + std::string("account/") + m_sAccUUID + "/savings-goals";
//     // First check if a Savings Goal exists:
//     m_jResponse = m_pCustomer->GetRequest(m_sURL);
//     CheckSavings();
//   }
// }
//=========================================================================================

//=========================================================================================
// void Account::CheckSavings() {
//   // Check if empty
//   std::ifstream iFile("Config/createSavings.json");
//   json jData;
//   iFile >> jData;
//   iFile.close();

//   if (m_jResponse["savingsGoalList"].empty()) {
//     // Creating a Savings goal
//     m_jResponse = m_pCustomer->PutRequest(m_sURL, jData);
//   } else {
//     m_sURL.clear();
//     m_sSavingsUUID = m_jResponse["savingsGoalList"][0]["savingsGoalUid"];
//     // std::cout << m_sSavingsUUID << std::endl;
//     m_jResponse.clear();
//     // std::cout << GenerateUUID() << std::endl;
//     AddToSavings();
//   }
// }
//=========================================================================================

//=========================================================================================
// void Account::AddToSavings() {
//   m_sURL = BASEURL + std::string("account/") + m_sAccUUID + "/savings-goals/" +
//            m_sSavingsUUID + "/add-money/" + GenerateUUID();
//   std::ifstream iFile("Config/addToSavings.json");
//   json jData;
//   iFile >> jData;
//   iFile.close();

//   jData["amount"]["minorUnits"] = static_cast<int>(std::roundf(m_fSum * 100.0));
//   // std::cout << jData << std::endl;
//   m_jResponse = m_pCustomer->PutRequest(m_sURL, jData);
//   // std::cout << m_jResponse << std::endl;
// }
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
