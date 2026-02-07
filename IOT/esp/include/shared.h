#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT11

typedef struct struct_message
{
    float temperature;
    float humidity;
} struct_message;
