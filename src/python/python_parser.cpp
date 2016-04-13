/*
 *  Copyright (c) 2013 Croatia Control Ltd. (www.crocontrol.hr)
 *
 *  This file is part of Asterix.
 *
 *  Asterix is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Asterix is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Asterix.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * AUTHORS: Damir Salantic, Croatia Control Ltd.
 *
 */

//#include <stdlib.h>
//#include <memory.h>
//#include <stdio.h>
//#include <string>

#define LOGDEBUG(cond, ...)
#define LOGERROR(cond, ...)


#include <Python.h>

#include "python_parser.h"
#include "AsterixDefinition.h"
#include "XMLParser.h"
#include "InputParser.h"
#include "../engine/converterengine.hxx"
#include "../engine/channelfactory.hxx"


static AsterixDefinition* pDefinition = NULL;
bool gFiltering = false;
bool gSynchronous = false;
const char* gAsterixDefinitionsFile = NULL;
bool gVerbose = false;
bool gForceRouting = false;
int gHeartbeat = 0;


/*
 * Initialize Asterix Python wrapper
 */
int python_start(const char* ini_file_path)
{
  std::string strFilePath;
  std::string strInputFile;
  char inputFile[256];

  int ret = 0;
  // initialize Fulliautomatix
//  Tracer::Configure(pPrintFunc);
  pDefinition = new AsterixDefinition();

  if (ini_file_path)
  {
    strFilePath = ini_file_path;
#ifdef LINUX
    strFilePath += "/";
#else
    strFilePath += "\\";
#endif
  }

  strInputFile = strFilePath + "asterix.ini";

  gAsterixDefinitionsFile = strInputFile.c_str();


  // open asterix.ini file
  FILE* fpini = fopen(strInputFile.c_str(), "rt");
  if (!fpini)
  {
//    Tracer::Error("Failed to open %s\n", strInputFile.c_str());
    return 1;
  }
  while(fgets(inputFile, sizeof(inputFile), fpini))
  {
    // remove trailing /n from filename
    int lastChar = strlen(inputFile)-1;
    while (lastChar>=0 && (inputFile[lastChar] == '\r' || inputFile[lastChar] == '\n'))
    {
      inputFile[lastChar] = 0;
      lastChar--;
    }
    if (lastChar <= 0)
      continue;

    strInputFile = strFilePath + inputFile;

    FILE* fp = fopen(strInputFile.c_str(), "rt");
    if (!fp)
    {
//      Tracer::Error("Failed to open definitions file: %s\n", inputFile);
      continue;
    }

    // parse format file
    XMLParser Parser;
    if (!Parser.Parse(fp, pDefinition, strInputFile.c_str()))
    {
//      Tracer::Error("Failed to parse definitions file: %s\n", strInputFile.c_str());
      ret = 1;
    }

    fclose(fp);
  };

  fclose(fpini);

  return ret;
}


PyObject *python_parse(const unsigned char* pBuf, unsigned int len)
{
  InputParser inputParser(pDefinition);
  AsterixData* pData = inputParser.parsePacket(pBuf, len);
  if (pData)
  { // convert to Python format

	  PyObject *lst = pData->getData();
    // delete AsterixData
	  delete pData;
	  return lst;
  }
  return NULL;
}


