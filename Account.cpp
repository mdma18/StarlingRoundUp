/* Copyright (c) 2021 */

#include "Account.hpp"
// Necessary to reference parent class
#include "Customer.hpp"

Account::Account(Customer* pCust, std::string sUID, std::string sCat)
    : m_pCustomer(pCust),
      m_sAccUUID(sUID),
      m_sCategory(sCat),
      m_fBalance(0),
      m_fSum(0) {
  // Start redirecting
  // std::cout << "Your account UUID is: " << sUID << std::endl;
  Controller();
  std::cout << "\n\nYour account balance is: " << m_fBalance << std::endl;
  printf("\nThe total sum calculated from the Round-Up is: %.2f\n", m_fSum);
}
//=========================================================================================

Account::~Account() {
  delete m_pCustomer;
}
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

void Account::SetParams(AccInfo* zInfo, json jData) {
  float nVal(0);
  json jTemp;
  std::stringstream ssErr;

  switch (*zInfo) {
    /* Set, convert Balance info and proceed to retrieve transaction data (feed items). */
    case AccBalance:
      m_fBalance = jData["effectiveBalance"]["minorUnits"];
      m_fBalance /= 100;
      *zInfo = m_fSum ? Esc : AccFeed;
      break;
    /* Check for feed items, initiate round-up if found. */
    case AccFeed:
      if (jData["feedItems"].empty()) {
        ssErr << "\nNo transactions found. Nothing to send to Savings Goal.\n";
        throw ssErr.str();
      } else {
        for (int i(0); i < static_cast<int>(jData["feedItems"].size()); i++) {
          nVal = jData["feedItems"][i]["amount"]["minorUnits"];
          nVal /= 100;
          // Calculate "round-up" value and add to sum.
          m_fSum += (std::ceil(nVal) - nVal);
        }
        // Enough funds? If yes, move round-up sum to savings goal, otherwise terminate.
        if (m_fSum > 0 && m_fBalance > m_fSum) {
          *zInfo = SavingsUUID;
        } else {
          ssErr << "\nEnding program as not enough funds available or nothing to save up.";
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
        // std::cout << "\nA Savings Goal has been created with the name \"MySavings\" and "
        //           << "with a target goal of \"£1,000\"\n";
        // Prepare buffer for PUT request
        m_pCustomer->SetBuffer(jTemp);
        m_pCustomer->CurlRequest(SetURL(*zInfo), PUT);
        // Retry again
        *zInfo = SavingsUUID;
        break;
      } else {
        // Grab UID
        m_sSavingsUUID = jData["savingsGoalList"][0]["savingsGoalUid"];
        // std::cout << "\nA Savings Goal with the name: "
        //           << jData["savingsGoalList"][0]["name"]
        //           << " and a target goal of: "
        //           << jData["savingsGoalList"][0]["target"]["minorUnits"]
        //           << " has been found.\n"
        //           << "Currently, "
        //           << jData["savingsGoalList"][0]["totalSaved"]["minorUnits"]
        //           << " have been saved up. (N.B Divide value by 100).\n"
        //           << "Percentage of fulfilment: "
        //           << jData["savingsGoalList"][0]["savedPercentage"]
        //           << std::endl;
        *zInfo = Transfer;
      }
    case Transfer:
      jTemp = Parser("Config/addToSavings.json");
      jTemp["amount"]["minorUnits"] = static_cast<int>(std::roundf(m_fSum * 100.0));
      // std::cout << "Money transferred! Run program again to see changes.\n"
      //           << std::endl;
      m_pCustomer->SetBuffer(jTemp);
      jData = m_pCustomer->CurlRequest(SetURL(*zInfo), PUT);
      *zInfo = AccBalance;
      break;
    case Esc:
      break;
  }
}
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
             GetDate("MIN") + "T17%3A00%3A00.000Z" + "&" + "maxTransactionTimestamp=" +
             GetDate("MAX") + "T17%3A00%3A00.000Z";
      break;
    case SavingsUUID:
      sURL = BASEURL + std::string("account/") + m_sAccUUID + "/savings-goals";
      break;
    case Transfer:
      sURL = BASEURL + std::string("account/") + m_sAccUUID + "/savings-goals/" +
             m_sSavingsUUID + "/add-money/" + GenerateUUID();
      break;
    case Esc:
      break;
  }
  return sURL;
}
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

std::string Account::GetDate(std::string sTemp) {
  std::cout << "Please provide the "
            << sTemp
            << " transaction timestamp in the format (YYYY-MM-DD)"
            << std::endl;
  std::cin >> sTemp;
  return sTemp;
}
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
