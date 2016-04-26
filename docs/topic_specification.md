# Message model

The topics and messages used in this project are described here.

## Public MQTT Brokers

Details to brokers used in this project.

ID | Server             | Broker    | Port                    | Web socket
-- | ------------------ | --------- | ----------------------- | --------------
01 | test.mosquitto.org | Mosquitto | 1883, 8883`*`, 8884`**` | 8080 / 8081`*`
02 | iot.eclipse.org    | Mosquitto | 1883, 8883`*`           | 80, 443`*`
03 | broker.hivemq.com  | HiveMQ    | 1883                    | 8000
04 | test.mosca.io      | mosca     | 1883                    | 80

`*`: encrypted<br>
`**`: encrypted, client certificate required

## Root topic

`ESEiot/DNS/`

## topic list

This is a global topic listing. All the headers are a topic that will follow after the [Root topic](#root-topic). I.e. `root_topic/header`.

### Request

The request topic is a topic where all the requests to devices are handled.

- `request/online`: Request a message from all the online devices. The devices respond with their `clientid` to `info/client/online`.
- `request/info/clients`: Request all the devices to send their device specific information to a topic. Payload is topic to publish to. All devices respond with the JSON string that they have. (See below) If a device has no information, It wont respond. This command is mostly useful for the website to build up the device information list.

```
{
  "clientid": {
    "objectid1": {
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

- `info/music/time`: When a device needs to play a music file, it will send the following information to this topic. The name is the name of the music file (see `info/music/sources`) and the number is the total playtime of this music file in _seconds_.

```
{
  "name": 0
}
```

- `info/music/time/position`: The position of the music file in the time. If send, the speaker will adapt their time to this value. If a speaker gets the play command but doesn't know the time, a value of `0` will be used. The name is the name of the music file (see `info/music/sources`) and the number is the position in _seconds_.

```
{
  "name": 0
}
```

- `info/music/status`: The state of the playing music. [`play`/`p`] / [`s`/`stop`] If a speaker joins, it will wait for a command before it starts to do anything.

- `info/music/sources`: Contains the URIs to the music files in JSON format. The client will download these files. The identifier is the `objectid`, because the sound is bound to an object. So the _identifier_ is the _name_ of the object and the name on the local file system. See below for an example:

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

- `info/music/volume`: The master volume. This value will be of a value between `100` or `0`. `100` is the maximum value and `0` is the minimum value.

#### Clients

- `info/clients`: Global data for sending information of all the online clients to all devices. Clients that are online and are not **participating** in the audio network, carry the value `-1` for the distance and angle. Because this is a 2D field, all the clients will contain all the objects. See below:

```
{
  "clientid1": {
    "objectid1": {
      "distance": 0,
      "angle": 0
    },
    "objectid2": {
      "distance": 0,
      "angle": 0
    }
  },
  "clientid2": {
    "objectid1": {
      "distance": 0,
      "angle": 0
    },
    "objectid2": {
      "distance": 0,
      "angle": 0
    }
  },
  "clientid3": {
    "objectid1": {
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

#### Device status

- `info/clients/online`: If a clients comes online, it will post its `clientid`.
- `info/clients/offline`: If a client goes offline, it will post its `clientid`.

# General notes

- Client name (aka the `clientid`) - Speaker: The name for a speaker will be in the form of `speak_xxx`, where XXX is a random number. The minimal value is `0` and the maximal value of this random number is: `999`. Make sure that the devices can connect with its clientname. If not, that name already could be in use.
- An MQTT client can create topic strings of up to 65535 bytes.
- Parts between `[]` or the use of `*` after a sentence in this document usually mean that information needs to be added. Or that clarification is needed.
- Values used in client settings are as follows: `0` - `xxx` are legitimate values. `>0` i.e. `-1` are ignore values, these say that the client can ignore this value. See [Clients](#clients)