void asterix_start(const char* ini_filename, const char* filename)
{
	//std::string strDefinitions = ini_filename;
	std::string strFileInput = filename;
	std::string strIPInput;
	std::string strFilterFile;
	std::string strInputFormat = "ASTERIX_PCAP";
	std::string strOutputFormat = "ASTERIX_TXT";
	bool bListDefinitions = false;
	bool bLoopFile = false;


	  gAsterixDefinitionsFile = ini_filename;

	LOGERROR(1, "Ini file = %s\n", ini_filename);

/*
	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		if ((arg == "-h") || (arg == "--help"))
		{
				show_usage(argv[0]);
				return 0;
		}
		else if ((arg == "-v") || (arg == "--sync"))
		{
			gVerbose = true;
		}
		else if ((arg == "-s") || (arg == "--verbose"))
		{
			gSynchronous = true;
		}
		else if ((arg == "-o") || (arg == "--loop"))
		{
			bLoopFile = true;
		}
		else if ((arg == "-L") || (arg == "--list"))
		{
			bListDefinitions = true;
		}
		else if ((arg == "-LF") || (arg == "--filter"))
		{
			if (i >= argc-1)
			{
				std::cerr << "Error: " + arg + " option requires one argument." << std::endl;
				return 1;
			}
			strFilterFile = argv[++i];
			gFiltering = true;
		}
		else if ((arg == "-P") || (arg == "--pcap"))
		{
			if (strInputFormat != "ASTERIX_RAW")
			{
				std::cerr << "Error: Option -P not allowed because input format already defined as "+strInputFormat << std::endl;
				return 1;
			}
			strInputFormat = "ASTERIX_PCAP";
		}
		else if ((arg == "-O") || (arg == "--oradis"))
		{
			if (strInputFormat != "ASTERIX_RAW")
			{
				std::cerr << "Error: Option -O not allowed because input format already defined as "+strInputFormat << std::endl;
				return 1;
			}
			strInputFormat = "ASTERIX_ORADIS_RAW";
		}
		else if ((arg == "-R") || (arg == "--oradispcap"))
		{
			if (strInputFormat != "ASTERIX_RAW")
			{
				std::cerr << "Error: Option -R not allowed because input format already defined as "+strInputFormat << std::endl;
				return 1;
			}
			strInputFormat = "ASTERIX_ORADIS_PCAP";
		}
		else if ((arg == "-F") || (arg == "--final"))
		{
			if (strInputFormat != "ASTERIX_RAW")
			{
				std::cerr << "Error: Option -F not allowed because input format already defined as "+strInputFormat << std::endl;
				return 1;
			}
			strInputFormat = "ASTERIX_FINAL";
		}
		else if ((arg == "-H") || (arg == "--hdlc"))
		{
			if (strInputFormat != "ASTERIX_RAW")
			{
				std::cerr << "Error: Option -H not allowed because input format already defined as "+strInputFormat << std::endl;
				return 1;
			}
			strInputFormat = "ASTERIX_HDLC";
		}
		else if ((arg == "-l") || (arg == "--line"))
		{
			if (strOutputFormat != "ASTERIX_TXT")
			{
				std::cerr << "Error: Option -l not allowed because output format already defined as "+strOutputFormat << std::endl;
				return 1;
			}
			strOutputFormat = "ASTERIX_OUT";
		}
		else if ((arg == "-x") || (arg == "--xml"))
		{
			if (strOutputFormat != "ASTERIX_TXT")
			{
				std::cerr << "Error: Option -x not allowed because output format already defined as "+strOutputFormat << std::endl;
				return 1;
			}
			strOutputFormat = "ASTERIX_XML";
		}
		else if ((arg == "-j") || (arg == "--json"))
		{
			if (strOutputFormat != "ASTERIX_TXT")
			{
				std::cerr << "Error: Option -j not allowed because output format already defined as "+strOutputFormat << std::endl;
				return 1;
			}
			strOutputFormat = "ASTERIX_JSON";
		}
		else if ((arg == "-jh") || (arg == "--jsonh"))
		{
			if (strOutputFormat != "ASTERIX_TXT")
			{
				std::cerr << "Error: Option -jh not allowed because output format already defined as "+strOutputFormat << std::endl;
				return 1;
			}
			strOutputFormat = "ASTERIX_JSONH";
		}
		else if ((arg == "-k") || (arg == "--kml"))
		{
			if (strOutputFormat != "ASTERIX_TXT")
			{
				std::cerr << "Error: Option -k not allowed because output format already defined as "+strOutputFormat << std::endl;
				return 1;
			}
			strOutputFormat = "ASTERIX_KML";
		}
		else if ((arg == "-d") || (arg == "--definitions"))
		{
			if (i >= argc-1)
			{
				std::cerr << "Error: " + arg + " option requires one argument." << std::endl;
				return 1;
			}

			strDefinitions = argv[++i];
		}
		else if ((arg == "-f"))
		{
			if (i >= argc-1)
			{
				std::cerr << "Error: " + arg + " option requires one argument." << std::endl;
				return 1;
			}
			strFileInput = argv[++i];
		}
		else if ((arg == "-i"))
		{
			if (i >= argc-1)
			{
				std::cerr << "Error: " + arg + " option requires one argument." << std::endl;
				return 1;
			}
			strIPInput = argv[++i];
		}
	}

	// definitions file
	gAsterixDefinitionsFile = strDefinitions.c_str();

	// check for definitions file
	FILE *tmp = fopen(gAsterixDefinitionsFile, "r");
	if (tmp == NULL)
	{
		std::cerr << "Error: Asterix definitions file " +strDefinitions+ " not found." << std::endl;
		exit (2);
	}
	fclose(tmp);
*/
	// Create input string
	std::string strInput = "std 0 ";
	if (!strFileInput.empty() && !strIPInput.empty())
	{
		strInput = "std 0 ASTERIX_RAW";
	}
	if (!strFileInput.empty())
	{
		strInput = "disk " + strFileInput + ":0:";

		if (bLoopFile)
		{
			strInput += "65 ";
		}
		else
		{
			strInput += "1 ";
		}
	}
	else if (!strIPInput.empty())
	{
		// count ':'
		int cntr=0;
		int indx=0;
		while((indx=strIPInput.find(':', indx+1)) >= 0)
		{
			cntr++;
		}

		if (cntr == 2)
			strInput = "udp " + strIPInput + "::S ";
		else if (cntr == 3)
			strInput = "udp " + strIPInput + ":S ";
		else {
			//std::cerr << "Error: Wrong input address format  (shall be: mcastaddress:ipaddress:port[:srcaddress]" << std::endl;
			exit (3);
		}
	}

	strInput += strInputFormat;

	// Create output string
	std::string strOutput = "std 0 " + strOutputFormat;

	const char         *inputChannel=NULL;
	const char         *outputChannel[CChannelFactory::MAX_OUTPUT_CHANNELS];
	unsigned int 		chFailover = 0;
	unsigned int        nOutput = 1; // Total number of output channels

	inputChannel = strInput.c_str();
	outputChannel[0] = strOutput.c_str();

	// Print out options
	//LOGDEBUG(inputChannel, "Input channel description: %s\n", inputChannel);

	for (unsigned int i=0; i<nOutput; i++)
	{
		//LOGDEBUG(outputChannel[i], "Output channel %d description: %s\n", i+1, outputChannel[i]);
	}

	//gHeartbeat = abs(gHeartbeat); // ignore negative values
	//    LOGDEBUG(1, "Heart-beat: %d\n", gHeartbeat);

    // Finally execute converter engine
    if (CConverterEngine::Instance()->Initialize(inputChannel, outputChannel, nOutput, chFailover))
    {
    	if (bListDefinitions)
    	{ // Parse definitions file and print all items
			CBaseFormatDescriptor* desc = CChannelFactory::Instance()->GetInputChannel()->GetFormatDescriptor();
			if (desc == NULL)
			{
    			//std::cerr << "Error: Format description not found." << std::endl;
    			exit (2);
			}
			//std::cout << desc->printDescriptor();
    	}
    	else
    	{
    		CConverterEngine::Instance()->Start();
    	}
    }
    else
    {
        //LOGERROR(1, "Couldn't initialize asterix engine.\n");
        return;
    }

    CConverterEngine::DeleteInstance();
}
  
  
  
