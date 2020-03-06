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
#ifndef __HTTP_PARSE__
#define __HTTP_PARSE__

// Method supported by the parser
enum http_method {
   HTTP_GET,
   HTTP_UNSUPPORTED,
};


// The maximum size of the target string.
#define MAX_TARGET_SIZE 1024

// describes a http request
typedef struct
{
   enum http_method method;
   int http_major;
   int http_minor;
   char target[MAX_TARGET_SIZE];
} http_request;

/** Parses a http request line.
    
    @param in request_line the line to parse (as a null terminated string)
    @param out request a valid pointer to a http_request that will be filled by the function

    @return -1 if error and 0 on success. If the error is an
    unsupported http method, then the method field of request will be set to HTTP_UNSUPPORTED
*/
int parse_http_request(const char *request_line , http_request *request);

#endif
