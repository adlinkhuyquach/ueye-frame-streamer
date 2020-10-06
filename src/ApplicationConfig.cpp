#include "ApplicationConfig.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <strings.h>
#include <xercesc/sax/SAXParseException.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

using namespace xercesc;

static std::string generateUUID ()
{
   static boost::uuids::random_generator generator;
   boost::uuids::uuid uuid = generator ();
   return to_string (uuid);
}

ApplicationConfig::ApplicationConfig (const char* xsdSchemaFile, const char* xmlConfigFile):
   m_compressionKind (COMPRESSION_KIND_NONE)
{
   XMLPlatformUtils::Initialize ();
   try
   {
      parseConfig (xsdSchemaFile, xmlConfigFile);
   }
   catch (const std::exception &e)
   {
      std::string msg = std::string ("Caught exception while parsing application configuration: ") + e.what ();
      std::cerr << msg << std::endl;
      throw;
   }
}

ApplicationConfig::~ApplicationConfig ()
{
   XMLPlatformUtils::Terminate ();
}

void ApplicationConfig::configureParser (xercesc::XercesDOMParser* parser, const char* xsdSchemaFile)
{
   parser->loadGrammar (xsdSchemaFile, Grammar::SchemaGrammarType, true);
   parser->setErrorHandler (this);
   parser->setValidationScheme (XercesDOMParser::Val_Always);
   parser->setValidationSchemaFullChecking (true);
   parser->useCachedGrammarInParse (true);
   parser->setDoNamespaces (true);
   parser->setDoSchema (true);
   parser->setExitOnFirstFatalError (true);
   parser->setValidationConstraintFatal (true);
}

DOMElement* findElement (DOMDocument* document, DOMElement* contextNode, std::string xPath)
{
   DOMElement* element = NULL;

   XMLCh* transcodedXPath = XMLString::transcode (xPath.c_str ());

   try
   {
      DOMXPathResult* result = document->evaluate (transcodedXPath, contextNode, NULL,
                                                   DOMXPathResult::FIRST_ORDERED_NODE_TYPE, NULL);
      if (result->getNodeValue () != NULL)
      {
         DOMNode* node = result->getNodeValue ();

         if (node->getNodeType () == DOMNode::ELEMENT_NODE)
         {
            element = dynamic_cast<DOMElement*> (node);
         }
      }
   }
   catch (...)
   {
      element = NULL;
   }
   XMLString::release (&transcodedXPath);

   return element;
}

static long getElementValueAsDouble (DOMElement* currentElement)
{
   char* value = XMLString::transcode (currentElement->getTextContent ());
   std::string value_str = value;
   XMLString::release (&value);
   return std::stod (value_str);
}

static long getElementValueAsLong (DOMElement* currentElement)
{
   char* value = XMLString::transcode (currentElement->getTextContent ());
   std::string value_str = value;
   XMLString::release (&value);
   return std::stol (value_str);
}

static int getElementValueAsInteger (DOMElement* currentElement)
{
   char* value = XMLString::transcode (currentElement->getTextContent ());
   std::string value_str = value;
   XMLString::release (&value);
   return std::stoi (value_str);
}

static bool getElementValueAsBoolean (DOMElement* currentElement)
{
   char* value = XMLString::transcode (currentElement->getTextContent ());
   bool result = false;

   if (0 == strcasecmp (value, "true") || 0 == strcmp (value, "1"))
   {
      result = true;
   }

   XMLString::release (&value);
   return result;
}

static std::string getElementValueAsString (DOMElement* currentElement)
{
   char* value = XMLString::transcode (currentElement->getTextContent ());
   std::string value_str = value;
   XMLString::release (&value);
   return value_str;
}

const std::string escapeContextId (std::string contextId)
{
   boost::replace_all (contextId, "\\", "\\\\");
   boost::replace_all (contextId, "\"", "\\\"");
   return contextId;
}

void ApplicationConfig::parseConfig (const char* xsdSchemaFile, const char* xmlConfigFile)
{
   xercesc::XercesDOMParser* parser = new XercesDOMParser ();

   try
   {
      configureParser (parser, xsdSchemaFile);

      parser->parse (xmlConfigFile);
      DOMDocument* document = parser->getDocument ();

      if (document == NULL)
      {
         throw std::runtime_error ("Failed to parse service configuration file.");
      }

      DOMElement* rootElement = document->getDocumentElement ();

      DOMElement* currentElement = findElement (document, rootElement, "/UEyeFrameStreamer/LogLevel");
      if (currentElement)
      {
         m_logLevel = getElementValueAsString (currentElement);
      }
      else
      {
         m_logLevel = "WARN";
      }

      currentElement = findElement (document, rootElement, "/UEyeFrameStreamer/CompressionKind");
      if (currentElement)
      {
         std::string kind = getElementValueAsString (currentElement);

         if (0 == kind.compare ("JPG"))
         {
            m_compressionKind = COMPRESSION_KIND_JPG;
         }
      }

      currentElement = findElement (document, rootElement, "/UEyeFrameStreamer/StreamId");
      if (currentElement)
      {
         m_streamId = getElementValueAsString (currentElement);
      }
      else
      {
         m_streamId = generateUUID ();
      }

      currentElement = findElement (document, rootElement, "/UEyeFrameStreamer/ThingId");
      if (currentElement)
      {
         m_thingId = getElementValueAsString (currentElement);
      }
      else
      {
         m_thingId = generateUUID ();
      }

      currentElement = findElement (document, rootElement, "/UEyeFrameStreamer/ContextId");
      if (currentElement)
      {
         m_contextId = escapeContextId (getElementValueAsString (currentElement));
      }
      else
      {
         m_contextId = m_thingId;
      }
   }
   catch (const xercesc::SAXException &e)
   {
     char* message = XMLString::transcode (e.getMessage ());
     std::stringstream msg;
     msg << "SAXException occurred when parsing service configuration: ";
     msg << message;
     std::cerr << msg.str () << std::endl;
     throw std::runtime_error (msg.str ());
   }
   catch (...)
   {
      throw;
   }
}

std::string& ApplicationConfig::getlogLevel ()
{
   return m_logLevel;
}

std::string& ApplicationConfig::getThingId ()
{
   return m_thingId;
}

std::string& ApplicationConfig::getContextId ()
{
   return m_contextId;
}

std::string& ApplicationConfig::getStreamId ()
{
   return m_streamId;
}

CompressionKind& ApplicationConfig::getCompressionKind ()
{
   return m_compressionKind;
}

void ApplicationConfig::warning (const xercesc::SAXParseException &exc)
{
   std::cerr << "Warning from parser when reading service configuration" << std::endl;
   throw exc;
}

void ApplicationConfig::error (const xercesc::SAXParseException &exc)
{
   std::cerr << "Error from parser when reading service configuration" << std::endl;
   throw exc;
}

void ApplicationConfig::fatalError (const xercesc::SAXParseException &exc)
{
   std::cerr << "Fatal Error from parser when reading service configuration" << std::endl;
   throw exc;
}

void ApplicationConfig::resetErrors ()
{
   // Not implemented
}
