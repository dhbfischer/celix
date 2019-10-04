# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.


# - Try to find OpenSSL
# Once done this will define
#  OpenSSL_FOUND - System has OpenSSL
#  OpenSSL_INCLUDE_DIRS - The OpenSSL include directories
#  OpenSSL_LIBRARIES - The libraries needed to use OpenSSL
#  OpenSSL::lib - Imported target for OpenSSL

find_path(OpenSSL_INCLUDE_DIR ssl.h crypto.h
          /usr/include/openssl
          /usr/local/include/openssl 
          /usr/local/opt/openssl/include/openssl
          ${OpenSSL_DIR}/include/openssl)

find_library(OpenSSL_LIBRARY NAMES ssl
             PATHS /usr/lib /usr/local/lib  /usr/local/opt/openssl/lib ${OpenSSL_DIR}/lib)

find_library(Crypto_LIBRARY NAMES crypto
             PATHS /usr/lib /usr/local/lib  /usr/local/opt/openssl/lib ${OpenSSL_DIR}/lib)

set(OPENSSL_LIBRARIES ${OpenSSL_LIBRARY} ${CRYPTO_LIBRARY})
set(OPENSSL_INCLUDE_DIRS ${OpenSSL_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set OPENSSL_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(OpenSSL  DEFAULT_MSG
              OpenSSL_LIBRARY Crypto_LIBRARY OpenSSL_INCLUDE_DIR)

mark_as_advanced(OpenSSL_INCLUDE_DIR OpenSSL_LIBRARY Crypto_LIBRARY)

if (OpenSSL_FOUND AND NOT TARGET OpenSSL::lib)
    add_library(OpenSSL::lib SHARED IMPORTED)
    set_target_properties(OpenSSL::lib PROPERTIES
            IMPORTED_LOCATION "${OpenSSL_LIBRARY};${Crypto_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenSSL_INCLUDE_DIR}"
    )
endif ()