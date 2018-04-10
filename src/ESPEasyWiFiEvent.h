//********************************************************************************
// Functions called on events.
// Make sure not to call anything in these functions that result in delay() or yield()
//********************************************************************************
#ifdef ESP32
void WiFiEvent(system_event_id_t event, system_event_info_t info) {
  switch(event) {
    case SYSTEM_EVENT_STA_CONNECTED:
      lastConnectMoment = millis();
      processedConnect = false;
      wifiStatus = ESPEASY_WIFI_CONNECTED;
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      lastDisconnectMoment = millis();
      if (timeDiff(lastConnectMoment, last_wifi_connect_attempt_moment) > 0) {
        // There was an unsuccessful connection attempt
        lastConnectedDuration = timeDiff(last_wifi_connect_attempt_moment, lastDisconnectMoment);
      } else {
        lastConnectedDuration = timeDiff(lastConnectMoment, lastDisconnectMoment);
      }
      processedDisconnect = false;
      lastDisconnectReason = static_cast<WiFiDisconnectReason>(info.disconnected.reason);
      wifiStatus = ESPEASY_WIFI_DISCONNECTED;
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      lastGetIPmoment = millis();
      wifiStatus = ESPEASY_WIFI_GOT_IP;
      processedGetIP = false;
      break;
    case SYSTEM_EVENT_AP_STACONNECTED:
      for (byte i = 0; i < 6; ++i) {
        lastMacConnectedAPmode[i] = info.sta_connected.mac[i];
      }
      processedConnectAPmode = false;
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
      for (byte i = 0; i < 6; ++i) {
        lastMacConnectedAPmode[i] = info.sta_disconnected.mac[i];
      }
      processedDisconnectAPmode = false;
      break;
    default:
      break;
  }
}
#else

void onConnected(const WiFiEventStationModeConnected& event){
  lastConnectMoment = millis();
  processedConnect = false;
  wifiStatus = ESPEASY_WIFI_CONNECTED;
  last_channel = event.channel;
  last_ssid = event.ssid;
  bssid_changed = false;
  for (byte i=0; i < 6; ++i) {
    if (lastBSSID[i] != event.bssid[i]) {
      bssid_changed = true;
      lastBSSID[i] = event.bssid[i];
    }
  }
}

void onDisconnect(const WiFiEventStationModeDisconnected& event){
  lastDisconnectMoment = millis();
  if (timeDiff(lastConnectMoment, last_wifi_connect_attempt_moment) > 0) {
    // There was an unsuccessful connection attempt
    lastConnectedDuration = timeDiff(last_wifi_connect_attempt_moment, lastDisconnectMoment);
  } else
    lastConnectedDuration = timeDiff(lastConnectMoment, lastDisconnectMoment);
  lastDisconnectReason = event.reason;
  wifiStatus = ESPEASY_WIFI_DISCONNECTED;
  processedDisconnect = false;
}

void onGotIP(const WiFiEventStationModeGotIP& event){
  lastGetIPmoment = millis();
  wifiStatus = ESPEASY_WIFI_GOT_IP;
  processedGetIP = false;
}

void onConnectedAPmode(const WiFiEventSoftAPModeStationConnected& event) {
  for (byte i = 0; i < 6; ++i) {
    lastMacConnectedAPmode[i] = event.mac[i];
  }
  processedConnectAPmode = false;
}

void onDisonnectedAPmode(const WiFiEventSoftAPModeStationDisconnected& event) {
  for (byte i = 0; i < 6; ++i) {
    lastMacDisconnectedAPmode[i] = event.mac[i];
  }
  processedDisconnectAPmode = false;
}

#endif
