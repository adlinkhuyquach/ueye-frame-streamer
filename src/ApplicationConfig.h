#ifndef APPLICATIONCONFIG_H_
#define APPLICATIONCONFIG_H_

#include <string>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

enum CompressionKind
{
   COMPRESSION_KIND_NONE,
   COMPRESSION_KIND_JPG
};

class ApplicationConfig : public xercesc::ErrorHandler
{
   public:
      ApplicationConfig (const char* xsdSchemaFile, const char* xmlConfigFile);
      ~ApplicationConfig ();

      void warning (const xercesc::SAXParseException &exc);
      void error (const xercesc::SAXParseException &exc);
      void fatalError (const xercesc::SAXParseException &exc);
      void resetErrors ();

      std::string& getlogLevel ();
      std::string& getThingId ();
      std::string& getContextId ();
      std::string& getStreamId ();
      CompressionKind& getCompressionKind ();

   private:
      void configureParser (xercesc::XercesDOMParser* parser, const char* xsdSchemaFile);
      void parseConfig (const char* xsdSchemaFile, const char* xmlConfigFile);

      std::string m_logLevel;
      std::string m_thingId;
      std::string m_contextId;
      std::string m_streamId;
      CompressionKind m_compressionKind;
};

#endif /* APPLICATIONCONFIG_H_ */
