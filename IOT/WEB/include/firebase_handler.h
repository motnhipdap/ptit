// firebase_handler.h
#ifndef FIREBASE_HANDLER_H
#define FIREBASE_HANDLER_H

#include <FirebaseClient.h>

void setupFirebase();
void uploadBMP180DHT11Data();
void uploadActivityCount(bool isDaily = false);
void uploadWeight(bool isDaily = false);
void updateSystemStatus();
void startFanStream();
void uploadFanData();
void uploadHealth(bool health);
void processData(AsyncResult &aResult);
void processFanData(AsyncResult &aResult);

#endif