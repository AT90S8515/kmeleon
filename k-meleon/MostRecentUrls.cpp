/*
*  Copyright (C) 2001 Jeff Doozan
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2, or (at your option)
*  any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "StdAfx.h"
#include "nsIFile.h"
#include "nsILocalFile.h"
#include "nsAppDirectoryServiceDefs.h"
#include "MostRecentUrls.h"
#include "MfcEmbed.h"
extern CMfcEmbedApp theApp;

CMostRecentUrls::CMostRecentUrls() {

   m_maxURLs = theApp.preferences.GetInt("kmeleon.MRU.maxURLs", 16); 
   if (m_maxURLs <= 0) {
      m_maxURLs=0;
      m_URLCount=0;
      m_URLs=NULL;
      return;
   }

   m_URLs = new char*[m_maxURLs];
   
   char sPref[20] = "kmeleon.MRU.URL", sBuf[5];
   char *sCount = sPref + strlen(sPref);
   int x=0;

   do {
      itoa(x, sBuf, 10);
      strcpy(sCount, sBuf);                              // create  "kmeleon.MRU.URL##" string
      int len = theApp.preferences.GetString(sPref, NULL, "");
      if (len > 0) {
         m_URLs[x] = new char[len+1];
         theApp.preferences.GetString(sPref, m_URLs[x], "");
      }
      else
         break;
   } while (++x<m_maxURLs);
   
   m_URLCount = x;
   // nullify the empty entries
   for(;x<m_maxURLs;x++)
      m_URLs[x] = NULL;
}

CMostRecentUrls::~CMostRecentUrls() {
   if (m_maxURLs < 1) return;

   char sPref[20] = "kmeleon.MRU.URL", sBuf[5];
   char *sCount = sPref + strlen(sPref);

   theApp.preferences.SetInt("kmeleon.MRU.maxURLs", m_maxURLs); 

   for (int x=0; x<m_URLCount; x++) {
      itoa(x, sBuf, 10);
      strcpy(sCount, sBuf);                              // create  "kmeleon.MRU.URL##" string
      theApp.preferences.SetString(sPref, m_URLs[x]);
   }
   
   for (x=0;x<m_maxURLs;x++)
      if(m_URLs[x]) delete m_URLs[x];

   delete m_URLs;
}

char * CMostRecentUrls::GetURL(int aInx) {
    if (aInx < m_URLCount) return m_URLs[aInx];
    return NULL;
}

void CMostRecentUrls::AddURL(const char * aURL) {

   if (m_maxURLs<1) return;

   // check list for previous entry
   for (int x=0; x<m_URLCount-1; x++)	{
      if(m_URLs[x]) {
         if(strcmpi(m_URLs[x], aURL) == 0)
            break; 
      }
   }

   // if no match
   if((x==m_URLCount-1) || (m_URLCount == 0)) {
      if (m_URLCount==m_maxURLs)
         delete m_URLs[x];            // if list is full, remove the bottom entry
      else {                          // otherwise add a new entry and increase the count
         m_URLCount++;
         x++;
      }
   }

   // shift everything down
   while (x) {
      m_URLs[x] = m_URLs[x-1];
      x--;
   }

	// add the new url
   m_URLs[0] = _strdup(aURL);
}