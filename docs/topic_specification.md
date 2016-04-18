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

### Request

The request topic is a topic where all the requests to devices are handled.

- `request/online`: Request a message from all the online devices, the message payload is the topic location to respond to. The preferred location for the answer would be `info/client/online`.
- `request/distance/objectid`: Request the distance from all devices relative to the object [objectid]. Payload is topic to publish to. If a location is unknown to a device, they don't respond.
- `request/info/clients`: Request all the devices to send their device specific information to a topic. Payload is topic to publish to. All devices respond with the JSON string that they have.(See [client](#client)) If a device has no information, It wont respond. This command is mostly useful for the website to build up it's online device listing and parameter listing.

### Client

The client topic is for sending client specific data.

`client/speaker/clientid/objects`: This is used to send device specific details concerning the relative distance from a virtual object. I.e. the website will send the data belonging to the device.

The data is in the from of a JSON string. (more information possible):

```
{
  "clientid": {
    "objectid": {
      "distance": 0,
      "angle": 0
    },
    "objectid2": {
      "distance": 0,
      "angle": 0
    }
  }
}
```

### Info

A general information topic.

#### Music status

- `info/music/time`: When a device needs to play a music file, it will send the following information to this topic. The name is the name of the music file (see `info/music/sources`) and the number is the total playtime of this music file in *seconds*.
```
{
  "name": 0
}
```
- `info/music/time/position`: The position of the song in the time. If send, the speaker will adapt their time to this value. If a speaker gets the play command but doesn't know the time, a value of `0` will be used. [in `sec` format or `mm:ss`, not yet decided]
- `info/music/status`: The state of the playing music. [`play`/`p`] / [`s`/`stop`] If a speaker joins, they will wait for a command before they start doing anything.
- `info/music/sources`: Contains the URIs to the music files in JSON format. The client will download these files. The identifier is the `objectid`, because the sound is bound to an object. The name also will be the filename on the local file system.
```
{
    "name": "uri"
}
```
```
{
    "name_is_objectid": "uri_is_uri"
}
```
```
{
    "object_1": "http://www.example.org",
    "object_2": "http://www.example.com",
    ...
    "object_5": "http://www.example.org"
}
```
- `info/music/volume`: The master volume. This value will be of a value between `100` or `0`. [For now just use `100` as the value. This could later be in a request format to the website]
- `info/clients`: global data for sending information of all the clients which are **participating** in the audio network. Some devices can be online and *not* participate in the active network, these devices won't be in this global data list.
```
{
  "clientid_1": {
    "objectid_1": {
      "distance": 0,
      "angle": 0
    },
    "objectid_2": {
      "distance": 0,
      "angle": 0
    }
  },
  "clientid_2": {
    "objectid_1": {
      "distance": 0,
      "angle": 0
    }
  },
  "clientid_3": {
    "objectid_2": {
      "distance": 0,
      "angle": 0
    }
  }
}
```

#### Device status

- `info/client/online`: If a clients comes online, it will post its `clientid`.
- `info/client/offline`: If a client goes offline, it will post its `clientid`.

# General notes

- Client name (aka the `clientid`) - Speaker: The name for a speaker will be in the form of `speak_xxx`, where XXX is a random number. The maximal value of this random number is: `999`. Make sure that the devices can connect with its clientname. If not, that name already could be in use.
- An MQTT client can create topic strings of up to 65535 bytes.
- The maximal MQTT payload size is: `xxx xx` [280MB?]
- Parts between `[]` or the use of `*` after a sentence in this document usually mean that information needs to be added. Or that clarification is needed.
