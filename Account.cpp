/* Copyright (c) 2021 */

#include "Account.hpp"
// Necessary to reference parent class
#include "Customer.hpp"

//=========================================================================================
Account::Account(Customer* pCust, std::string sUID, std::string sCat)
    : m_sAccUUID(sUID),
      m_sCategory(sCat),
      m_pCustomer(pCust),
      m_fBalance(0),
      m_fSum(0) {
  // Start redirecting
  Controller();
}
//=========================================================================================

//=========================================================================================
Account::~Account() {
  delete m_pCustomer;
}
//=========================================================================================

//=========================================================================================
void Account::Controller(AccInfo zInfo) {
  std::string sTemp;

  // Loop around Curl until all requests have been performed.
  while (zInfo != Esc) {
    // Get the correct URL for the HTTP Request specified by zInfo.
    sTemp = SetURL(zInfo);
    // Perform Request and Set parameters based on the obtained data.
    SetParams(&zInfo, m_pCustomer->CurlRequest(sTemp, GET));
  }
}
//=========================================================================================

//=========================================================================================
void Account::SetParams(AccInfo* zInfo, json jData) {
  float nVal(0);
  json jTemp;
  std::stringstream ssErr;

  switch (*zInfo) {
    /* Set, convert Balance info and proceed to retrieve transaction data (feed items). */
    case AccBalance:
      m_fBalance = jData["effectiveBalance"]["minorUnits"];
      m_fBalance /= 100;
      *zInfo = AccFeed;
      break;
    /* Check for feed items, initiate round-up if found. */
    case AccFeed:
      if (jData["feedItems"].empty()) {
        ssErr << "No transactions found. Nothing to send to Savings Goal.";
        throw ssErr.str();
      } else {
        for (int i(0); i < jData["feedItems"].size(); i++) {
          nVal = jData["feedItems"][i]["amount"]["minorUnits"];
          nVal /= 100;
          // Calculate "round-up" value and add to sum.
          m_fSum += (std::ceil(nVal) - nVal);
          // std::cout << fValue << std::endl;
        }
        // Enough funds? If yes, move round-up sum to savings goal, otherwise terminate.
        if (m_fSum > 0 && m_fBalance > m_fSum) {
          *zInfo = SavingsUUID;
        } else {
          ssErr << "Ending program as not enough funds available or nothing to save up.";
          throw ssErr;
        }
      }
      break;
    /* Check whether a Savings Goal exists, if not, create one, otherwise send Sum. */
    case SavingsUUID:
      if (jData["savingsGoalList"].empty()) {
        jTemp = Parser("Config/createSavings.json");
        jTemp["name"] = "MySavings";
        jTemp["target"]["minorUnits"] = 100000;
        // Prepare buffer for PUT request
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
      break;
    case Transfer:
      jTemp = Parser("Config/addToSavings.json");
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
json Account::Parser(std::string sPath) {
  std::ifstream iFile;
  json jTemp;

  iFile.open(sPath);
  jTemp = json::parse(iFile);
  iFile.close();

  return jTemp;
}
//=========================================================================================

//=========================================================================================
std::string Account::GenerateUUID() {
  std::string sUUID;
  char* czStr;
  UUID zUUID;
  ::ZeroMemory(&zUUID, sizeof(UUID));

  // Create UUID
  ::UuidCreate(&zUUID);

  // Convert to String format
  UuidToStringA(&zUUID, reinterpret_cast<RPC_CSTR*>(&czStr));
  sUUID = czStr;
  RpcStringFreeA(reinterpret_cast<RPC_CSTR*>(&czStr));

  return sUUID;
}
//=========================================================================================
