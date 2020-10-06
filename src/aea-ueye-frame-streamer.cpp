#include "ApplicationConfig.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "ueye.h"
#include "opencv2/opencv.hpp"
#include "JSonThingAPI.hpp"
#include "thingapi.hpp"
#include "ThingAPIException.hpp"

#include "edge/logger/Logger.h"
#include "edge/exception/Exception.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

using com::adlinktech::edge::logger::levels;
using namespace com::adlinktech::datariver;
using namespace com::adlinktech::iot;

extern const char* __progname;

volatile std::sig_atomic_t gSignalStatus = 0;

static void signalHandler (int signal)
{
   gSignalStatus = signal;
}

void usage ()
{
   std::cerr << "Usage: " << __progname << " <xsd-file>.xsd <config-file>.xml" << std::endl;
}

void abortOnError(const int status, const char *msg)
{
    if (status != 0)
    {
       throw com::adlinktech::edge::exception::Exception (std::string (msg) + ":" + std::to_string (status));
    }
}
void setLogLevel (std::string level)
{
   if (level == "TRACE")
   {
      com::adlinktech::edge::logger::setLogLevel (levels::TRACE);
   }
   else if (level == "DEBUG")
   {
      com::adlinktech::edge::logger::setLogLevel (levels::DEBUG);
   }
   else if (level == "INFO")
   {
      com::adlinktech::edge::logger::setLogLevel (levels::INFO);
   }
   else if (level == "WARN")
   {
      com::adlinktech::edge::logger::setLogLevel (levels::WARN);
   }
   else if (level == "ERROR")
   {
      com::adlinktech::edge::logger::setLogLevel (levels::ERROR);
   }
   else if (level == "CRITICAL")
   {
      com::adlinktech::edge::logger::setLogLevel (levels::CRITICAL);
   }
   else if (level == "OFF")
   {
      com::adlinktech::edge::logger::setLogLevel (levels::OFF);
   }
   else
   {
      throw com::adlinktech::edge::exception::Exception ("Invalid log level: " + level);
   }
}

static std::string generateInstanceDefintion (ApplicationConfig& config)
{
   std::stringstream def;
   def << "{ ";
   def << "\"id\": \"";
   def << config.getThingId ();
   def << "\", ";
   def << "\"classId\": \"FrameStreamer:com.adlinktech.aea-frame-streamer:v1.0\", ";
   def << "\"contextId\": \"";
   def << config.getContextId ();
   def << "\", ";
   def << "\"description\": \"Frame Streamer Application\"";
   def << " }";

   return def.str ();
}

static void populateDeviceInfoSequence (IOT_NVP_SEQ& nvpSeq, std::string& streamId)
{
   IOT_VALUE stream_id;
   stream_id.iotv_string (streamId);
   nvpSeq.emplace_back ("stream_id", stream_id);

   IOT_VALUE mac_address;
   mac_address.iotv_string ("");
   nvpSeq.emplace_back ("mac_address", mac_address);

   IOT_VALUE ip_address;
   ip_address.iotv_string ("");
   nvpSeq.emplace_back ("ip_address", ip_address);

   IOT_VALUE port;
   port.iotv_int32 (-1);
   nvpSeq.emplace_back ("port", port);

   IOT_VALUE uri;
   uri.iotv_string ("");
   nvpSeq.emplace_back ("uri", uri);

   IOT_VALUE manufacturer;
   manufacturer.iotv_string ("ADLINK");
   nvpSeq.emplace_back ("manufacturer", manufacturer);

   IOT_VALUE model;
   model.iotv_string ("Frame Streamer");
   nvpSeq.emplace_back ("model", model);

   IOT_VALUE serial;
   serial.iotv_string (streamId);
   nvpSeq.emplace_back ("serial", serial);

   IOT_VALUE fw_version;
   fw_version.iotv_string ("v1.0");
   nvpSeq.emplace_back ("fw_version", fw_version);

   IOT_VALUE dev_id;
   dev_id.iotv_string ("DDS");
   nvpSeq.emplace_back ("dev_id", dev_id);

   IOT_VALUE status;
   status.iotv_string ("com.vision.models.DEVICE_STATUS_ONLINE");
   nvpSeq.emplace_back ("status", status);

   IOT_VALUE kind;
   kind.iotv_string ("com.vision.models.DEVICE_KIND_UNKNOWN");
   nvpSeq.emplace_back ("kind", kind);

   IOT_VALUE protocol;
   protocol.iotv_string ("com.vision.models.INTERFACE_DDS");
   nvpSeq.emplace_back ("protocol", protocol);
}

