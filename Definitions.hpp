/**
 * @file Definitions.hpp
 * @author Amdadullah Md (mamdadullah18@gmail.com)
 * @brief Header file to implement global definitions
 * accessible by any class.
 * @version 0.1
 * @date 2021-07-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef D__DOWNLOAD_PROJECTS_STARLING_CPP_DEFINITIONS_HPP_
#define D__DOWNLOAD_PROJECTS_STARLING_CPP_DEFINITIONS_HPP_

#pragma comment(lib, "rpcrt4.lib")

/* Standard Libraries */
#include <stdio.h>
#include <stdlib.h>

/* cURL Lib */
#include <curl/curl.h>
#include <math.h>
#include <rpc.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "json.hpp"
using nlohmann::json;

// curl --get https://api-sandbox.starlingbank.com/api/v2/accounts -H "Accept: application/json" -H
// "Authorization: Bearer {yourAccessToken}"

/* Constant definitions */
constexpr auto BASEURL = "https://api-sandbox.starlingbank.com/api/v2/";
constexpr auto TYPE = "Accept: application/json";
constexpr auto C_TYPE = "Content-Type: application/json";
constexpr auto MAX_TR_TIME = "2021-07-25T21%3A51%3A00.000Z";
constexpr auto MIN_TR_TIME = "2021-07-18T13%3A05%3A00.000Z";

struct WriteThis {
  const char* pData;
  size_t nSize;
};

typedef enum {
  AccBalance = 0,
  AccFeed = 1,
  SavingsUUID = 2,
  Transfer = 3,
  Esc = 4
} AccInfo;

typedef enum {
  GET = 0,
  PUT = 1
} ReqType;

#endif  // D__DOWNLOAD_PROJECTS_STARLING_CPP_DEFINITIONS_HPP_
