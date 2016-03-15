# Message model

The topics and messages used in this project are described here.

## Broker

Details for the used Broker.

- url: `test.mosquitto.org`
- Ports

  - `1883` : MQTT, unencrypted
  - `8883` : MQTT, encrypted
  - `8884` : MQTT, encrypted, client certificate required
  - `8080` : MQTT over WebSockets, unencrypted
  - `8081` : MQTT over WebSockets, encrypted

## Root topic

`ESEiot/DNS/`

## topic list

This is a global topic listing. All the headers are a topic that will follow after the [Root topic](#root-topic)

### request

The request topis is a topic where all the request to devices are handled.

- `request`: Request topic to request data e.g. `request/online`.
- `request/online`: request a message form all the online users. The message payload is the topic location to respond to?
- `request/distance/object_id`: request the distance from all devices from the object.
- [something for devices relative to object]

### client

The client topic is for sending client specific data.

For speakers the topic: `client/speakers/clientname` is used. The payload in the form of a JSON string (more information possible):

```
{
  "clientname": {
    "object_id": {
      "length": 0,
      "angle": 0
    },
    "object_id2": {
      "length": 0,
      "angle": 0
    }
  }
}
```

### Status

A general status topic.

- `status/time/position`: The position of the song in the time? If send, the speaker will adapt. if not time when it needs to play, value will be 0.
- `status/music`: the state of the playing music. [`play`/`p`] / [`s`/`stop`] in `sec` format or `mm:ss`

### Information

A general Information topic.

- `Information/music/stream`: contains the stream to the music file, if applicable.

# General notes

Some general notes/facts.

- An MQTT client can create topic strings of up to 65535 bytes.
- Payload size was 280MB?
