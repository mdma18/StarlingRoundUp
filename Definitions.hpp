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
constexpr auto OK = 200;
constexpr auto N_ACCOUNTS = 1;
constexpr auto MAX_TR_TIME = "2021-07-19T13%3A15%3A07.000Z";
constexpr auto MIN_TR_TIME = "2021-07-12T13%3A15%3A07.000Z";

// const CURLoption zOpts[] = {
//     CURLOPT_VERBOSE,
//     CURLOPT_SSL_VERIFYPEER,
//     CURLOPT_IPRESOLVE,
//     CURLOPT_TIMEOUT,
//     CURLOPT_FOLLOWLOCATION,
//     CURLOPT_USERAGENT,
//     CURLOPT_WRITEFUNCTION,
//     CURLOPT_WRITEDATA,
//     CURLOPT_HTTPHEADER};

typedef enum {
  AccBalance = 0,
  AccFeed = 1,
  SavingsUUID = 2,
  Transfer = 3,
  Esc = 4
} AccInfo;

#endif  // D__DOWNLOAD_PROJECTS_STARLING_CPP_DEFINITIONS_HPP_
