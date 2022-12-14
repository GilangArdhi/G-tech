#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

DynamicJsonDocument scheduleJson(2048);

String namaOnSchedule, expiredOnSchedule, messageOnSchedule, scheduleCode;

String removeChars(String data, char deleted){
  int idx = data.indexOf(deleted);
  while(idx != -1){
    data.remove(idx, 1);
    idx = data.indexOf(deleted);
  }

  return data;
}

void processScheduleData(String data){
  // scheduleJson.clear();

  int strLen = data.length() + 1;
  char charArr[strLen];
  data.toCharArray(charArr, strLen);

  deserializeJson(scheduleJson, charArr);

  const char* code = scheduleJson["code"];
//  JsonObject scheduleCode = scheduleJson["code"];
  JsonObject scheduleMessage = scheduleJson["message"];
  JsonObject scheduleData = scheduleJson["data"];

  JsonObject namaSchedule = scheduleData["nama_lengkap"];
  JsonObject expiredSchedule = scheduleData["exp_date"];

  namaOnSchedule = namaSchedule["data"].as<String>();
//  pumpOffSchedule = pumpSchedule["off"].as<String>();

  expiredOnSchedule = expiredSchedule["exp_date"].as<String>();
//  fertilizerOffSchedule = fertilizerSchedule["off"].as<String>();

  messageOnSchedule = scheduleMessage["message"].as<String>();


  scheduleCode = String(code);
}
