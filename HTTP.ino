EthernetClient client;
const char server[] = "abfahrten.hvv.de";

unsigned long lastRequest = 0;
char trail[15];
int trailIndex = 0;

int idxNeg(int idx) {
  int res = trailIndex - idx;
  if (res >= 0) {
    return res;
  } else {
    return res + 15;
  }
}

void updateHttp() {
  while (client.available()) {
    char c = client.read();
    trail[trailIndex] = c;
    
    if (trail[idxNeg(3)] == 't' && trail[idxNeg(2)] == 'i' && trail[idxNeg(1)] == 'm' && trail[idxNeg(0)] == 'e') {
      client.read();
      client.read();
      client.read();
      
      for (int i = 0; i < 5; i++) {
        busTime[i] = client.read();
      }
      busTime[5] = '\0';
      fetchedBusTime += 1;
    }

    if (trail[idxNeg(4)] == 'd' && trail[idxNeg(3)] == 'e' && trail[idxNeg(2)] == 'l' && trail[idxNeg(1)] == 'a' && trail[idxNeg(0)] == 'y') {
      client.read();
      client.read();
      client.read();
      int i = 0;
      bool doneDelay = 0;
      while (i < 6) {
        if (doneDelay) {
          busDelay[i] = ' ';
        } else {
          busDelay[i] = client.read();
          if (busDelay[i] == ')') {
            doneDelay = 1;
          }
        }
        
        i++;
      }
      busDelay[6] = '\0';
      fetchedBusTime += 1;
    }
    
    if (fetchedBusTime == 2) {
      trailIndex = 0;
      client.stop();
      return;
    }
    trailIndex = (trailIndex + 1) % 15;
  }

  if (millis() - lastRequest > 30000 && fetchNext) {
    client.stop();
    fetchedBusTime = 0;
    if (client.connect(server, 80)) {
      client.println("GET /api/monitors/b2d2d31c-36b2-4a86-b014-be7caad2608f HTTP/1.1");
      client.println("Host: abfahrten.hvv.de");
      client.println("User-Agent: arduino-ethernet");
      client.println("Accept: */*");
      client.println("Content-Type: application/vnd.api+json");
      client.println("Connection: close");
      client.println();
    } else {
      setText("404", 5);
    }
    
    lastRequest = millis();
  }
}