static void populateDataSequence (IOT_NVP_SEQ& nvpSeq, std::string& streamId,
   uint32_t frameId, float frameRate, int frameWidth, int frameHeight,
   int frameChannels, int frameSize, CompressionKind compressionKind,
   std::vector<uint8_t>& frame)
{
   IOT_VALUE stream_id;
   stream_id.iotv_string (streamId);
   nvpSeq.emplace_back ("stream_id", stream_id);

   IOT_VALUE frame_id;
   frame_id.iotv_uint32 (frameId);
   nvpSeq.emplace_back ("frame_id", frame_id);

   // Calculate timestamp
   std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>
   (std::chrono::system_clock::now ().time_since_epoch ());
   IOT_VALUE timestamp;
   timestamp.iotv_int64 (ms.count ());
   nvpSeq.emplace_back ("timestamp", timestamp);

   IOT_VALUE data;
   data.iotv_byte_seq (frame);
   nvpSeq.emplace_back ("data", data);

   IOT_VALUE width;
   width.iotv_uint32 (frameWidth);
   nvpSeq.emplace_back ("width", width);

   IOT_VALUE height;
   height.iotv_uint32 (frameHeight);
   nvpSeq.emplace_back ("height", height);

   IOT_VALUE channels;
   channels.iotv_uint32 (frameChannels);
   nvpSeq.emplace_back ("channels", channels);

   IOT_VALUE size;
   size.iotv_uint32 (frameSize);
   nvpSeq.emplace_back ("size", size);

   IOT_VALUE format;
   format.iotv_string ("com.vision.models.PIXEL_FORMAT_BGR8");
   nvpSeq.emplace_back ("format", format);

   IOT_VALUE compression;

   switch (compressionKind)
   {
      case COMPRESSION_KIND_NONE:
      {
         compression.iotv_string ("com.vision.model.COMPRESSION_KIND_NONE");
         break;
      }
      case COMPRESSION_KIND_JPG:
      {
         compression.iotv_string ("com.vision.model.COMPRESSION_KIND_JPG");
         break;
      }
   }
   nvpSeq.emplace_back ("compression", compression);

   IOT_VALUE framerate;
   framerate.iotv_float32 (frameRate);
   nvpSeq.emplace_back ("framerate", framerate);
}

