var CONFIG =
{
  "name": "Dynamic Network of Speakers",
  "name_short": "DNS",
  "version": {
    "major": 0,
    "minor": 0,
    "revision": 1
  },
  "name_speaker": "speaker_",
  "name_website": "site_",
  "use_broker": 1,
  "broker": {
    1: {
      "broker": "Mosquitto",
      "uri": "test.mosquitto.org",
      "port": 1883,
      "port_encrypted": 8883,
      "port_encrypted_certificate": 8884,
      "port_ws": 8080,
      "port_ws_encrypted": 8081
    },
    2: {
      "broker": "Mosquitto eclipse",
      "uri": "iot.eclipse.org",
      "port": 1883,
      "port_encrypted": 8883,
      "port_ws": 80,
      "port_ws_encrypted": 443
    },
    3: {
      "broker": "HiveMQ",
      "uri": "broker.mqttdashboard.com",
      "port": 1883,
      "port_ws": 8000
    },
    4: {
      "broker": "mosca",
      "uri": "test.mosca.io",
      "port": 1883,
      "port_ws": 80
    }
  }
}
;
