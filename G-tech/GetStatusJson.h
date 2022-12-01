#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

DynamicJsonDocument statusJson(2048);
String statusMessage, statusNama, statusExpired, statusCode;

void processStatusData(String data){
  int strLen = data.length() + 1;
  char charArr[strLen];
  data.toCharArray(charArr, strLen);

  deserializeJson(statusJson, charArr);

  const char* msg = statusJson["message"];
  const char* code = statusJson["code"];
//  JsonObject statuscode = statusJson["code"];
//  JsonObject statusmsg = statusJson["message"];
  JsonObject statusData = statusJson["data"];
  
  statusMessage = String(msg);
  statusCode = String(code);
  statusNama = statusData["nama_lengkap"].as<String>();
  statusExpired = statusData["exp_date"].as<String>();

  statusJson.clear();
}