int main (int argc, char **argv)
{
   if (argc < 3)
   {
      usage ();
      return 1;
   }

   // Install signal handlers
   signal (SIGTERM, signalHandler);
   signal (SIGINT, signalHandler);

   int returnStatus = 0;
   com::adlinktech::edge::logger::Logger edgeLogger (levels::INFO);

   try
   {
      // Read application configuration
      EDGE_LOG (levels::INFO, "Parsing Config");
      ApplicationConfig config (argv[1], argv[2]);
      setLogLevel (config.getlogLevel ());

      EDGE_LOG (levels::INFO, "Creating data river Object");
      // Setup Data River
      DataRiver river = DataRiver::getInstance ();

      // Register tag groups
      JSonTagGroupRegistry tgr;
      tgr.registerTagGroupsFromURI ("file:///definitions/TagGroup/com.vision.data/VideoFrameTagGroup.json");
      tgr.registerTagGroupsFromURI ("file:///definitions/TagGroup/com.vision.data/DeviceInfoTagGroup.json");
      river.addTagGroupRegistry (tgr);
      EDGE_LOG (levels::INFO, "Registered tag groups");

      // Register thing class
      JSonThingClassRegistry tcr;
      tcr.registerThingClassesFromURI ("file:///definitions/ThingClass/com.adlinktech.aea-frame-streamer/FrameStreamerClass.json");
      river.addThingClassRegistry (tcr);
      EDGE_LOG (levels::INFO, "Registred thing class");

      // Create thing on river
      JSonThingProperties tp;
      tp.readPropertiesFromString (generateInstanceDefintion (config));
      Thing myThing = river.createThing (tp);
      EDGE_LOG (levels::INFO, "Created Thing");

      // Populate and write DeviceInfo tag group
      IOT_NVP_SEQ deviceInfoNvpSeq;
      populateDeviceInfoSequence (deviceInfoNvpSeq, config.getStreamId ());
      myThing.write ("DeviceInfo", config.getStreamId (), deviceInfoNvpSeq);

      /* initialize first available camera */
      HIDS hCam = 0;
      int nRet = is_InitCamera(&hCam, 0);
      abortOnError(nRet, "InitCamera failed with error code: ");

      /* receive sensor info */
      SENSORINFO sensorInfo;
      nRet = is_GetSensorInfo(hCam, &sensorInfo);
      abortOnError(nRet, "Receving sensor info failed with error code: ");

      /* receive camera image size */
      IS_RECT rectAoi;
      nRet = is_AOI(hCam, IS_AOI_IMAGE_GET_AOI, &rectAoi, sizeof(rectAoi));
      abortOnError(nRet, "Receving image aoi failed with error code: ");

      /* set color mode */
      nRet = is_SetColorMode(hCam, IS_CM_BGR8_PACKED);
      abortOnError(nRet, "Setting color mode failed with error code: ");

      /* allocate image buffer */
      char *pMemoryBuffer;
      int nMemoryId;
      nRet = is_AllocImageMem(hCam, rectAoi.s32Width, rectAoi.s32Height, 24, &pMemoryBuffer, &nMemoryId);
      abortOnError(nRet, "Allocating image memory failed with error code: ");

      /* set allocated image buffer active */
      nRet = is_SetImageMem(hCam, pMemoryBuffer, nMemoryId);
      abortOnError(nRet, "Setting image memory active failed with error code: ");


      // Print the model name of the camera.
      BOARDINFO pInfo;
      nRet = is_GetCameraInfo (hCam, &pInfo);
      if (nRet == IS_SUCCESS)
      {
         EDGE_LOG (levels::INFO, "Using device {}", std::string (pInfo.ID));
      }


      EDGE_LOG (levels::INFO, "Streaming frames at stream_id: {}", config.getStreamId ());
      int frameNo = 0;
      while (gSignalStatus == 0)
      {
         try
         {
            /* capture a single frame */
            nRet = is_FreezeVideo (hCam, IS_WAIT);
            // nRet = is_CaptureVideo (hCam, IS_WAIT);
            // int width = 0, height = 0, pnBits = 0, pnPitch  = 0;
            // nRet = is_InquireImageMem (hCam, pMemoryBuffer, nMemoryId, &width, &height, &pnBits, &pnPitch);
            
            // Read next frame
            if (nRet == IS_SUCCESS)
            {
               double frameRate = 0;
               nRet = is_GetFramesPerSecond (hCam, &frameRate);
               if (nRet == IS_SUCCESS)
               {
                  EDGE_LOG (levels::TRACE, "Streaming frames at {} fps (stream_id: {} )", frameRate, config.getStreamId ());
               }

               try
               {
                  cv::Mat frame;
                  EDGE_LOG (levels::TRACE, "Converting Frame");

                  // Create an OpenCV image out of pylon image
                  frame = cv::Mat(rectAoi.s32Height, rectAoi.s32Width, CV_8UC3, pMemoryBuffer);
                  //frame = cv::Mat(height, width, CV_8UC3, pMemoryBuffer);

                  // If the frame is empty, break immediately
                  if (frame.empty ())
                  {
                     EDGE_LOG (levels::TRACE, "Breaking because of empty frame");
                     break;
                  }

                  std::vector<uint8_t> data;

                  if (config.getCompressionKind () == CompressionKind::COMPRESSION_KIND_JPG)
                  {
                     cv::imencode (".jpg", frame, data);
                  }
                  else
                  {
                     EDGE_LOG (levels::TRACE, "Preparing frame data");
                     data.assign (frame.data, frame.data + (frame.total () * frame.elemSize ()));
                  }

                  // Populate and write VideoFrame tag group
                  IOT_NVP_SEQ dataNvpSeq;
                  populateDataSequence (dataNvpSeq, config.getStreamId (), frameNo,
                     frameRate, frame.cols, frame.rows, frame.channels (), data.size (),
                     config.getCompressionKind (), data);

                  try
                  {
                     EDGE_LOG (levels::TRACE, "Publishing to river");
                     myThing.write ("VideoFrameData", config.getStreamId (), dataNvpSeq);
                  }
                  catch (const com::adlinktech::datariver::ThingAPIException &e)
                  {
                     EDGE_LOG (levels::ERROR, "An exception occurred when writing frame ({})  to Data River: {}", frameNo, e.what ());
                     returnStatus = 1;
                  }

                  frameNo++;
               }
               catch(const std::exception& e)
               {
                  EDGE_LOG (levels::ERROR, "An exception occurred when converting frame : {}", e.what ());
               }
            }
         }
         catch (const std::exception& e)
         {
            EDGE_LOG (levels::ERROR, "Error when Retrieving Frame: {}", e.what ());
         }
      }

      /* close camera */
      nRet = is_ExitCamera(hCam);
      abortOnError(nRet, "exit camera failed with error code: ");

      EDGE_LOG (levels::INFO, "Finished streaming frames");
   }
   catch (const com::adlinktech::datariver::ThingAPIException &e)
   {
      std::cerr << "An exception occurred: " << e.what () << std::endl;
      returnStatus = 1;
   }
   catch (com::adlinktech::edge::exception::Exception e)
   {
      std::cerr << "An exception occurred: " << e.what () << std::endl;
      returnStatus = 1;
   }
   catch(const std::exception& e)
   {
      std::cerr << "An exception occurred: " << e.what () << std::endl;
      returnStatus = 1;
   }
   catch (...)
   {
      std::cerr << "Unknown exception occurred"<< std::endl;
      returnStatus = 1;
   }

   return returnStatus;
}
