.. _`ThingConcept`:

#############
Thing Concept
#############

This section describes how AEA uEye Frame Streamer is represented as a Thing on the Data River.

Thing Definition
****************

The thangId and contextId are defined in configuration or randomly generated.

.. code-block:: JSON

    {
        "id": "Neon-i-streamer-thing",
        "classId": "FrameStreamer:com.adlinktech.aea-frame-streamer:v1.0",
        "contextId": "Neon-i-streamer-context",
        "description": "Frame Streamer Application"
    }


Thing Class Definition
**********************

The ThingClass defined with 2 output tagGroup.

.. code-block:: JSON

    {
        "name" : "FrameStreamer",
        "context" : "com.adlinktech.aea-frame-streamer",
        "version" : "v1.0",
        "description" : "Class definition for a frame streaming application",
        "outputs" :
        [
            {
            "name" : "VideoFrameData",
            "tagGroupId" : "VideoFrame:com.vision.data:v1.0"
            },
            {
            "name" : "DeviceInfo",
            "tagGroupId" : "DeviceInfo:com.vision.data:v1.0"
            }
        ]
    }


Tag Group Definition
********************

The "VideoFrameData" tagGroup is defined as following.

.. code-block:: JSON

    {
        "name":"VideoFrame",
        "context":"com.vision.data",
        "qosProfile":"video",
        "version":"v1.0",
        "description":"Video frame sample",
        "tags":[
            {
                "name":"stream_id",
                "description":"Stream publisher ID",
                "kind":"STRING",
                "unit":"UUID"
            },
            {
                "name":"frame_id",
                "description":"Frame sample ID",
                "kind":"UINT32",
                "unit":"NUM"
            },
            {
                "name":"timestamp",
                "description":"Time of image capture event",
                "kind":"INT64",
                "unit":"time"
            },
            {
                "name":"data",
                "description":"Video frame data",
                "kind":"TYPE_BYTE_SEQ",
                "unit":"Frame data"
            },
            {
                "name":"width",
                "description":"Frame width",
                "kind":"UINT32",
                "unit":"Pixels"
            },
            {
                "name":"height",
                "description":"Frame height",
                "kind":"UINT32",
                "unit":"Pixels"
            },
            {
                "name":"channels",
                "description":"Channels",
                "kind":"UINT32",
                "unit":"Number of channels"
            },
            {
                "name":"size",
                "description":"Data size",
                "kind":"UINT32",
                "unit":"Size"
            },
            {
                "name":"format",
                "description":"Pixel format using OpenCV Definitions",
                "kind":"STRING",
                "unit":"PixelFormat"
            },
            {
                "name":"compression",
                "description":"Compression technology used for video frame",
                "kind":"STRING",
                "unit":"CompressionKind"
            },
            {
                "name":"framerate",
                "description":"Frame transmission frequency",
                "kind":"FLOAT32",
                "unit":"fps"
            }
        ]
    }


The "DeviceInfo" tagGroup definition is described below.

.. code-block:: JSON

    {
        "name":"DeviceInfo",
        "context":"com.vision.data",
        "qosProfile":"state",
        "version":"v1.0",
        "description":"Vision Device properties",
        "tags":[
            {
                "name":"stream_id",
                "description":"Stream publisher ID",
                "kind":"STRING",
                "unit":"UUID"
            },
            {
                "name":"mac_address",
                "description":"Host address",
                "kind":"STRING",
                "unit":"n/a"
            },
            {
                "name":"ip_address",
                "description":"Host machine IP Address",
                "kind":"STRING",
                "unit":"n/a"
            },
            {
                "name":"port",
                "description":"Connection port (optional)",
                "kind":"INT32",
                "unit":"n/a"
            },
            {
                "name":"uri",
                "description":"Video Interface URI (rtsp://xx/h264)",
                "kind":"STRING",
                "unit":"n/a"
            },
            {
                "name":"manufacturer",
                "description":"Vision Device manufacturer",
                "kind":"STRING",
                "unit":"n/a"
            },
            {
                "name":"model",
                "description":"Vision Device model",
                "kind":"STRING",
                "unit":"n/a"
            },
            {
                "name":"serial",
                "description":"Vision Device serial identifier",
                "kind":"STRING",
                "unit":"n/a"
            },
            {
                "name":"fw_version",
                "description":"Vision Device firmware version",
                "kind":"STRING",
                "unit":"n/a"
            },
            {
                "name":"dev_id",
                "description":"Vision Device host interface (ex. /dev/video0 or /dev/ttyUSB0)",
                "kind":"STRING",
                "unit":"n/a"
            },
            {
                "name":"status",
                "description":"DeviceStatus enum",
                "kind":"STRING",
                "unit":"DeviceStatus"
            },
            {
                "name":"kind",
                "description":"Vision device kind enum",
                "kind":"STRING",
                "unit":"DeviceKind"
            },
            {
                "name":"protocol",
                "description":"ProtocolKind enum describing how the device communicates",
                "kind":"STRING",
                "unit":"ProtocolKind"
            }
        ]
    }



.. END
