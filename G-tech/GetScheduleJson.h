#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

DynamicJsonDocument scheduleJson(2048);

String scheduleMessage, namaOnSchedule, expiredOnSchedule;

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

  const char* msg = scheduleJson["message"];
  JsonObject scheduleCode = scheduleJson["code"];
//  JsonObject scheduleMessage = scheduleJson["massage"];
  JsonObject scheduleData = scheduleJson["data"];

  JsonObject namaSchedule = scheduleData["nama_lengkap"];
  JsonObject expiredSchedule = scheduleData["exp_date"];

  namaOnSchedule = namaSchedule["nama_lengkap"].as<String>();
//  pumpOffSchedule = pumpSchedule["off"].as<String>();

  expiredOnSchedule = expiredSchedule["exp_date"].as<String>();
//  fertilizerOffSchedule = fertilizerSchedule["off"].as<String>();


  scheduleMessage = String(msg);
}
