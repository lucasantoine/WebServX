// Copyright © 2019, Université de Lille.
// Created: 22 janvier 2019
// 
// Corresponding author: Michael Hauspie <michael.hauspie@univ-lille.fr>
// For full author list, see AUTHORS.md file
// 
// This software is governed by the CeCILL license under French law and
// abiding by the rules of distribution of free software.  You can  use,
// modify and/ or redistribute the software under the terms of the CeCILL
// license as circulated by CEA, CNRS and INRIA at the following URL
// "http://www.cecill.info".
// 
// As a counterpart to the access to the source code and  rights to copy,
// modify and redistribute granted by the license, users are provided only
// with a limited warranty  and the software author,  the holder of the
// economic rights,  and the successive licensors  have only  limited
// liability.
// 
// In this respect, the user attention is drawn to the risks associated
// with loading,  using,  modifying and/or developing or reproducing the
// software by the user in light of its specific status of free software,
// that may mean  that it is complicated to manipulate,  and  that  also
// therefore means  that it is reserved for developers  and  experienced
// professionals having in-depth computer knowledge. Users are therefore
// encouraged to load and test the software suitability as regards their
// requirements in conditions enabling the security of their systems and/or
// data to be ensured and,  more generally, to use and operate it in the
// same conditions as regards security.
// 
// The fact that you are presently reading this means that you have had
// knowledge of the CeCILL license and that you accept its terms.
#include <string.h>
#include <stdio.h>

#include "http_parse.h"

#define min(a,b) ((a) < (b) ? (a) : (b))
#define in_range(a,b,c) ((a) < (b) ? 0 : ((a) > (c) ? 0 : 1))

int parse_http_request(const char *request_line , http_request *request)
{

   if (strncmp(request_line, "GET ", 4) != 0)
   {
      request->method = HTTP_UNSUPPORTED;
      return 0;
   }
   request->method = HTTP_GET;
   /* Find the target start */
   const char *target = strchr(request_line, ' ');
   if (target == NULL)
      return 0;
   target++;
   /* Find target end and copy target to request */
   char *target_end = strchr(target, ' ');
   if (target_end == NULL)
      return 0;
   int size = min(target_end - target, MAX_TARGET_SIZE);
   strncpy(request->target, target, size);
   /* If target is more than size, \0 is not add to dst, so... */
   request->target[size] = '\0';

   /* Now http version (only support HTTP/M.m version format) */
   /* Quote from RFC:
      Additionally, version numbers have been restricted to
      single digits, due to the fact that implementations are known to
      handle multi-digit version numbers incorrectly.
   */
   char *version = target_end + 1;
   if (strncmp(version, "HTTP/", 5) != 0)
      return 0;
   if (!in_range(version[5], '0', '9')) // major
      return 0;
   if (version[6] != '.') // mandatory dot
      return 0;
   if (!in_range(version[7], '0', '9')) // minor
      return 0;
   request->http_major = version[5] - '0';
   request->http_minor = version[7] - '0';
   return 1;
}

#ifdef COMPILE_MAIN
int main(int argc, char **argv)
{
   if (argc != 2)
   {
      fprintf(stderr, "usage: %s http_request_line\n", argv[0]);
      return 1;
   }

   http_request r;
   if (!parse_http_request(argv[1], &r))
   {
      fprintf(stderr, "Fails to parse request\n");
      if (r.method == HTTP_UNSUPPORTED)
         fprintf(stderr, "Unsupported method\n");
      return 1;
   }
   printf("request line: %s\n", argv[1]);
   printf("method: %s\n", r.method == HTTP_GET ? "GET" : "UNSUPPORTED");
   printf("target: %s\n", r.target);
   printf("version: %d.%d\n", r.http_major, r.http_minor);
   return 0;
}

#endif
